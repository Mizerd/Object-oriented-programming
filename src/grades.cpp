#include "grades.h"

#include <algorithm>

namespace grade {

double calcFinal(double ndValue, int egz, const Weights& w) {
    return w.nd * ndValue + w.egz * static_cast<double>(egz);
}

double average(const std::vector<int>& v) {
    if (v.empty()) return 0.0;
    long long sum = 0;
    for (int x : v) sum += x;
    return static_cast<double>(sum) / static_cast<double>(v.size());
}

double medianFromSorted(const std::vector<int>& sorted) {
    if (sorted.empty()) return 0.0;
    const std::size_t n = sorted.size();
    if (n % 2 == 1) return static_cast<double>(sorted[n / 2]);
    return (static_cast<double>(sorted[n / 2 - 1]) + static_cast<double>(sorted[n / 2])) / 2.0;
}

double median(std::vector<int> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    return medianFromSorted(v);
}

} // namespace grade
