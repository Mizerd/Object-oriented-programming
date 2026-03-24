#pragma once

#include "student_container.h"

enum class SortKey { ByName = 1, BySurname = 2, ByFinalAvg = 3, ByFinalMed = 4 };

void sortStudents(StudentContainer& students, SortKey key);
const char* sortKeyLabel(SortKey key);
