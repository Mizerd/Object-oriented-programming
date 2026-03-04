#include "menu.h"

#include "constants.h"
#include "grades.h"
#include "input.h"
#include "utf8.h"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static int rndGrade(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 10);
    return dist(rng);
}

static void readHomeworkInteractive(std::vector<int>& nd) {
    nd.clear();
    nd.reserve(cfg::kMaxNdPerStudent);

    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n"
              << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        if (static_cast<int>(nd.size()) >= cfg::kMaxNdPerStudent) {
            std::cout << "Pasiektas maksimalus ND kiekis (" << cfg::kMaxNdPerStudent << ").\n";
            break;
        }

        const int g = inp::readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        nd.push_back(g);
    }

    if (nd.empty()) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

void menuV01Style(std::vector<StudentRec>& students,
                  std::size_t& maxVCols,
                  std::size_t& maxPCols,
                  const Weights& weights) {
    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    while (true) {
        std::cout
            << "\nMeniu:\n"
            << "1 - Įvesti ranka (vardas pavardė, tada ND ir egz)\n"
            << "2 - Generuoti pažymius (vardą/pavardę įvedate)\n"
            << "3 - Generuoti vardus, pavardes ir pažymius\n"
            << "4 - Baigti darbą\n";

        const int choice = inp::readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        if (choice == 4) break;

        if (choice == 1) {
            std::cout << "\n--- Rankinis įvedimas ---\n";
            std::vector<int> nd;

            while (true) {
                std::string v, p;
                if (!inp::readNameSurname(v, p)) break;

                readHomeworkInteractive(nd);
                const int egz = inp::readIntInRange("Egzamino pažymys (1..10): ", 1, 10);

                StudentRec s;
                s.vardas = std::move(v);
                s.pavarde = std::move(p);
                s.galVid = grade::calcFinal(grade::average(nd), egz, weights);
                s.galMed = grade::calcFinal(grade::median(nd), egz, weights);

                maxVCols = std::max(maxVCols, utf8::displayWidth(s.vardas));
                maxPCols = std::max(maxPCols, utf8::displayWidth(s.pavarde));
                students.push_back(std::move(s));
            }

        } else if (choice == 2) {
            std::cout << "\n--- Vardą/pavardę įvedate, pažymius generuoja ---\n";
            const int k = inp::readIntInRange(
                "Kiek ND generuoti kiekvienam studentui? (1..1000): ", 1, cfg::kMaxNdPerStudent);

            std::vector<int> nd;
            nd.reserve(static_cast<std::size_t>(k));

            while (true) {
                std::string v, p;
                if (!inp::readNameSurname(v, p)) break;

                nd.clear();
                for (int i = 0; i < k; ++i) nd.push_back(rndGrade(rng));
                const int egz = rndGrade(rng);

                StudentRec s;
                s.vardas = std::move(v);
                s.pavarde = std::move(p);
                s.galVid = grade::calcFinal(grade::average(nd), egz, weights);
                s.galMed = grade::calcFinal(grade::median(nd), egz, weights);

                maxVCols = std::max(maxVCols, utf8::displayWidth(s.vardas));
                maxPCols = std::max(maxPCols, utf8::displayWidth(s.pavarde));
                students.push_back(std::move(s));
            }

        } else if (choice == 3) {
            std::cout << "\n--- Generuojami vardai, pavardės ir pažymiai ---\n";
            const int genM = inp::readIntInRange(
                "Kiek studentų sugeneruoti? (1..10000): ", 1, cfg::kMaxStudentsGenerate);
            const int k = inp::readIntInRange(
                "Kiek ND generuoti kiekvienam studentui? (1..1000): ", 1, cfg::kMaxNdPerStudent);

            students.reserve(students.size() + static_cast<std::size_t>(genM));

            static const std::vector<std::string> maleNames = {
                "Arvydas","Rimas","Mantas","Lukas","Tomas","Paulius","Jonas","Darius"};
            static const std::vector<std::string> femaleNames = {
                "Ieva","Gabija","Eglė","Monika","Austėja","Greta","Justė","Ugnė"};
            static const std::vector<std::string> maleSurnames = {
                "Sabonis","Kurtinaitis","Petrauskas","Kazlauskas","Vaitkus",
                "Stankevičius","Brazdeikis","Jankauskas"};
            static const std::vector<std::string> femaleSurnames = {
                "Sabonytė","Kurtinaitytė","Petrauskaitė","Kazlauskaitė","Vaitkutė",
                "Stankevičiūtė","Brazdeikytė","Jankauskaitė"};

            std::uniform_int_distribution<std::size_t> dMaleName(0, maleNames.size() - 1);
            std::uniform_int_distribution<std::size_t> dFemaleName(0, femaleNames.size() - 1);
            std::uniform_int_distribution<std::size_t> dMaleSur(0, maleSurnames.size() - 1);
            std::uniform_int_distribution<std::size_t> dFemaleSur(0, femaleSurnames.size() - 1);
            std::bernoulli_distribution pickFemale(0.5);

            std::vector<int> nd;
            nd.reserve(static_cast<std::size_t>(k));

            for (int i = 0; i < genM; ++i) {
                StudentRec s;
                const bool isFemale = pickFemale(rng);
                if (isFemale) {
                    s.vardas = femaleNames[dFemaleName(rng)];
                    s.pavarde = femaleSurnames[dFemaleSur(rng)];
                } else {
                    s.vardas = maleNames[dMaleName(rng)];
                    s.pavarde = maleSurnames[dMaleSur(rng)];
                }

                nd.clear();
                for (int j = 0; j < k; ++j) nd.push_back(rndGrade(rng));
                const int egz = rndGrade(rng);

                s.galVid = grade::calcFinal(grade::average(nd), egz, weights);
                s.galMed = grade::calcFinal(grade::median(nd), egz, weights);

                maxVCols = std::max(maxVCols, utf8::displayWidth(s.vardas));
                maxPCols = std::max(maxPCols, utf8::displayWidth(s.pavarde));
                students.push_back(std::move(s));
            }
        }
    }
}
