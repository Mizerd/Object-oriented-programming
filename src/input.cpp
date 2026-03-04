#include "input.h"

#include <cctype>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace inp {

static std::string trimAscii(std::string s) {
    auto isTrim = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\r'; };
    std::size_t b = 0;
    while (b < s.size() && isTrim(static_cast<unsigned char>(s[b]))) ++b;
    std::size_t e = s.size();
    while (e > b && isTrim(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

static char toLowerAscii(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

std::string readLinePrompted(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw std::runtime_error("Įvesties srautas uždarytas (EOF).");
    }
    return line;
}

std::string readLineTrimmedPrompted(const std::string& prompt) {
    return trimAscii(readLinePrompted(prompt));
}

static int parseIntStrict(const std::string& text) {
    if (text.empty()) throw std::runtime_error("Nieko neįvedėte. Reikia sveiko skaičiaus.");

    std::istringstream iss(text);
    int x;
    if (!(iss >> x)) throw std::runtime_error("Neteisinga įvestis. Reikia sveiko skaičiaus.");

    iss >> std::ws;
    if (!iss.eof()) throw std::runtime_error("Neteisingas formatas (rašykite tik sveiką skaičių).");

    return x;
}

static double parseDoubleStrict(std::string text) {
    if (text.empty()) throw std::runtime_error("Nieko neįvedėte. Reikia realaus skaičiaus.");

    // Lietuviškai dažnai rašoma su kableliu.
    for (char& c : text) {
        if (c == ',') c = '.';
    }

    std::istringstream iss(text);
    double x;
    if (!(iss >> x)) throw std::runtime_error("Neteisinga įvestis. Reikia realaus skaičiaus.");

    iss >> std::ws;
    if (!iss.eof()) throw std::runtime_error("Neteisingas formatas (rašykite tik skaičių).");

    return x;
}

static std::string validateWord(const std::string& w, const std::string& fieldName) {
    if (w.empty()) throw std::runtime_error("Tuščias " + fieldName + " laukas.");

    for (unsigned char uc : w) {
        if (uc < 128) {
            // Leidžiame raides ir brūkšnelį.
            if (std::isalpha(uc) || uc == '-') continue;
            throw std::runtime_error("Neteisingas " + fieldName + ": turi būti žodis iš raidžių (leidžiamas '-').");
        }
        // UTF-8 (ne-ASCII) simbolius priimame kaip validžius.
    }

    return w;
}

int readIntInRange(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        try {
            const std::string line = readLineTrimmedPrompted(prompt);
            const int x = parseIntStrict(line);
            if (x < minVal || x > maxVal) {
                throw std::runtime_error("Reikšmė turi būti intervale [" + std::to_string(minVal) + ".." +
                                         std::to_string(maxVal) + "].");
            }
            return x;
        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
        }
    }
}

double readDoubleInRange(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        try {
            const std::string line = readLineTrimmedPrompted(prompt);
            const double x = parseDoubleStrict(line);
            if (x < minVal || x > maxVal) {
                std::ostringstream oss;
                oss << "Reikšmė turi būti intervale [" << minVal << ".." << maxVal << "].";
                throw std::runtime_error(oss.str());
            }
            return x;
        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
        }
    }
}

char readCharFromSet(const std::string& prompt, const std::string& allowed, char defaultValue) {
    while (true) {
        try {
            const std::string line = readLineTrimmedPrompted(prompt);

            if (line.empty() && defaultValue != '\0') {
                return toLowerAscii(defaultValue);
            }

            if (line.size() != 1) {
                throw std::runtime_error("Įveskite vieną raidę.");
            }

            const char c = toLowerAscii(line[0]);
            if (!std::isalpha(static_cast<unsigned char>(c))) {
                throw std::runtime_error("Įveskite raidę.");
            }

            if (allowed.find(c) == std::string::npos) {
                throw std::runtime_error("Leidžiamos reikšmės: " + allowed + ".");
            }
            return c;
        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
        }
    }
}

bool readNameSurname(std::string& vardas, std::string& pavarde) {
    while (true) {
        try {
            const std::string line = readLineTrimmedPrompted(
                "\nĮveskite: vardas pavardė (arba 0, kad baigti): ");

            if (line == "0") return false;
            if (line.empty()) throw std::runtime_error("Nieko neįvedėte.");

            std::istringstream iss(line);
            std::string v, p;
            if (!(iss >> v)) throw std::runtime_error("Įveskite vardą.");
            if (v == "0") return false;
            if (!(iss >> p)) throw std::runtime_error("Įveskite ir pavardę (formatas: vardas pavardė).");

            iss >> std::ws;
            if (!iss.eof()) throw std::runtime_error("Įveskite tik du žodžius: vardas pavardė.");

            vardas = validateWord(v, "vardas");
            pavarde = validateWord(p, "pavardė");
            return true;

        } catch (const std::exception& e) {
            std::cout << "Klaida: " << e.what() << "\n";
        }
    }
}

} // namespace inp
