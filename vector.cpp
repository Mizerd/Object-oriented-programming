#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

// v0.1.4 - rikiavimas + išvedimas į failą (be paskutinio „polish“ lygio)
struct StudentRec {
    std::string vardas;
    std::string pavarde;
    double galVid = 0.0;
    double galMed = 0.0;
};

static std::string trimAscii(std::string s) {
    auto isTrim = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r'; };
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
static std::string readLineTrimmedPrompted(const std::string& prompt) {
    return trimAscii(readLinePrompted(prompt));
}

int readIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        const std::string line = readLineTrimmedPrompted(prompt);
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
        const std::string line = readLineTrimmedPrompted(
            "\nĮveskite: vardas pavardė (arba 0, kad baigti): ");
        if (line == "0") return false;
        if (line.empty()) continue;

        std::istringstream iss(line);
        if (!(iss >> v)) continue;
        if (v == "0") return false;
        if (!(iss >> p)) {
            std::cout << "Klaida: įveskite ir pavardę.\n";
            continue;
        }
        return true;
    }
}

static double calcFinal(double ndValue, int egz) {
    return kNdWeight * ndValue + kEgzWeight * static_cast<double>(egz);
}

static double medianFromSorted(const std::vector<int>& sorted) {
    if (sorted.empty()) return 0.0;
    const size_t n = sorted.size();
    if (n % 2 == 1) return static_cast<double>(sorted[n / 2]);
    return (static_cast<double>(sorted[n / 2 - 1]) + static_cast<double>(sorted[n / 2])) / 2.0;
}

static double averageVec(const std::vector<int>& v) {
    if (v.empty()) return 0.0;
    long long sum = 0;
    for (int x : v) sum += x;
    return static_cast<double>(sum) / static_cast<double>(v.size());
}

static double medianVec(std::vector<int> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    return medianFromSorted(v);
}

static int rndGrade(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 10);
    return dist(rng);
}

static void readHomeworkInteractive(std::vector<int>& nd) {
    nd.clear();
    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n"
              << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        const int g = readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        nd.push_back(g);
    }

    if (nd.empty()) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

static void printHeader(std::ostream& out, size_t wV, size_t wP) {
    const size_t wG = 18;
    out << std::left << std::setw(static_cast<int>(wV)) << "Vardas"
        << std::left << std::setw(static_cast<int>(wP)) << "Pavardė"
        << std::left << std::setw(static_cast<int>(wG)) << "Galutinis (Vid.)"
        << std::left << std::setw(static_cast<int>(wG)) << "Galutinis (Med.)"
        << '\n';
    out << std::string(wV + wP + wG + wG, '-') << '\n';
}

static void printTable(std::ostream& out,
                       const std::vector<StudentRec>& students,
                       size_t maxVLen,
                       size_t maxPLen) {
    const size_t wV = std::max<size_t>(maxVLen, std::string("Vardas").size()) + 2;
    const size_t wP = std::max<size_t>(maxPLen, std::string("Pavardė").size()) + 2;
    const size_t wG = 18;

    printHeader(out, wV, wP);

    out << std::fixed << std::setprecision(2);
    for (const auto& s : students) {
        out << std::left << std::setw(static_cast<int>(wV)) << s.vardas
            << std::left << std::setw(static_cast<int>(wP)) << s.pavarde
            << std::left << std::setw(static_cast<int>(wG)) << s.galVid
            << std::left << std::setw(static_cast<int>(wG)) << s.galMed
            << '\n';
    }
}

static size_t detectNdCountFromHeaderLine(const std::string& headerLine) {
    std::istringstream iss(headerLine);
    std::vector<std::string> tokens;
    for (std::string t; iss >> t;) tokens.push_back(std::move(t));
    if (tokens.size() < 4) {
        throw std::runtime_error("Neteisinga antraštė: turi būti bent 4 stulpeliai (Vardas Pavarde ND.. Egz).");
    }
    return tokens.size() - 3;
}

static void readStudentsFromFileVec(const std::string& filename,
                                    std::vector<StudentRec>& out,
                                    size_t& maxVLen,
                                    size_t& maxPLen) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Nepavyko atidaryti failo: " + filename);

    std::string headerLine;
    if (!std::getline(in, headerLine)) {
        throw std::runtime_error("Failas tuščias arba nepavyko nuskaityti antraštės: " + filename);
    }

    const size_t ndCount = detectNdCountFromHeaderLine(headerLine);
    std::vector<int> nd(ndCount);
    std::vector<int> tmp(ndCount);

    std::string v, p;
    size_t lineNo = 1;
    while (in >> v >> p) {
        ++lineNo;

        long long sum = 0;
        for (size_t i = 0; i < ndCount; ++i) {
            if (!(in >> nd[i])) throw std::runtime_error("Trūksta ND reikšmių eilutėje " + std::to_string(lineNo));
            sum += nd[i];
        }

        int egz = 0;
        if (!(in >> egz)) throw std::runtime_error("Trūksta egzamino pažymio eilutėje " + std::to_string(lineNo));

        const double avg = ndCount ? (static_cast<double>(sum) / static_cast<double>(ndCount)) : 0.0;
        tmp = nd;
        std::sort(tmp.begin(), tmp.end());
        const double med = medianFromSorted(tmp);

        StudentRec s;
        s.vardas = std::move(v);
        s.pavarde = std::move(p);
        s.galVid = calcFinal(avg, egz);
        s.galMed = calcFinal(med, egz);

        maxVLen = std::max(maxVLen, s.vardas.size());
        maxPLen = std::max(maxPLen, s.pavarde.size());
        out.push_back(std::move(s));
    }

    if (out.empty()) throw std::runtime_error("Faile nerasta studentų įrašų: " + filename);
}

enum class SortKey {
    ByName = 1,
    BySurname = 2,
    ByFinalAvg = 3,
    ByFinalMed = 4
};

static void sortStudents(std::vector<StudentRec>& students, SortKey key) {
    auto byName = [](const StudentRec& a, const StudentRec& b) {
        if (a.vardas != b.vardas) return a.vardas < b.vardas;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.galVid < b.galVid;
    };
    auto bySurname = [](const StudentRec& a, const StudentRec& b) {
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        if (a.vardas != b.vardas) return a.vardas < b.vardas;
        return a.galVid < b.galVid;
    };
    auto byFinalAvg = [](const StudentRec& a, const StudentRec& b) {
        if (a.galVid != b.galVid) return a.galVid < b.galVid;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.vardas < b.vardas;
    };
    auto byFinalMed = [](const StudentRec& a, const StudentRec& b) {
        if (a.galMed != b.galMed) return a.galMed < b.galMed;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.vardas < b.vardas;
    };

    switch (key) {
        case SortKey::ByName:     std::sort(students.begin(), students.end(), byName);     break;
        case SortKey::BySurname:  std::sort(students.begin(), students.end(), bySurname);  break;
        case SortKey::ByFinalAvg: std::sort(students.begin(), students.end(), byFinalAvg); break;
        case SortKey::ByFinalMed: std::sort(students.begin(), students.end(), byFinalMed); break;
    }
}

static void menuV01Style(std::vector<StudentRec>& students, size_t& maxVLen, size_t& maxPLen) {
    std::mt19937 rng(static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()));

    while (true) {
        std::cout
            << "\nMeniu:\n"
            << "1 - Įvesti ranka (vardas pavardė, tada ND ir egz)\n"
            << "2 - Generuoti pažymius (vardą/pavardę įvedate)\n"
            << "3 - Generuoti vardus, pavardes ir pažymius\n"
            << "4 - Baigti darbą\n";

        const int choice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        if (choice == 4) break;

        if (choice == 1) {
            std::vector<int> nd;
            while (true) {
                std::string v, p;
                if (!readNameSurname(v, p)) break;
                readHomeworkInteractive(nd);
                const int egz = readIntInRange("Egzamino pažymys (1..10): ", 1, 10);

                StudentRec s;
                s.vardas = std::move(v);
                s.pavarde = std::move(p);
                s.galVid = calcFinal(averageVec(nd), egz);
                s.galMed = calcFinal(medianVec(nd), egz);

                maxVLen = std::max(maxVLen, s.vardas.size());
                maxPLen = std::max(maxPLen, s.pavarde.size());
                students.push_back(std::move(s));
            }

        } else if (choice == 2) {
            const int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);
            std::vector<int> nd;
            nd.reserve(static_cast<size_t>(k));

            while (true) {
                std::string v, p;
                if (!readNameSurname(v, p)) break;

                nd.clear();
                for (int i = 0; i < k; ++i) nd.push_back(rndGrade(rng));
                const int egz = rndGrade(rng);

                StudentRec s;
                s.vardas = std::move(v);
                s.pavarde = std::move(p);
                s.galVid = calcFinal(averageVec(nd), egz);
                s.galMed = calcFinal(medianVec(nd), egz);

                maxVLen = std::max(maxVLen, s.vardas.size());
                maxPLen = std::max(maxPLen, s.pavarde.size());
                students.push_back(std::move(s));
            }

        } else if (choice == 3) {
            const int genM = readIntInRange("Kiek studentų sugeneruoti? (1..200): ", 1, 200);
            const int k = readIntInRange("Kiek ND generuoti kiekvienam studentui? (1..50): ", 1, 50);

            // jau naudojame lietuviškas raides (pasiruošimas v0.2 UTF-8 komentarui)
            static const std::vector<std::string> maleNames = {
                "Arvydas", "Rimas", "Mantas", "Lukas", "Tomas", "Paulius", "Jonas", "Darius"
            };
            static const std::vector<std::string> femaleNames = {
                "Ieva", "Gabija", "Eglė", "Monika", "Austėja", "Greta", "Justė", "Ugnė"
            };
            static const std::vector<std::string> maleSurnames = {
                "Sabonis", "Kurtinaitis", "Petrauskas", "Kazlauskas", "Vaitkus",
                "Stankevičius", "Brazdeikis", "Jankauskas"
            };
            static const std::vector<std::string> femaleSurnames = {
                "Sabonytė", "Kurtinaitytė", "Petrauskaitė", "Kazlauskaitė", "Vaitkutė",
                "Stankevičiūtė", "Brazdeikytė", "Jankauskaitė"
            };

            std::uniform_int_distribution<size_t> dMaleName(0, maleNames.size() - 1);
            std::uniform_int_distribution<size_t> dFemaleName(0, femaleNames.size() - 1);
            std::uniform_int_distribution<size_t> dMaleSur(0, maleSurnames.size() - 1);
            std::uniform_int_distribution<size_t> dFemaleSur(0, femaleSurnames.size() - 1);
            std::bernoulli_distribution pickFemale(0.5);

            std::vector<int> nd;
            nd.reserve(static_cast<size_t>(k));

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

                s.galVid = calcFinal(averageVec(nd), egz);
                s.galMed = calcFinal(medianVec(nd), egz);

                maxVLen = std::max(maxVLen, s.vardas.size());
                maxPLen = std::max(maxPLen, s.pavarde.size());
                students.push_back(std::move(s));
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<StudentRec> students;
    students.reserve(10000);

    size_t maxVLen = 0;
    size_t maxPLen = 0;

    try {
        std::cout
            << "v0.1.4 - Studentų galutinio balo skaičiuoklė\n"
            << "1 - Nuskaityti studentus iš failo\n"
            << "2 - Įvesti / generuoti (v0.1 meniu)\n";

        const int mode = readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (mode == 1) {
            std::string filename = readLineTrimmedPrompted(
                "\nĮveskite failo pavadinimą (pvz. kursiokai.txt, studentai10000.txt):\n> ");
            if (filename.empty()) filename = "kursiokai.txt";

            readStudentsFromFileVec(filename, students, maxVLen, maxPLen);
        } else {
            menuV01Style(students, maxVLen, maxPLen);
        }

        if (students.empty()) {
            std::cout << "\nNėra studentų duomenų.\n";
            return 0;
        }

        std::cout
            << "\nRikiavimas:\n"
            << "1 - pagal vardą\n"
            << "2 - pagal pavardę\n"
            << "3 - pagal Galutinis (Vid.)\n"
            << "4 - pagal Galutinis (Med.)\n";

        const int sortChoice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        sortStudents(students, static_cast<SortKey>(sortChoice));

        std::cout
            << "\nIšvedimas:\n"
            << "1 - į ekraną\n"
            << "2 - į failą\n";

        const int outChoice = readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (outChoice == 1) {
            printTable(std::cout, students, maxVLen, maxPLen);
        } else {
            std::string outName = readLineTrimmedPrompted(
                "\nĮveskite rezultatų failo pavadinimą (pvz. rezultatai.txt):\n> ");
            if (outName.empty()) outName = "rezultatai.txt";

            std::ofstream out(outName);
            if (!out) throw std::runtime_error("Nepavyko sukurti rezultatų failo: " + outName);

            printTable(out, students, maxVLen, maxPLen);
            std::cout << "\nRezultatai įrašyti į failą: " << outName << "\n";
        }

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }
}
