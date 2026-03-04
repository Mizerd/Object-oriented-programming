#pragma once

#include "types.h"

#include <vector>

namespace grade {

double calcFinal(double ndValue, int egz, const Weights& w);

double average(const std::vector<int>& v);

double median(std::vector<int> v);

double medianFromSorted(const std::vector<int>& sorted);

} // namespace grade
