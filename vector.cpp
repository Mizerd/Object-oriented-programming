#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

// v0.1.2 - saugome tik galutinius rezultatus (kaip v0.2)
struct StudentRec {
    std::string vardas;
    std::string pavarde;
    double galVid = 0.0;
    double galMed = 0.0;
};

static std::string trimWs(std::string s) {
    auto isTrim = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    size_t start = 0;
    while (start < s.size() && isTrim(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && isTrim(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

static std::string readLinePrompted(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) throw std::runtime_error("Input stream closed (EOF).");
    return line;
}

int readIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        const std::string line = trimWs(readLinePrompted(prompt));
        if (line.empty()) {
            std::cout << "Klaida: įveskite skaičių.\n";
            continue;
        }

        std::istringstream iss(line);
        int x;
        char extra;
        if (!(iss >> x) || (iss >> extra)) {
            std::cout << "Klaida: neteisingas formatas (rašykite tik skaičių).\n";
            continue;
        }

        if (x < minVal || x > maxVal) {
            std::cout << "Klaida: reikšmė turi būti [" << minVal << ".." << maxVal << "].\n";
            continue;
        }
        return x;
    }
}

static bool readNameSurname(std::string& v, std::string& p) {
    while (true) {
        const std::string line = trimWs(readLinePrompted(
            "\nĮveskite: vardas pavardė (arba 0, kad baigti): "));
        if (line == "0") return false;
        if (line.empty()) continue;

        std::istringstream iss(line);
        if (!(iss >> v)) continue;
        if (v == "0") return false;
        if (!(iss >> p)) {
            std::cout << "Klaida: įveskite ir pavardę (formatas: vardas pavardė).\n";
            continue;
        }
        return true;
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

static StudentRec makeRec(std::string v, std::string p, const std::vector<int>& nd, int egz) {
    StudentRec s;
    s.vardas = std::move(v);
    s.pavarde = std::move(p);
    s.galVid = calcFinal(averageVec(nd), egz);
    s.galMed = calcFinal(medianVec(nd), egz);
    return s;
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

void printTable(const std::vector<StudentRec>& students) {
    std::cout << "\n";
    std::cout << std::left << std::setw(15) << "Vardas"
              << std::left << std::setw(15) << "Pavardė"
              << std::left << std::setw(18) << "Galutinis (Vid.)"
              << std::left << std::setw(18) << "Galutinis (Med.)"
              << "\n";
    std::cout << std::string(66, '-') << "\n";

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& s : students) {
        std::cout << std::left << std::setw(15) << s.vardas
                  << std::left << std::setw(15) << s.pavarde
                  << std::left << std::setw(18) << s.galVid
                  << std::left << std::setw(18) << s.galMed
                  << "\n";
    }
}

static void readHomeworkInteractive(std::vector<int>& nd) {
    nd.clear();
    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n";
    std::cout << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        int g = readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        nd.push_back(g);
        if (nd.size() >= 50) {
            std::cout << "Pasiektas ND limitas (50).\n";
            break;
        }
    }

    if (nd.empty()) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    try {
        std::mt19937 rng(static_cast<unsigned>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()));

        std::vector<StudentRec> students;

        while (true) {
            printMenu();
            int choice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
            if (choice == 4) break;

            if (choice == 1) {
                std::cout << "\n--- Rankinis įvedimas ---\n";
                while (true) {
                    std::string v, p;
                    if (!readNameSurname(v, p)) break;

                    std::vector<int> nd;
                    readHomeworkInteractive(nd);
                    int egz = readIntInRange("Egzamino pažymys (1..10): ", 1, 10);

                    students.push_back(makeRec(std::move(v), std::move(p), nd, egz));
                }

            } else if (choice == 2) {
                std::cout << "\n--- Vardą/pavardę įvedate, pažymius generuoja ---\n";
                int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);

                while (true) {
                    std::string v, p;
                    if (!readNameSurname(v, p)) break;

                    std::vector<int> nd;
                    nd.reserve(static_cast<size_t>(k));
                    for (int i = 0; i < k; ++i) nd.push_back(rndGrade(rng));
                    int egz = rndGrade(rng);

                    students.push_back(makeRec(std::move(v), std::move(p), nd, egz));
                }

            } else if (choice == 3) {
                std::cout << "\n--- Generuojami vardai, pavardės ir pažymiai ---\n";
                int genM = readIntInRange("Kiek studentų sugeneruoti? (1..200): ", 1, 200);
                int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);

                static const std::vector<std::string> maleNames = {
                    "Arvydas","Rimas","Mantas","Lukas","Tomas","Paulius","Jonas","Darius"
                };
                static const std::vector<std::string> femaleNames = {
                    "Ieva","Gabija","Egle","Monika","Austeja","Greta","Juste","Ugne"
                };
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
                    std::string v, p;
                    const bool isFemale = pickFemale(rng);
                    if (isFemale) {
                        v = femaleNames[dFemaleName(rng)];
                        p = femaleSurnames[dFemaleSur(rng)];
                    } else {
                        v = maleNames[dMaleName(rng)];
                        p = maleSurnames[dMaleSur(rng)];
                    }

                    std::vector<int> nd;
                    nd.reserve(static_cast<size_t>(k));
                    for (int j = 0; j < k; ++j) nd.push_back(rndGrade(rng));
                    int egz = rndGrade(rng);

                    students.push_back(makeRec(std::move(v), std::move(p), nd, egz));
                }
            }
        }

        if (students.empty()) {
            std::cout << "\nNėra įvestų studentų.\n";
            return 0;
        }

        printTable(students);
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }
}
