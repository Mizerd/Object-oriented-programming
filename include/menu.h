#pragma once

#include "types.h"

#include <cstddef>
#include <vector>

// Interaktyvus režimas (v0.1 meniu logika) su std::vector.
void menuV01Style(std::vector<StudentRec>& students,
                  std::size_t& maxVCols,
                  std::size_t& maxPCols,
                  const Weights& weights);
