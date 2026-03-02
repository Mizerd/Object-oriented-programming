#pragma once

#include "types.h"

#include <cstddef>
#include <iosfwd>
#include <vector>

struct Cols {
    std::size_t wV = 0;
    std::size_t wP = 0;
    std::size_t wG = 0;
};

void printTable(std::ostream& out,
                const std::vector<StudentRec>& students,
                std::size_t maxVCols,
                std::size_t maxPCols,
                std::size_t surnameExtraGapCols);
