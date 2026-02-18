#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>

constexpr int MAX_STUDENTS = 1000;
constexpr int MAX_ND = 50;

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

struct StudentA {
    std::string vardas;
    std::string pavarde;

    int nd[MAX_ND];
    int ndCount = 0;

    int egz = 0;

    double galVid = 0.0; // Galutinis (Vid.)
    double galMed = 0.0; // Galutinis (Med.)
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

double averageArr(const int* a, int n) {
    if (n <= 0) return 0.0;
    long long sum = 0;
    for (int i = 0; i < n; ++i) sum += a[i];
    return static_cast<double>(sum) / static_cast<double>(n);
}

double medianArrCopy(const int* a, int n) {
    if (n <= 0) return 0.0;
    int tmp[MAX_ND];
    for (int i = 0; i < n; ++i) tmp[i] = a[i];
    std::sort(tmp, tmp + n);
    if (n % 2 == 1) return static_cast<double>(tmp[n / 2]);
    return (static_cast<double>(tmp[n / 2 - 1]) + static_cast<double>(tmp[n / 2])) / 2.0;
}

double calcFinal(double ndValue, int egz) {
    return kNdWeight * ndValue + kEgzWeight * static_cast<double>(egz);
}

void computeBothFinals(StudentA& s) {
    const double ndAvg = averageArr(s.nd, s.ndCount);
    const double ndMed = medianArrCopy(s.nd, s.ndCount);
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
void printTable(const StudentA* students, int m) {
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "Vardas"
              << std::left << std::setw(15) << "Pavardė"
              << std::left << std::setw(18) << "Galutinis (Vid.)"
              << std::left << std::setw(18) << "Galutinis (Med.)"
              << "\n";
    std::cout << std::string(66, '-') << "\n";

    for (int i = 0; i < m; ++i) {
        std::cout << std::left << std::setw(15) << students[i].vardas
                  << std::left << std::setw(15) << students[i].pavarde
                  << std::fixed << std::setprecision(2)
                  << std::left << std::setw(18) << students[i].galVid
                  << std::left << std::setw(18) << students[i].galMed
                  << "\n";
    }
}

// Reads "vardas pavarde" in one go. Type 0 as first token to stop.
bool readNameSurname(StudentA& s) {
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

void readHomeworkInteractive(StudentA& s) {
    s.ndCount = 0;

    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n";
    std::cout << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        if (s.ndCount >= MAX_ND) {
            std::cout << "Pasiektas MAX_ND limitas (" << MAX_ND << ").\n";
            break;
        }
        int g = readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        s.nd[s.ndCount++] = g;
    }

    if (s.ndCount == 0) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    StudentA students[MAX_STUDENTS];
    int m = 0;

    while (true) {
        printMenu();
        int choice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        if (choice == 4) break;

        if (choice == 1) {
            std::cout << "\n--- Rankinis įvedimas ---\n";
            while (m < MAX_STUDENTS) {
                StudentA s;
                if (!readNameSurname(s)) break;

                std::cout << "Studentas: " << s.vardas << " " << s.pavarde << "\n";
                readHomeworkInteractive(s);
                s.egz = readIntInRange("Egzamino pažymys (1..10): ", 1, 10);

                computeBothFinals(s);
                students[m++] = s;
            }
            if (m >= MAX_STUDENTS) std::cout << "Pasiektas MAX_STUDENTS limitas.\n";
        } else if (choice == 2) {
            std::cout << "\n--- Vardą/pavardę įvedate, pažymius generuoja ---\n";
            int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, MAX_ND);

            while (m < MAX_STUDENTS) {
                StudentA s;
                if (!readNameSurname(s)) break;

                std::cout << "Studentas: " << s.vardas << " " << s.pavarde
                          << " (generuojami pažymiai...)\n";

                s.ndCount = k;
                for (int i = 0; i < k; ++i) s.nd[i] = rndGrade(rng);
                s.egz = rndGrade(rng);

                computeBothFinals(s);
                students[m++] = s;
            }
            if (m >= MAX_STUDENTS) std::cout << "Pasiektas MAX_STUDENTS limitas.\n";
        } else if (choice == 3) {
            std::cout << "\n--- Generuojami vardai, pavardės ir pažymiai ---\n";
            int genM = readIntInRange("Kiek studentų sugeneruoti? (1..200): ", 1, 200);
            int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, MAX_ND);

            // Atskiri duomenų rinkiniai vyrų ir moterų vardams/pavardėms
            static const std::string maleNames[] = {
                "Arvydas","Rimas","Mantas","Lukas","Tomas","Paulius","Jonas","Darius"
            };
            static const std::string femaleNames[] = {
                "Ieva","Gabija","Egle","Monika","Austeja","Greta","Juste","Ugne"
            };

            // Lietuviškos pavardės dažnai turi skirtingas formas (pvz., -as/-is vs -aitė/-ytė/-ienė).
            // Čia pateikiami atskiri sąrašai sugeneravimui.
            static const std::string maleSurnames[] = {
                "Sabonis","Kurtinaitis","Petrauskas","Kazlauskas","Vaitkus","Stankevicius","Brazdeikis","Jankauskas"
            };
            static const std::string femaleSurnames[] = {
                "Sabonyte","Kurtinaityte","Petrauskaite","Kazlauskaite","Vaitkute","Stankeviciute","Brazdeikyte","Jankauskaite"
            };

            const int maleNamesN = static_cast<int>(sizeof(maleNames) / sizeof(maleNames[0]));
            const int femaleNamesN = static_cast<int>(sizeof(femaleNames) / sizeof(femaleNames[0]));
            const int maleSurN = static_cast<int>(sizeof(maleSurnames) / sizeof(maleSurnames[0]));
            const int femaleSurN = static_cast<int>(sizeof(femaleSurnames) / sizeof(femaleSurnames[0]));

            std::uniform_int_distribution<int> dMaleName(0, maleNamesN - 1);
            std::uniform_int_distribution<int> dFemaleName(0, femaleNamesN - 1);
            std::uniform_int_distribution<int> dMaleSur(0, maleSurN - 1);
            std::uniform_int_distribution<int> dFemaleSur(0, femaleSurN - 1);
            std::bernoulli_distribution pickFemale(0.5);

            for (int i = 0; i < genM && m < MAX_STUDENTS; ++i) {
                StudentA s;
                const bool isFemale = pickFemale(rng);
                if (isFemale) {
                    s.vardas = femaleNames[dFemaleName(rng)];
                    s.pavarde = femaleSurnames[dFemaleSur(rng)];
                } else {
                    s.vardas = maleNames[dMaleName(rng)];
                    s.pavarde = maleSurnames[dMaleSur(rng)];
                }

                s.ndCount = k;
                for (int j = 0; j < k; ++j) s.nd[j] = rndGrade(rng);
                s.egz = rndGrade(rng);

                computeBothFinals(s);
                students[m++] = s;
            }
            if (m >= MAX_STUDENTS) std::cout << "Pasiektas MAX_STUDENTS limitas.\n";
        }
    }

    if (m == 0) {
        std::cout << "\nNėra įvestų studentų.\n";
        return 0;
    }

    for (int i = 0; i < m; ++i) computeBothFinals(students[i]);
    printTable(students, m);
    return 0;
}
