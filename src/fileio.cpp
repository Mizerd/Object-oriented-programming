#include "fileio.h"

#include "grades.h"
#include "utf8.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Tikimasi antraštės formato: Vardas Pavarde ND1 ND2 ... Egz
static std::size_t detectNdCountFromHeaderLine(const std::string& headerLine) {
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

static void validateGradeRange(int g, const std::string& where) {
    if (g < 0 || g > 10) {
        throw std::runtime_error("Neteisingas pažymys (" + where + "): " + std::to_string(g) +
                                 ". Leidžiama [0..10].");
    }
}

void readStudentsFromFileVec(const std::string& filename,
                             std::vector<StudentRec>& out,
                             std::size_t& maxVCols,
                             std::size_t& maxPCols,
                             const Weights& weights) {
    std::ifstream in(filename);
    if (!in) {
        throw std::runtime_error("Nepavyko atidaryti failo (ar jis egzistuoja?): " + filename);
    }

    std::string headerLine;
    if (!std::getline(in, headerLine)) {
        throw std::runtime_error("Failas tuščias arba nepavyko nuskaityti antraštės: " + filename);
    }

    const std::size_t ndCount = detectNdCountFromHeaderLine(headerLine);

    std::vector<int> nd(ndCount);
    std::vector<int> tmp(ndCount);

    std::string v, p;
    std::size_t lineNo = 1; // antraštė

    while (in >> v >> p) {
        ++lineNo;

        long long sum = 0;
        for (std::size_t i = 0; i < ndCount; ++i) {
            int g;
            if (!(in >> g)) {
                throw std::runtime_error("Neteisingas failo formatas: trūksta ND reikšmių eilutėje " +
                                         std::to_string(lineNo) + ".");
            }
            validateGradeRange(g, "ND, eilutė " + std::to_string(lineNo));
            nd[i] = g;
            sum += g;
        }

        int egz;
        if (!(in >> egz)) {
            throw std::runtime_error("Neteisingas failo formatas: trūksta egzamino pažymio eilutėje " +
                                     std::to_string(lineNo) + ".");
        }
        validateGradeRange(egz, "egzaminas, eilutė " + std::to_string(lineNo));

        const double avg = ndCount ? (static_cast<double>(sum) / static_cast<double>(ndCount)) : 0.0;
        tmp = nd;
        std::sort(tmp.begin(), tmp.end());
        const double med = grade::medianFromSorted(tmp);

        StudentRec s;
        s.vardas = std::move(v);
        s.pavarde = std::move(p);
        s.galVid = grade::calcFinal(avg, egz, weights);
        s.galMed = grade::calcFinal(med, egz, weights);

        maxVCols = std::max(maxVCols, utf8::displayWidth(s.vardas));
        maxPCols = std::max(maxPCols, utf8::displayWidth(s.pavarde));
        out.push_back(std::move(s));
    }

    if (out.empty()) {
        throw std::runtime_error("Faile nerasta studentų įrašų (gal neteisingas formatas?): " + filename);
    }
}
