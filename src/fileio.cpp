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
        throw std::runtime_error(
            "Neteisingas pazymys (" + std::to_string(x) + ") lauke: " + what +
            ". Leistinas intervalas: 1..10.");
    }
}

std::string stripUtf8Bom(const std::string& s) {
    if (s.size() >= 3 &&
        static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB &&
        static_cast<unsigned char>(s[2]) == 0xBF) {
        return s.substr(3);
    }
    return s;
}

bool isAcceptedSurnameHeader(const std::string& token) {
    return token == "Pavarde" || token == "Pavardė";
}

std::size_t detectNdCountFromHeaderLine(const std::string& rawHeaderLine) {
    const std::string headerLine = stripUtf8Bom(rawHeaderLine);

    std::istringstream hs(headerLine);
    std::vector<std::string> tokens;
    std::string tok;

    while (hs >> tok) {
        tokens.push_back(tok);
    }

    if (tokens.size() < 3) {
        throw std::runtime_error("Neteisinga failo antraste: per mazai stulpeliu.");
    }

    if (tokens.front() != "Vardas" || !isAcceptedSurnameHeader(tokens[1])) {
        throw std::runtime_error(
            "Neteisinga failo antraste: pirmi stulpeliai turi buti "
            "'Vardas Pavarde' arba 'Vardas Pavardė'.");
    }

    if (tokens.back() != "Egz") {
        throw std::runtime_error(
            "Neteisinga failo antraste: paskutinis stulpelis turi buti 'Egz'.");
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
        throw std::runtime_error("Failas tuscias arba nepavyko nuskaityti antrastes: " + filename);
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
                throw std::runtime_error(
                    "Neteisingas failo formatas: truksta ND reiksmiu eiluteje " +
                    std::to_string(lineNo) + ".");
            }
            validateGradeRange(g, "ND, eilute " + std::to_string(lineNo));
            nd[i] = g;
            sum += g;
        }

        int egz;
        if (!(in >> egz)) {
            throw std::runtime_error(
                "Neteisingas failo formatas: truksta egzamino pazymio eiluteje " +
                std::to_string(lineNo) + ".");
        }
        validateGradeRange(egz, "egzaminas, eilute " + std::to_string(lineNo));

        const double avg = ndCount
            ? (static_cast<double>(sum) / static_cast<double>(ndCount))
            : 0.0;

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
        throw std::runtime_error(
            "Faile nerasta studentu irasu (gal neteisingas formatas?): " + filename);
    }
}
