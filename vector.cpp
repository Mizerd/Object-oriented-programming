#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <algorithm>
#include <chrono>
#include <clocale>
#include <cwchar>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <wchar.h>

// ============================================================
// Student final-grade calculator (Linux-only, C++17)
//
// Key fixes in this version:
//  - FIX: All interactive input uses getline-based helpers.
//         (No mixing std::cin >> ... with std::getline on std::cin.)
//  - FIX: All prompts flush before reading input to avoid "prompt appears late".
//  - FIX: UTF-8 Lithuanian alignment in terminal/file via mbsrtowcs + wcwidth (POSIX).
//  - FIX: Robust UTF-8 locale initialization with graceful fallback.
//  - UPDATE: Generation limits: up to 10,000 students; ND per student up to 1,000.
//  - UPDATE: File output uses a larger gap between surname and grade columns.
//
// Build:
//   g++ -std=c++17 -O2 -g vector.cpp -o vector
// ============================================================

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

constexpr int kMaxStudentsGenerate = 10000;
constexpr int kMaxNdPerStudent     = 1000;

struct StudentRec {
    std::string vardas;
    std::string pavarde;
    double galVid = 0.0;
    double galMed = 0.0;
};

// ============================
// Locale + UTF-8 display width (POSIX)
// ============================

static bool utf8ConversionWorks() {
    // If this converts, we can reliably use mbsrtowcs + wcwidth for UTF-8.
    const char* sample = u8"Ąčęėįšųūž";
    std::mbstate_t st{};
    const char* src = sample;
    const size_t n = std::mbsrtowcs(nullptr, &src, 0, &st);
    return n != static_cast<size_t>(-1);
}

static void initUtf8Locale() {
    // FIX: enable the user locale first; it controls LC_CTYPE used by mbsrtowcs.
    (void)std::setlocale(LC_ALL, "");
    if (utf8ConversionWorks()) return;

    // Common UTF-8 locale fallbacks found on many Linux distros.
    for (const char* candidate : {"C.UTF-8", "C.utf8", "lt_LT.UTF-8", "en_US.UTF-8"}) {
        if (std::setlocale(LC_ALL, candidate) && utf8ConversionWorks()) return;
    }

    // Graceful fallback: we keep running; alignment may be imperfect.
    std::cerr
        << "[WARN] UTF-8 locale not active; Lithuanian column alignment may be imperfect.\n"
        << "       Try: export LANG=C.UTF-8; export LC_ALL=C.UTF-8\n";
}

static size_t displayWidthUtf8(const std::string& s) {
    if (s.empty()) return 0;

    std::mbstate_t st{};
    const char* src = s.c_str();

    const size_t needed = std::mbsrtowcs(nullptr, &src, 0, &st);
    if (needed == static_cast<size_t>(-1)) {
        // Fallback: assume 1 column per byte.
        return s.size();
    }

    std::wstring w;
    w.resize(needed);

    st = std::mbstate_t{};
    src = s.c_str();
    const size_t converted = std::mbsrtowcs(w.data(), &src, w.size(), &st);
    if (converted == static_cast<size_t>(-1)) return s.size();

    size_t cols = 0;
    for (wchar_t wc : w) {
        const int cw = ::wcwidth(wc);
        if (cw >= 0) cols += static_cast<size_t>(cw);
        else cols += 1; // non-printable => avoid breaking layout
    }
    return cols;
}

static void printPaddedRight(std::ostream& out, const std::string& text, size_t widthCols) {
    // Left-align text in a widthCols column
    out << text;
    const size_t used = displayWidthUtf8(text);
    if (used < widthCols) out << std::string(widthCols - used, ' ');
}

static void printPaddedLeft(std::ostream& out, const std::string& text, size_t widthCols) {
    // Right-align text in a widthCols column
    const size_t used = displayWidthUtf8(text);
    if (used < widthCols) out << std::string(widthCols - used, ' ');
    out << text;
}

// ============================
// Input helpers (FIX: line-based + flushed prompts)
// ============================

static std::string trimAscii(std::string s) {
    auto isTrim = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r'; };
    size_t b = 0;
    while (b < s.size() && isTrim(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && isTrim(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

static std::string readLinePrompted(const std::string& prompt) {
    // FIX: flush prompt so it appears before input is read
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("Input stream closed (EOF).");
    }
    return line;
}

static std::string readLineTrimmedPrompted(const std::string& prompt) {
    return trimAscii(readLinePrompted(prompt));
}

static int readIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        const std::string line = readLineTrimmedPrompted(prompt);
        if (line.empty()) {
            std::cout << "Klaida: įveskite skaičių.\n";
            continue;
        }

        std::istringstream iss(line);
        int x;
        if (!(iss >> x)) {
            std::cout << "Klaida: įveskite skaičių.\n";
            continue;
        }

        iss >> std::ws;
        if (!iss.eof()) {
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

static bool readNameSurname(std::string& vardas, std::string& pavarde) {
    while (true) {
        const std::string line =
            readLineTrimmedPrompted("\nĮveskite: vardas pavardė (arba 0, kad baigti): ");

        if (line == "0") return false;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string v, p;
        if (!(iss >> v)) continue;
        if (v == "0") return false;

        if (!(iss >> p)) {
            std::cout << "Klaida: įveskite ir pavardę (formatas: vardas pavardė).\n";
            continue;
        }

        vardas = std::move(v);
        pavarde = std::move(p);
        return true;
    }
}

// ============================
// Grade calculations
// ============================

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

// ============================
// File import
// ============================

// Expected header format: Vardas Pavarde ND1 ND2 ... Egz
static size_t detectNdCountFromHeaderLine(const std::string& headerLine) {
    std::istringstream iss(headerLine);
    std::vector<std::string> tokens;
    for (std::string t; iss >> t;) tokens.push_back(std::move(t));

    if (tokens.size() < 4) {
        throw std::runtime_error(
            "Neteisinga antraštė: turi būti bent 4 stulpeliai (Vardas Pavarde ND.. Egz).");
    }
    // Vardas, Pavarde, ...ND..., Egz
    return tokens.size() - 3;
}

static void readStudentsFromFileVec(const std::string& filename,
                                    std::vector<StudentRec>& out,
                                    size_t& maxVCols,
                                    size_t& maxPCols) {
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
    size_t lineNo = 1; // header
    while (in >> v >> p) {
        ++lineNo;

        long long sum = 0;
        for (size_t i = 0; i < ndCount; ++i) {
            int g;
            if (!(in >> g)) {
                throw std::runtime_error("Trūksta ND reikšmių eilutėje " + std::to_string(lineNo));
            }
            nd[i] = g;
            sum += g;
        }

        int egz;
        if (!(in >> egz)) {
            throw std::runtime_error("Trūksta egzamino pažymio eilutėje " + std::to_string(lineNo));
        }

        const double avg = ndCount ? (static_cast<double>(sum) / static_cast<double>(ndCount)) : 0.0;
        tmp = nd;
        std::sort(tmp.begin(), tmp.end());
        const double med = medianFromSorted(tmp);

        StudentRec s;
        s.vardas = std::move(v);
        s.pavarde = std::move(p);
        s.galVid = calcFinal(avg, egz);
        s.galMed = calcFinal(med, egz);

        maxVCols = std::max(maxVCols, displayWidthUtf8(s.vardas));
        maxPCols = std::max(maxPCols, displayWidthUtf8(s.pavarde));
        out.push_back(std::move(s));
    }

    if (out.empty()) {
        throw std::runtime_error("Faile nerasta studentų įrašų (gal neteisingas formatas?): " + filename);
    }
}

static std::string fmt2(double x) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << x;
    return oss.str();
}

struct Cols {
    size_t wV = 0;
    size_t wP = 0;
    size_t wG = 0;
};

static Cols computeCols(size_t maxVCols, size_t maxPCols, size_t surnameExtraGapCols) {
    const std::string hV  = "Vardas";
    const std::string hP  = "Pavardė";
    const std::string hG1 = "Galutins vidurkis";
    const std::string hG2 = "Galutinė mediana";

    Cols c;
    c.wV = std::max(maxVCols, displayWidthUtf8(hV)) + 2;
    c.wP = std::max(maxPCols, displayWidthUtf8(hP)) + surnameExtraGapCols;

    const size_t hG = std::max(displayWidthUtf8(hG1), displayWidthUtf8(hG2));
    // Keep room for numbers like 100.00
    c.wG = std::max<size_t>(hG, 12) + 2;

    return c;
}

static void printHeader(std::ostream& out, const Cols& c) {
    printPaddedRight(out, "Vardas", c.wV);
    printPaddedRight(out, "Pavardė", c.wP);
    printPaddedRight(out, "Galutinis vidurkis", c.wG);
    printPaddedRight(out, "Galutinė mediana", c.wG);
    out << '\n';

    out << std::string(c.wV + c.wP + c.wG + c.wG, '-') << '\n';
}

static void printTable(std::ostream& out,
                       const std::vector<StudentRec>& students,
                       size_t maxVCols,
                       size_t maxPCols,
                       size_t surnameExtraGapCols) {
    const Cols c = computeCols(maxVCols, maxPCols, surnameExtraGapCols);

    printHeader(out, c);

    for (const auto& s : students) {
        printPaddedRight(out, s.vardas, c.wV);
        printPaddedRight(out, s.pavarde, c.wP);
        printPaddedLeft(out, fmt2(s.galVid), c.wG);
        printPaddedLeft(out, fmt2(s.galMed), c.wG);
        out << '\n';
    }
}

// ============================
// Sorting
// ============================

enum class SortKey { ByName = 1, BySurname = 2, ByFinalAvg = 3, ByFinalMed = 4 };

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

// ============================
// Interactive menu (v0.1 style)
// ============================

static int rndGrade(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(1, 10);
    return dist(rng);
}

static void readHomeworkInteractive(std::vector<int>& nd) {
    // UPDATE: reserve enough to support up to kMaxNdPerStudent comfortably.
    nd.clear();
    nd.reserve(kMaxNdPerStudent);

    std::cout << "Namų darbų įvedimas: įveskite pažymius (1..10).\n"
              << "Įvedus 0 - namų darbų įvedimas BAIGIAMAS.\n";

    while (true) {
        if (static_cast<int>(nd.size()) >= kMaxNdPerStudent) {
            std::cout << "Pasiektas maksimalus ND kiekis (" << kMaxNdPerStudent
                      << ").\n";
            break;
        }

        const int g = readIntInRange("ND: ", 0, 10);
        if (g == 0) break;
        nd.push_back(g);
    }

    if (nd.empty()) {
        std::cout << "Įspėjimas: neįvestas nė vienas ND. ND dalis bus 0.\n";
    }
}

static void menuV01Style(std::vector<StudentRec>& students, size_t& maxVCols, size_t& maxPCols) {
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
            std::cout << "\n--- Rankinis įvedimas ---\n";
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

                maxVCols = std::max(maxVCols, displayWidthUtf8(s.vardas));
                maxPCols = std::max(maxPCols, displayWidthUtf8(s.pavarde));
                students.push_back(std::move(s));
            }

        } else if (choice == 2) {
            std::cout << "\n--- Vardą/pavardę įvedate, pažymius generuoja ---\n";
            const int k = readIntInRange(
                "Kiek ND generuoti kiekvienam studentui? (1..1000): ", 1, kMaxNdPerStudent);

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

                maxVCols = std::max(maxVCols, displayWidthUtf8(s.vardas));
                maxPCols = std::max(maxPCols, displayWidthUtf8(s.pavarde));
                students.push_back(std::move(s));
            }

        } else if (choice == 3) {
            std::cout << "\n--- Generuojami vardai, pavardės ir pažymiai ---\n";
            const int genM = readIntInRange(
                "Kiek studentų sugeneruoti? (1..10000): ", 1, kMaxStudentsGenerate);
            const int k = readIntInRange(
                "Kiek ND generuoti kiekvienam studentui? (1..1000): ", 1, kMaxNdPerStudent);

            // UPDATE: reserve heap capacity for up to genM additional students.
            students.reserve(students.size() + static_cast<size_t>(genM));

            // UTF-8 Lithuanian letters included to validate output/alignment.
            static const std::vector<std::string> maleNames = {
                "Arvydas","Rimas","Mantas","Lukas","Tomas","Paulius","Jonas","Darius"
            };
            static const std::vector<std::string> femaleNames = {
                "Ieva","Gabija","Eglė","Monika","Austėja","Greta","Justė","Ugnė"
            };
            static const std::vector<std::string> maleSurnames = {
                "Sabonis","Kurtinaitis","Petrauskas","Kazlauskas","Vaitkus",
                "Stankevičius","Brazdeikis","Jankauskas"
            };
            static const std::vector<std::string> femaleSurnames = {
                "Sabonytė","Kurtinaitytė","Petrauskaitė","Kazlauskaitė","Vaitkutė",
                "Stankevičiūtė","Brazdeikytė","Jankauskaitė"
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

                maxVCols = std::max(maxVCols, displayWidthUtf8(s.vardas));
                maxPCols = std::max(maxPCols, displayWidthUtf8(s.pavarde));
                students.push_back(std::move(s));
            }
        }
    }
}

// ============================
// main
// ============================

int main() {
    initUtf8Locale();

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<StudentRec> students;
    // UPDATE: reserve heap capacity for large interactive workloads (up to 10k).
    students.reserve(kMaxStudentsGenerate);

    size_t maxVCols = 0;
    size_t maxPCols = 0;

    try {
        std::cout
            << "v0.2 - Studentų galutinio balo skaičiuoklė\n"
            << "1 - Nuskaityti studentus iš failo\n"
            << "2 - Įvesti / generuoti (v0.1 meniu)\n";

        const int mode = readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (mode == 1) {
            std::string filename = readLineTrimmedPrompted(
                "\nĮveskite failo pavadinimą (pvz. kursiokai.txt, studentai10000.txt):\n> ");
            if (filename.empty()) {
                filename = "kursiokai.txt";
                std::cout << "Naudosime numatytą failą: " << filename << "\n";
            }
            readStudentsFromFileVec(filename, students, maxVCols, maxPCols);
        } else {
            menuV01Style(students, maxVCols, maxPCols);
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

        const int sortChoice = readIntInRange("Pasirinkimas (1-4): ", 1, 4);
        sortStudents(students, static_cast<SortKey>(sortChoice));

        std::cout
            << "\nIšvedimas:\n"
            << "1 - į ekraną\n"
            << "2 - į failą\n";

        const int outChoice = readIntInRange("Pasirinkimas (1-2): ", 1, 2);

        if (outChoice == 1) {
            // Terminal: small extra gap after surname
            printTable(std::cout, students, maxVCols, maxPCols, /*surname gap*/4);
        } else {
            std::string outName = readLineTrimmedPrompted(
                "\nĮveskite rezultatų failo pavadinimą (pvz. rezultatai.txt):\n> ");
            if (outName.empty()) outName = "rezultatai.txt";

            std::ofstream out(outName);
            if (!out) throw std::runtime_error("Nepavyko sukurti rezultatų failo: " + outName);

            // File: bigger surname->grades gap as requested
            printTable(out, students, maxVCols, maxPCols, /*surname gap*/12);
            std::cout << "\nRezultatai įrašyti į failą: " << outName << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "\n[KLAIDA] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
