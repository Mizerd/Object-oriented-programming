#pragma once

#include "student_container.h"

#include <string>

void readStudentsFromFile(const std::string& filename,
                          StudentContainer& out,
                          const Weights& weights);
