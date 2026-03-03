#include "constants.h"
#include "fileio.h"
#include "input.h"
#include "menu.h"
#include "sorting.h"
#include "table.h"
#include "types.h"
#include "utf8.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

static Weights chooseWeights() {
    Weights w{cfg::kDefaultNdWeight, cfg::kDefaultEgzWeight};

    std::cout << "\nSvorio pasirinkimas (realių skaičių įvesties tikrinimas):\n"
              << "Numatyta: ND=0.4, Egz=0.6\n";

    const char ans = inp::readCharFromSet(
        "Naudoti numatytus svorius? (t/n) [t]: ", "tn", 't');

    if (ans == 't') return w;

    while (true) {
        try {
            const double nd = inp::readDoubleInRange("Įveskite ND svorį (0..1): ", 0.0, 1.0);
            const double egz = inp::readDoubleInRange("Įveskite Egz svorį (0..1): ", 0.0, 1.0);

            if (std::abs((nd + egz) - 1.0) > 1e-9) {
                throw std::runtime_error("Svorų suma turi būti lygi 1.0 (pvz. 0.4 ir 0.6).");
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

int main() {
    utf8::initUtf8Locale();

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<StudentRec> students;
    students.reserve(cfg::kMaxStudentsGenerate);

    std::size_t maxVCols = 0;
    std::size_t maxPCols = 0;

    try {
        std::cout
            << "v0.3 - Studentų galutinio balo skaičiuoklė (refactoring + išimtys)\n";

        const Weights weights = chooseWeights();

        std::cout
            << "\nDarbo režimas:\n"
            << "1 - Nuskaityti studentus iš failo\n"
            << "2 - Įvesti / generuoti (v0.1 meniu)\n";

        const int mode = inp::readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (mode == 1) {
            while (true) {
                try {
                    std::string filename = inp::readLineTrimmedPrompted(
                        "\nĮveskite failo pavadinimą (pvz. kursiokai.txt, studentai10000.txt):\n> ");
                    if (filename.empty()) {
                        filename = "kursiokai.txt";
                        std::cout << "Naudosime numatytą failą: " << filename << "\n";
                    }

                    // Failo egzistavimas / atidarymas tikrinamas per išimtis (readStudentsFromFileVec meta std::runtime_error).
                    readStudentsFromFileVec(filename, students, maxVCols, maxPCols, weights);
                    break;

                } catch (const std::exception& e) {
                    std::cout << "\nKlaida: " << e.what() << "\n";
                    const char again = inp::readCharFromSet("Bandyti kitą failą? (t/n) [t]: ", "tn", 't');
                    if (again == 'n') return 0;
                }
            }
        } else {
            menuV01Style(students, maxVCols, maxPCols, weights);
        }

        if (students.empty()) {
            std::cout << "\nNėra studentų duomenų.\n";
            return 0;
        }

        std::cout
            << "\nRikiavimas:\n"
            << "1 - pagal vardą\n"
            << "2 - pagal pavardę\n"
            << "3 - pagal Galutinį vidurkį\n"
            << "4 - pagal Galutinę medianą\n";

        const int sortChoice = inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        sortStudents(students, static_cast<SortKey>(sortChoice));

        std::cout
            << "\nIšvedimas:\n"
            << "1 - į ekraną\n"
            << "2 - į failą\n";

        const int outChoice = inp::readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (outChoice == 1) {
            // Terminal: mažesnis tarpas tarp pavardės ir pažymių.
            printTable(std::cout, students, maxVCols, maxPCols, 4);

        } else {
            while (true) {
                try {
                    std::string outName = inp::readLineTrimmedPrompted(
                        "\nĮveskite rezultatų failo pavadinimą (pvz. rezultatai.txt):\n> ");
                    if (outName.empty()) outName = "rezultatai.txt";

                    std::ofstream out(outName);
                    if (!out) throw std::runtime_error("Nepavyko sukurti rezultatų failo: " + outName);

                    // Failas: didesnis tarpas tarp pavardės ir pažymių.
                    printTable(out, students, maxVCols, maxPCols, 12);
                    std::cout << "\nRezultatai įrašyti į failą: " << outName << "\n";
                    break;

                } catch (const std::exception& e) {
                    std::cout << "\nKlaida: " << e.what() << "\n";
                    const char again = inp::readCharFromSet("Bandyti kitą failo pavadinimą? (t/n) [t]: ", "tn", 't');
                    if (again == 'n') break;
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
