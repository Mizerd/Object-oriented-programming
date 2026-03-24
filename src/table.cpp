#include "table.h"

#include "utf8.h"

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

namespace {

std::string fmt2(double x) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << x;
    return oss.str();
}

Cols computeCols(const StudentContainer& students, std::size_t surnameExtraGapCols) {
    const std::string hV = "Vardas";
    const std::string hP = "Pavardė";
    const std::string hG1 = "Galutinis vidurkis";
    const std::string hG2 = "Galutinė mediana";

    std::size_t maxVCols = utf8::displayWidth(hV);
    std::size_t maxPCols = utf8::displayWidth(hP);

    for (const auto& s : students) {
        maxVCols = std::max(maxVCols, utf8::displayWidth(s.vardas));
        maxPCols = std::max(maxPCols, utf8::displayWidth(s.pavarde));
    }

    Cols c;
    c.wV = maxVCols + 2;
    c.wP = maxPCols + surnameExtraGapCols;

    const std::size_t hG = std::max(utf8::displayWidth(hG1), utf8::displayWidth(hG2));
    c.wG = std::max<std::size_t>(hG, 12) + 2;

    return c;
}

void printHeader(std::ostream& out, const Cols& c) {
    utf8::printPaddedRight(out, "Vardas", c.wV);
    utf8::printPaddedRight(out, "Pavardė", c.wP);
    utf8::printPaddedRight(out, "Galutinis vidurkis", c.wG);
    utf8::printPaddedRight(out, "Galutinė mediana", c.wG);
    out << '\n';

    out << std::string(c.wV + c.wP + c.wG + c.wG, '-') << '\n';
}

} // namespace

void printTable(std::ostream& out,
                const StudentContainer& students,
                std::size_t surnameExtraGapCols) {
    const Cols c = computeCols(students, surnameExtraGapCols);

    printHeader(out, c);

    for (const auto& s : students) {
        utf8::printPaddedRight(out, s.vardas, c.wV);
        utf8::printPaddedRight(out, s.pavarde, c.wP);
        utf8::printPaddedLeft(out, fmt2(s.galVid), c.wG);
        utf8::printPaddedLeft(out, fmt2(s.galMed), c.wG);
        out << '\n';
    }
}
