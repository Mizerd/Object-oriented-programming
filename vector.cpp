#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

struct Student {
    std::string vardas;
    std::string pavarde;
    std::vector<int> nd;
    int egz = 0;
    double galVid = 0.0;
    double galMed = 0.0;
};

static void clearBadInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::cout << prompt << std::flush;
        int x;
        if (std::cin >> x) {
            if (x >= minVal && x <= maxVal) return x;
            std::cout << "Klaida: reikšmė turi būti [" << minVal << ".." << maxVal << "].\n";
        } else {
            std::cout << "Klaida: įveskite skaičių.\n";
            clearBadInput();
        }
    }
}

double averageVec(const std::vector<int>& v) {
    if (v.empty()) return 0.0;
    long long sum = 0;
    for (int x : v) sum += x;
    return static_cast<double>(sum) / static_cast<double>(v.size());
}

double medianVec(std::vector<int> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    const size_t n = v.size();
    if (n % 2 == 1) return static_cast<double>(v[n / 2]);
    return (static_cast<double>(v[n / 2 - 1]) + static_cast<double>(v[n / 2])) / 2.0;
}

double calcFinal(double ndValue, int egz) {
    return kNdWeight * ndValue + kEgzWeight * static_cast<double>(egz);
}

void computeBothFinals(Student& s) {
    const double ndAvg = averageVec(s.nd);
    const double ndMed = medianVec(s.nd);
    s.galVid = calcFinal(ndAvg, s.egz);
    s.galMed = calcFinal(ndMed, s.egz);
}

int rndGrade(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 10);
    return dist(rng);
}

void printMenu() {
    std::cout
        << "\nMeniu:\n"
        << "1 - Įvesti ranka (vardas pavardė, tada ND ir egz)\n"
        << "2 - Generuoti pažymius (vardą/pavardę įvedate)\n"
        << "3 - Generuoti vardus, pavardes ir pažymius\n"
        << "4 - Baigti darbą\n"
        << std::flush;
}

// Output: Vardas then Pavardė
void printTable(const std::vector<Student>& students) {
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "Vardas"
              << std::left << std::setw(15) << "Pavardė"
              << std::left << std::setw(18) << "Galutinis (Vid.)"
              << std::left << std::setw(18) << "Galutinis (Med.)"
              << "\n";
    std::cout << std::string(66, '-') << "\n";

    for (const auto& s : students) {
        std::cout << std::left << std::setw(15) << s.vardas
                  << std::left << std::setw(15) << s.pavarde
                  << std::fixed << std::setprecision(2)
                  << std::left << std::setw(18) << s.galVid
                  << std::left << std::setw(18) << s.galMed
                  << "\n";
    }
}

// Reads "vardas pavarde" in one go. Type 0 as first token to stop.
bool readNameSurname(Student& s) {
    std::cout << "\nĮveskite: vardas pavardė (arba 0, kad baigti): " << std::flush;
    std::string v;
    if (!(std::cin >> v)) return false;
    if (v == "0") return false;

    std::string p;
    if (!(std::cin >> p)) return false;

    s.vardas = v;
    s.pavarde = p;
    return true;
}

void readHomeworkInteractive(Student& s) {
    s.nd.clear();
    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n";
    std::cout << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        int g = readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        s.nd.push_back(g);
        if (s.nd.size() >= 50) { // optional limit to match array MAX_ND
            std::cout << "Pasiektas ND limitas (50).\n";
            break;
        }
    }

    if (s.nd.empty()) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    std::vector<Student> students;

    while (true) {
        printMenu();
        int choice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        if (choice == 4) break;

        if (choice == 1) {
            std::cout << "\n--- Rankinis įvedimas ---\n";
            while (true) {
                Student s;
                if (!readNameSurname(s)) break;

                std::cout << "Studentas: " << s.vardas << " " << s.pavarde << "\n";
                readHomeworkInteractive(s);
                s.egz = readIntInRange("Egzamino pažymys (1..10): ", 1, 10);

                computeBothFinals(s);
                students.push_back(std::move(s));
            }
        } else if (choice == 2) {
            std::cout << "\n--- Vardą/pavardę įvedate, pažymius generuoja ---\n";
            int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);

            while (true) {
                Student s;
                if (!readNameSurname(s)) break;

                std::cout << "Studentas: " << s.vardas << " " << s.pavarde
                          << " (generuojami pažymiai...)\n";

                s.nd.clear();
                s.nd.reserve(static_cast<size_t>(k));
                for (int i = 0; i < k; ++i) s.nd.push_back(rndGrade(rng));
                s.egz = rndGrade(rng);

                computeBothFinals(s);
                students.push_back(std::move(s));
            }
        } else if (choice == 3) {
            std::cout << "\n--- Generuojami vardai, pavardės ir pažymiai ---\n";
            int genM = readIntInRange("Kiek studentų sugeneruoti? (1..200): ", 1, 200);
            int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);

            // Atskiri duomenų rinkiniai vyrų ir moterų vardams/pavardėms
            static const std::vector<std::string> maleNames = {
                "Arvydas","Rimas","Mantas","Lukas","Tomas","Paulius","Jonas","Darius"
            };
            static const std::vector<std::string> femaleNames = {
                "Ieva","Gabija","Egle","Monika","Austeja","Greta","Juste","Ugne"
            };

            // Lietuviškos pavardės dažnai turi skirtingas formas (pvz., -as/-is vs -aitė/-ytė/-ienė).
            // Čia pateikiami atskiri sąrašai sugeneravimui.
            static const std::vector<std::string> maleSurnames = {
                "Sabonis","Kurtinaitis","Petrauskas","Kazlauskas","Vaitkus","Stankevicius","Brazdeikis","Jankauskas"
            };
            static const std::vector<std::string> femaleSurnames = {
                "Sabonyte","Kurtinaityte","Petrauskaite","Kazlauskaite","Vaitkute","Stankeviciute","Brazdeikyte","Jankauskaite"
            };

            std::uniform_int_distribution<size_t> dMaleName(0, maleNames.size() - 1);
            std::uniform_int_distribution<size_t> dFemaleName(0, femaleNames.size() - 1);
            std::uniform_int_distribution<size_t> dMaleSur(0, maleSurnames.size() - 1);
            std::uniform_int_distribution<size_t> dFemaleSur(0, femaleSurnames.size() - 1);
            std::bernoulli_distribution pickFemale(0.5);

            for (int i = 0; i < genM; ++i) {
                Student s;
                const bool isFemale = pickFemale(rng);
                if (isFemale) {
                    s.vardas = femaleNames[dFemaleName(rng)];
                    s.pavarde = femaleSurnames[dFemaleSur(rng)];
                } else {
                    s.vardas = maleNames[dMaleName(rng)];
                    s.pavarde = maleSurnames[dMaleSur(rng)];
                }

                s.nd.reserve(static_cast<size_t>(k));
                for (int j = 0; j < k; ++j) s.nd.push_back(rndGrade(rng));
                s.egz = rndGrade(rng);

                computeBothFinals(s);
                students.push_back(std::move(s));
            }
        }
    }

    if (students.empty()) {
        std::cout << "\nNėra įvestų studentų.\n";
        return 0;
    }

    for (auto& s : students) computeBothFinals(s);
    printTable(students);
    return 0;
}
