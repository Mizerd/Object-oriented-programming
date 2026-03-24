#include "fileio.h"

#include "grades.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void validateGradeRange(int x, const std::string& what) {
    if (x < 1 || x > 10) {
        throw std::runtime_error("Neteisingas pažymys (" + std::to_string(x) + ") lauke: " + what +
                                 ". Leistinas intervalas: 1..10.");
    }
}

std::size_t detectNdCountFromHeaderLine(const std::string& headerLine) {
    std::istringstream hs(headerLine);
    std::vector<std::string> tokens;
    std::string tok;

    while (hs >> tok) {
        tokens.push_back(tok);
    }

    if (tokens.size() < 3) {
        throw std::runtime_error("Neteisinga failo antraštė: per mažai stulpelių.");
    }

    if (tokens.front() != "Vardas" || tokens[1] != "Pavardė") {
        throw std::runtime_error("Neteisinga failo antraštė: pirmi stulpeliai turi būti 'Vardas Pavardė'.");
    }

    if (tokens.back() != "Egz") {
        throw std::runtime_error("Neteisinga failo antraštė: paskutinis stulpelis turi būti 'Egz'.");
    }

    return tokens.size() - 3;
}

} // namespace

void readStudentsFromFile(const std::string& filename,
                          StudentContainer& out,
                          const Weights& weights) {
    out.clear();

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

    std::string v;
    std::string p;
    std::size_t lineNo = 1;

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

        out.push_back(std::move(s));
    }

    if (out.empty()) {
        throw std::runtime_error("Faile nerasta studentų įrašų (gal neteisingas formatas?): " + filename);
    }
}
