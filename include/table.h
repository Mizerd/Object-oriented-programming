#pragma once

#include "student_container.h"

#include <cstddef>
#include <iosfwd>

struct Cols {
    std::size_t wV = 0;
    std::size_t wP = 0;
    std::size_t wG = 0;
};

void printTable(std::ostream& out,
                const StudentContainer& students,
                std::size_t surnameExtraGapCols = 18);
