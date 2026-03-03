#pragma once

#include "types.h"

#include <cstddef>
#include <string>
#include <vector>

// Nuskaitymas iš failo (v0.2 funkcionalumas). Meta std::runtime_error, jei:
//  - failas neegzistuoja / neatsidaro
//  - neteisinga antraštė ar formatas
//  - trūksta pažymių
void readStudentsFromFileVec(const std::string& filename,
                             std::vector<StudentRec>& out,
                             std::size_t& maxVCols,
                             std::size_t& maxPCols,
                             const Weights& weights);
