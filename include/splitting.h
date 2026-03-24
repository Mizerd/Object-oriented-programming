#pragma once

#include "student_container.h"

bool isWeakStudent(const StudentRec& s);

enum class SplitStrategy { Strategy1 = 1, Strategy2 = 2, Strategy3 = 3 };

const char* splitStrategyLabel(SplitStrategy strategy);

void splitStudentsStrategy1(const StudentContainer& students,
                            StudentContainer& strong,
                            StudentContainer& weak);

void splitStudentsStrategy2(StudentContainer& students,
                            StudentContainer& weak);

void splitStudentsStrategy3(StudentContainer& students,
                            StudentContainer& weak);
