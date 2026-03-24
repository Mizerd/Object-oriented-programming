#pragma once

#include "sorting.h"
#include "splitting.h"

#include <cstddef>
#include <string>

struct BenchmarkRow {
    std::string filename;
    SortKey sortKey = SortKey::ByFinalAvg;
    SplitStrategy strategy = SplitStrategy::Strategy1;
    std::size_t total = 0;
    std::size_t strong = 0;
    std::size_t weak = 0;
    double readSecs = 0.0;
    double sortSecs = 0.0;
    double splitSecs = 0.0;
};

BenchmarkRow benchmarkOnce(const std::string& filename,
                           const Weights& weights,
                           SortKey key,
                           SplitStrategy strategy);

BenchmarkRow benchmarkAverage(const std::string& filename,
                              const Weights& weights,
                              SortKey key,
                              SplitStrategy strategy,
                              int repeats);

void printBenchmarkRow(const BenchmarkRow& row);
void appendBenchmarkCsv(const std::string& csvFilename,
                        const BenchmarkRow& row,
                        int repeats);
