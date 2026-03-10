#include "constants.h"
#include "fileio.h"
#include "input.h"
#include "menu.h"
#include "sorting.h"
#include "table.h"
#include "types.h"
#include "utf8.h"
#include "fileGenerator.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static Weights chooseWeights() {
    Weights w{cfg::kDefaultNdWeight, cfg::kDefaultEgzWeight};

    std::cout << "\nSvorio pasirinkimas:\n"
    << "Numatyta: ND=0.4, Egz=0.6\n";

    const char ans = inp::readCharFromSet(
        "Naudoti numatytus svorius? (t/n) [t]: ", "tn", 't');

    if (ans == 't') return w;

    while (true) {
        try {
            const double nd = inp::readDoubleInRange("Iveskite ND svori (0..1): ", 0.0, 1.0);
            const double egz = inp::readDoubleInRange("Iveskite Egz svori (0..1): ", 0.0, 1.0);

            if (std::abs((nd + egz) - 1.0) > 1e-9) {
                throw std::runtime_error("Svoriu suma turi buti lygi 1.0.");
            }

            w.nd = nd;
            w.egz = egz;
            return w;

        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
            const char again = inp::readCharFromSet("Bandyti dar karta? (t/n) [t]: ", "tn", 't');
            if (again == 'n') return Weights{cfg::kDefaultNdWeight, cfg::kDefaultEgzWeight};
        }
    }
}

static std::vector<std::string> findTxtFilesInCurrentDirectory() {
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

static std::string chooseTxtFileFromDirectory() {
    while (true) {
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
            "Pasirinkite failo numeri: ", 0, static_cast<int>(files.size()) - 1);

        return files[static_cast<std::size_t>(choice)];
    }
}

static void printMainMenu() {
    std::cout << "\nDarbo rezimas:\n"
    << "1 - Nuskaityti studentus is failo\n"
    << "2 - Ivesti / generuoti (v0.1 meniu)\n"
    << "3 - Generuoti testinius failus\n"
    << "4 - Baigti darba\n";
}

int main() {
    utf8::initUtf8Locale();

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        std::cout << "v0.4 - Studentu galutinio balo skaiciuokle\n";

        const Weights weights = chooseWeights();

        while (true) {
            printMainMenu();
            const int mode = inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);

            if (mode == 4) {
                std::cout << "\nPrograma baigia darba.\n";
                break;
            }

            if (mode == 3) {
                const int ndCount = inp::readIntInRange(
                    "Kiek ND turi tureti kiekvienas studentas? ", 1, 1000);

                generateTestFiles(ndCount);

                std::cout << "\nTestiniai failai sugeneruoti. Griztama i pagrindini meniu.\n";
                continue;
            }

            std::vector<StudentRec> students;
            students.reserve(cfg::kMaxStudentsGenerate);

            std::size_t maxVCols = 0;
            std::size_t maxPCols = 0;

            if (mode == 1) {
                while (true) {
                    try {
                        const std::string filename = chooseTxtFileFromDirectory();

                        std::cout << "\nPasirinktas failas: " << filename << "\n";
                        readStudentsFromFileVec(filename, students, maxVCols, maxPCols, weights);
                        break;

                    } catch (const std::exception& e) {
                        std::cout << "\nKlaida: " << e.what() << "\n";
                        const char again = inp::readCharFromSet(
                            "Bandyti pasirinkti faila dar karta? (t/n) [t]: ", "tn", 't');
                        if (again == 'n') {
                            students.clear();
                            break;
                        }
                    }
                }
            } else {
                menuV01Style(students, maxVCols, maxPCols, weights);
            }

            if (students.empty()) {
                std::cout << "\nNera studentu duomenu. Griztama i pagrindini meniu.\n";
                continue;
            }

            std::cout << "\nRikiavimas:\n"
            << "1 - pagal varda\n"
            << "2 - pagal pavarde\n"
            << "3 - pagal Galutini vidurki\n"
            << "4 - pagal Galutine mediana\n";

            const int sortChoice = inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);
            const SortKey key = static_cast<SortKey>(sortChoice);

            sortStudents(students, key);

            std::vector<StudentRec> strong;
            std::vector<StudentRec> weak;

            std::size_t maxVStrong = 0, maxPStrong = 0;
            std::size_t maxVWeak = 0, maxPWeak = 0;

            for (const auto& s : students) {
                if (s.galVid >= 5.0) {
                    strong.push_back(s);
                    maxVStrong = std::max(maxVStrong, utf8::displayWidth(s.vardas));
                    maxPStrong = std::max(maxPStrong, utf8::displayWidth(s.pavarde));
                } else {
                    weak.push_back(s);
                    maxVWeak = std::max(maxVWeak, utf8::displayWidth(s.vardas));
                    maxPWeak = std::max(maxPWeak, utf8::displayWidth(s.pavarde));
                }
            }

            sortStudents(strong, key);
            sortStudents(weak, key);

            std::ofstream outStrong("kietiakai.txt");
            std::ofstream outWeak("vargsiukai.txt");

            if (!outStrong || !outWeak) {
                throw std::runtime_error("Nepavyko sukurti rezultatu failu.");
            }

            if (!strong.empty()) {
                printTable(outStrong, strong, maxVStrong, maxPStrong, 18);
            }

            if (!weak.empty()) {
                printTable(outWeak, weak, maxVWeak, maxPWeak, 18);
            }

            std::cout << "\nRezultatai issaugoti:\n"
            << "  - kietiakai.txt (>= 5.0)\n"
            << "  - vargsiukai.txt (< 5.0)\n";

            const char again = inp::readCharFromSet(
                "\nGrizti i pagrindini meniu? (t/n) [t]: ", "tn", 't');

            if (again == 'n') {
                std::cout << "\nPrograma baigia darba.\n";
                break;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
