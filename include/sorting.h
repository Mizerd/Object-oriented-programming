#pragma once

#include "types.h"

#include <vector>

enum class SortKey { ByName = 1, BySurname = 2, ByFinalAvg = 3, ByFinalMed = 4 };

void sortStudents(std::vector<StudentRec>& students, SortKey key);
