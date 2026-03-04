#include "table.h"

#include "utf8.h"

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

static std::string fmt2(double x) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << x;
    return oss.str();
}

static Cols computeCols(std::size_t maxVCols, std::size_t maxPCols, std::size_t surnameExtraGapCols) {
    const std::string hV  = "Vardas";
    const std::string hP  = "Pavardė";
    const std::string hG1 = "Galutinis vidurkis";
    const std::string hG2 = "Galutinė mediana";

    Cols c;
    c.wV = std::max(maxVCols, utf8::displayWidth(hV)) + 2;
    c.wP = std::max(maxPCols, utf8::displayWidth(hP)) + surnameExtraGapCols;

    const std::size_t hG = std::max(utf8::displayWidth(hG1), utf8::displayWidth(hG2));
    c.wG = std::max<std::size_t>(hG, 12) + 2;

    return c;
}

static void printHeader(std::ostream& out, const Cols& c) {
    utf8::printPaddedRight(out, "Vardas", c.wV);
    utf8::printPaddedRight(out, "Pavardė", c.wP);
    utf8::printPaddedRight(out, "Galutinis vidurkis", c.wG);
    utf8::printPaddedRight(out, "Galutinė mediana", c.wG);
    out << '\n';

    out << std::string(c.wV + c.wP + c.wG + c.wG, '-') << '\n';
}

void printTable(std::ostream& out,
                const std::vector<StudentRec>& students,
                std::size_t maxVCols,
                std::size_t maxPCols,
                std::size_t surnameExtraGapCols) {
    const Cols c = computeCols(maxVCols, maxPCols, surnameExtraGapCols);

    printHeader(out, c);

    for (const auto& s : students) {
        utf8::printPaddedRight(out, s.vardas, c.wV);
        utf8::printPaddedRight(out, s.pavarde, c.wP);
        utf8::printPaddedLeft(out, fmt2(s.galVid), c.wG);
        utf8::printPaddedLeft(out, fmt2(s.galMed), c.wG);
        out << '\n';
    }
}
