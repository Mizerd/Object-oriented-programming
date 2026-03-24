#include "benchmark.h"
#include "constants.h"
#include "fileGenerator.h"
#include "fileio.h"
#include "input.h"
#include "menu.h"
#include "sorting.h"
#include "splitting.h"
#include "student_container.h"
#include "table.h"
#include "timer.h"
#include "utf8.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

Weights chooseWeights() {
    Weights w{cfg::kDefaultNdWeight, cfg::kDefaultEgzWeight};

    std::cout << "\nSvorio pasirinkimas:\n"
              << "Numatyta: ND=0.4, Egz=0.6\n";

    const char ans = inp::readCharFromSet(
        "Naudoti numatytus svorius? (t/n) [t]: ", "tn", 't');

    if (ans == 't') return w;

    while (true) {
        try {
            const double nd = inp::readDoubleInRange("Įveskite ND svorį (0..1): ", 0.0, 1.0);
            const double egz = inp::readDoubleInRange("Įveskite Egz svorį (0..1): ", 0.0, 1.0);

            if (std::abs((nd + egz) - 1.0) > 1e-9) {
                throw std::runtime_error("Svorių suma turi būti lygi 1.0.");
            }

            w.nd = nd;
            w.egz = egz;
            return w;

        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
            const char again = inp::readCharFromSet("Bandyti dar kartą? (t/n) [t]: ", "tn", 't');
            if (again == 'n') return Weights{cfg::kDefaultNdWeight, cfg::kDefaultEgzWeight};
        }
    }
}

std::vector<std::string> findTxtFilesInCurrentDirectory() {
    std::vector<std::string> files;

    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == ".txt") {
            files.push_back(entry.path().filename().string());
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

std::vector<std::string> findBenchmarkInputFiles() {
    std::vector<std::string> files;
    for (const auto& name : findTxtFilesInCurrentDirectory()) {
        if (name == "kietiakai.txt" || name == "vargsiukai.txt") continue;
        files.push_back(name);
    }
    return files;
}

std::string chooseTxtFileFromDirectory() {
    const std::vector<std::string> files = findTxtFilesInCurrentDirectory();
    if (files.empty()) {
        throw std::runtime_error("Dabartiniame kataloge nerasta nei vieno .txt failo.");
    }

    std::cout << "\nGalimi .txt failai dabartiniame kataloge:\n";
    std::cout << "----------------------------------------\n";
    for (std::size_t i = 0; i < files.size(); ++i) {
        std::cout << std::setw(3) << i << " - " << files[i] << "\n";
    }
    std::cout << "----------------------------------------\n";

    const int choice = inp::readIntInRange(
        "Pasirinkite failo numerį: ", 0, static_cast<int>(files.size()) - 1);

    return files[static_cast<std::size_t>(choice)];
}

SortKey chooseSortKey() {
    std::cout << "\nRikiavimas:\n"
              << "1 - pagal vardą\n"
              << "2 - pagal pavardę\n"
              << "3 - pagal galutinį vidurkį\n"
              << "4 - pagal galutinę medianą\n";

    const int sortChoice = inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);
    return static_cast<SortKey>(sortChoice);
}

SplitStrategy chooseSplitStrategy() {
    std::cout << "\nSkirstymo strategija:\n"
              << "1 - du nauji konteineriai (strong + weak)\n"
              << "2 - vienas naujas konteineris + trynimai iš bendro\n"
              << "3 - stable_partition + vienas naujas konteineris\n";

    const int choice = inp::readIntInRange("Pasirinkimas (1-3): ", 1, 3);
    return static_cast<SplitStrategy>(choice);
}

int chooseSplitStrategyOrAll() {
    std::cout << "\nSkirstymo strategija benchmark'ui:\n"
              << "1 - tik 1 strategija\n"
              << "2 - tik 2 strategija\n"
              << "3 - tik 3 strategija\n"
              << "4 - visos strategijos\n";
    return inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);
}

std::string containerSlug() {
    if (kStudentContainerName == "std::list") return "list";
    if (kStudentContainerName == "std::deque") return "deque";
    return "vector";
}

void executeSplit(const StudentContainer& sortedStudents,
                  SplitStrategy strategy,
                  StudentContainer& strong,
                  StudentContainer& weak,
                  double& splitSecs) {
    strong.clear();
    weak.clear();

    switch (strategy) {
        case SplitStrategy::Strategy1: {
            const auto splitStart = timer::now();
            splitStudentsStrategy1(sortedStudents, strong, weak);
            const auto splitEnd = timer::now();
            splitSecs = timer::seconds(splitStart, splitEnd);
            break;
        }
        case SplitStrategy::Strategy2: {
            StudentContainer working = sortedStudents;
            const auto splitStart = timer::now();
            splitStudentsStrategy2(working, weak);
            const auto splitEnd = timer::now();
            splitSecs = timer::seconds(splitStart, splitEnd);
            strong = std::move(working);
            break;
        }
        case SplitStrategy::Strategy3: {
            StudentContainer working = sortedStudents;
            const auto splitStart = timer::now();
            splitStudentsStrategy3(working, weak);
            const auto splitEnd = timer::now();
            splitSecs = timer::seconds(splitStart, splitEnd);
            strong = std::move(working);
            break;
        }
    }
}

void writeGroupToFile(const std::string& filename,
                     const StudentContainer& students) {
    std::ofstream out(filename);
    if (!out) {
        throw std::runtime_error("Nepavyko sukurti failo: " + filename);
    }
    printTable(out, students, 18);
}

void runOutputPipeline(StudentContainer students,
                       bool fromFile,
                       const std::string& sourceLabel,
                       double readSecs) {
    if (students.empty()) {
        std::cout << "\nNėra studentų duomenų. Grįžtama į pagrindinį meniu.\n";
        return;
    }

    const SortKey key = chooseSortKey();
    const SplitStrategy strategy = chooseSplitStrategy();

    const auto totalStart = timer::now();

    const auto sortStart = timer::now();
    sortStudents(students, key);
    const auto sortEnd = timer::now();
    const double sortSecs = timer::seconds(sortStart, sortEnd);

    StudentContainer strong;
    StudentContainer weak;
    double splitSecs = 0.0;
    executeSplit(students, strategy, strong, weak, splitSecs);

    const auto writeWeakStart = timer::now();
    writeGroupToFile("vargsiukai.txt", weak);
    const auto writeWeakEnd = timer::now();

    const auto writeStrongStart = timer::now();
    writeGroupToFile("kietiakai.txt", strong);
    const auto writeStrongEnd = timer::now();

    const auto totalEnd = timer::now();

    std::cout << "\nRezultatai išsaugoti:\n"
              << "  - kietiakai.txt (>= 5.0)\n"
              << "  - vargsiukai.txt (< 5.0)\n";

    std::cout << "\nLaikų suvestinė:\n";
    std::cout << "------------------------------------------------------------\n";
    std::cout << "Konteineris: " << kStudentContainerName << "\n";
    std::cout << "Strategija: " << splitStrategyLabel(strategy) << "\n";
    std::cout << "Rikiavimo raktas: " << sortKeyLabel(key) << "\n";
    if (fromFile) {
        std::cout << "Šaltinis failas: " << sourceLabel << "\n";
        timer::print("Failo nuskaitymo laikas", readSecs);
    } else {
        std::cout << "Šaltinis: interaktyvus / generuotas v0.1 meniu\n";
    }
    std::cout << "Studentų kiekis: " << students.size() << "\n";
    std::cout << "Kietiakų kiekis: " << strong.size() << "\n";
    std::cout << "Vargšiukų kiekis: " << weak.size() << "\n";
    timer::print("Rikiavimo laikas", sortSecs);
    timer::print("Padalijimo į dvi grupes laikas", splitSecs);
    timer::print("Vargšiukų įrašymo į failą laikas", timer::seconds(writeWeakStart, writeWeakEnd));
    timer::print("Kietiakų įrašymo į failą laikas", timer::seconds(writeStrongStart, writeStrongEnd));
    timer::print("Visos fazės laikas", timer::seconds(totalStart, totalEnd));
    std::cout << "------------------------------------------------------------\n";
}

void runBenchmarkMode(const Weights& weights) {
    const std::vector<std::string> available = findBenchmarkInputFiles();
    if (available.empty()) {
        throw std::runtime_error("Benchmark režimui nerasta tinkamų .txt failų.");
    }

    std::cout << "\nBenchmark režimas matuoja tik:\n"
              << "  - duomenų nuskaitymą\n"
              << "  - bendro konteinerio rikiavimą\n"
              << "  - studentų skirstymą į dvi grupes\n";

    std::cout << "\nFailų pasirinkimas benchmark'ui:\n"
              << "1 - vienas failas\n"
              << "2 - visi rasti .txt failai (be rezultatų failų)\n";

    const int scope = inp::readIntInRange("Pasirinkimas (1-2): ", 1, 2);
    std::vector<std::string> files;
    if (scope == 1) {
        files.push_back(chooseTxtFileFromDirectory());
    } else {
        files = available;
    }

    const SortKey key = chooseSortKey();
    const int strategyChoice = chooseSplitStrategyOrAll();
    const int repeats = inp::readIntInRange("Kiek kartų kartoti kiekvieną testą? (1-20): ", 1, 20);

    const std::string csvName = "benchmark_" + containerSlug() + ".csv";
    const char resetCsv = inp::readCharFromSet(
        "Iš naujo sukurti benchmark CSV failą? (t/n) [t]: ", "tn", 't');
    if (resetCsv == 't') {
        std::ofstream(csvName, std::ios::trunc).close();
    }

    const std::array<SplitStrategy, 3> allStrategies = {
        SplitStrategy::Strategy1,
        SplitStrategy::Strategy2,
        SplitStrategy::Strategy3
    };

    for (const auto& file : files) {
        if (strategyChoice == 4) {
            for (SplitStrategy strategy : allStrategies) {
                const BenchmarkRow row = benchmarkAverage(file, weights, key, strategy, repeats);
                printBenchmarkRow(row);
                appendBenchmarkCsv(csvName, row, repeats);
            }
        } else {
            const SplitStrategy strategy = static_cast<SplitStrategy>(strategyChoice);
            const BenchmarkRow row = benchmarkAverage(file, weights, key, strategy, repeats);
            printBenchmarkRow(row);
            appendBenchmarkCsv(csvName, row, repeats);
        }
    }

    std::cout << "\nBenchmark rezultatai išsaugoti: " << csvName << "\n";
}

void printMainMenu() {
    std::cout << "\nDarbo rėžimas:\n"
              << "1 - Nuskaityti studentus iš failo ir sugeneruoti rezultatus\n"
              << "2 - Įvesti / generuoti (v0.1 meniu)\n"
              << "3 - Generuoti studentų failus\n"
              << "4 - Benchmark / tyrimas (tik read + sort + split)\n"
              << "5 - Baigti darbą\n";
}

} // namespace

int main() {
    utf8::initUtf8Locale();

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        std::cout << "v1.0 - Studentų galutinio balo skaičiuoklė\n";
        std::cout << "Aktyvus konteineris: " << kStudentContainerName << "\n";

        const Weights weights = chooseWeights();

        while (true) {
            printMainMenu();
            const int mode = inp::readIntInRange("Pasirinkimas (1-5): ", 1, 5);

            if (mode == 5) {
                std::cout << "\nPrograma baigia darbą.\n";
                break;
            }

            if (mode == 3) {
                const int ndCount = inp::readIntInRange(
                    "Kiek ND turi turėti kiekvienas studentas? ", 1, 1000);
                generateTestFiles(ndCount);
                std::cout << "\nStudentų failai sugeneruoti. Grįžtama į pagrindinį meniu.\n";
                continue;
            }

            if (mode == 4) {
                runBenchmarkMode(weights);
                continue;
            }

            if (mode == 1) {
                while (true) {
                    try {
                        const std::string selectedFilename = chooseTxtFileFromDirectory();
                        std::cout << "\nPasirinktas failas: " << selectedFilename << "\n";

                        StudentContainer students;
                        const auto readStart = timer::now();
                        readStudentsFromFile(selectedFilename, students, weights);
                        const auto readEnd = timer::now();

                        runOutputPipeline(students, true, selectedFilename, timer::seconds(readStart, readEnd));
                        break;

                    } catch (const std::exception& e) {
                        std::cout << "\nKlaida: " << e.what() << "\n";
                        const char again = inp::readCharFromSet(
                            "Bandyti pasirinkti failą dar kartą? (t/n) [t]: ", "tn", 't');
                        if (again == 'n') {
                            break;
                        }
                    }
                }
            } else if (mode == 2) {
                StudentContainer students;
                menuV01Style(students, weights);
                runOutputPipeline(students, false, "", 0.0);
            }

            const char again = inp::readCharFromSet(
                "\nGrįžti į pagrindinį meniu? (t/n) [t]: ", "tn", 't');
            if (again == 'n') {
                std::cout << "\nPrograma baigia darbą.\n";
                break;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
