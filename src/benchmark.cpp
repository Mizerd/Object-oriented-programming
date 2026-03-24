#include "benchmark.h"

#include "fileio.h"
#include "student_container.h"
#include "timer.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {

BenchmarkRow measureOne(const std::string& filename,
                        const Weights& weights,
                        SortKey key,
                        SplitStrategy strategy) {
    BenchmarkRow row;
    row.filename = filename;
    row.sortKey = key;
    row.strategy = strategy;

    StudentContainer students;

    const auto readStart = timer::now();
    readStudentsFromFile(filename, students, weights);
    const auto readEnd = timer::now();
    row.readSecs = timer::seconds(readStart, readEnd);
    row.total = students.size();

    const auto sortStart = timer::now();
    sortStudents(students, key);
    const auto sortEnd = timer::now();
    row.sortSecs = timer::seconds(sortStart, sortEnd);

    StudentContainer strong;
    StudentContainer weak;

    switch (strategy) {
        case SplitStrategy::Strategy1: {
            const auto splitStart = timer::now();
            splitStudentsStrategy1(students, strong, weak);
            const auto splitEnd = timer::now();
            row.splitSecs = timer::seconds(splitStart, splitEnd);
            break;
        }
        case SplitStrategy::Strategy2: {
            StudentContainer working = students;
            const auto splitStart = timer::now();
            splitStudentsStrategy2(working, weak);
            const auto splitEnd = timer::now();
            row.splitSecs = timer::seconds(splitStart, splitEnd);
            strong = std::move(working);
            break;
        }
        case SplitStrategy::Strategy3: {
            StudentContainer working = students;
            const auto splitStart = timer::now();
            splitStudentsStrategy3(working, weak);
            const auto splitEnd = timer::now();
            row.splitSecs = timer::seconds(splitStart, splitEnd);
            strong = std::move(working);
            break;
        }
    }

    row.strong = strong.size();
    row.weak = weak.size();
    return row;
}

bool fileExistsAndNonEmpty(const std::string& path) {
    std::error_code ec;
    return std::filesystem::exists(path, ec) && std::filesystem::file_size(path, ec) > 0;
}

} // namespace

BenchmarkRow benchmarkOnce(const std::string& filename,
                           const Weights& weights,
                           SortKey key,
                           SplitStrategy strategy) {
    return measureOne(filename, weights, key, strategy);
}

BenchmarkRow benchmarkAverage(const std::string& filename,
                              const Weights& weights,
                              SortKey key,
                              SplitStrategy strategy,
                              int repeats) {
    if (repeats <= 0) {
        throw std::runtime_error("Pakartojimų skaičius turi būti teigiamas.");
    }

    BenchmarkRow avg;
    avg.filename = filename;
    avg.sortKey = key;
    avg.strategy = strategy;

    for (int i = 0; i < repeats; ++i) {
        const BenchmarkRow row = measureOne(filename, weights, key, strategy);
        avg.total = row.total;
        avg.strong = row.strong;
        avg.weak = row.weak;
        avg.readSecs += row.readSecs;
        avg.sortSecs += row.sortSecs;
        avg.splitSecs += row.splitSecs;
    }

    avg.readSecs /= static_cast<double>(repeats);
    avg.sortSecs /= static_cast<double>(repeats);
    avg.splitSecs /= static_cast<double>(repeats);

    return avg;
}

void printBenchmarkRow(const BenchmarkRow& row) {
    std::cout << "\nFailas: " << row.filename << "\n"
              << "Konteineris: " << kStudentContainerName << "\n"
              << "Rikiavimo raktas: " << sortKeyLabel(row.sortKey) << "\n"
              << "Strategija: " << splitStrategyLabel(row.strategy) << "\n"
              << "Studentų kiekis: " << row.total << "\n"
              << "Kietiakai: " << row.strong << "\n"
              << "Vargšiukai: " << row.weak << "\n";
    timer::print("Failo nuskaitymo vidurkis", row.readSecs);
    timer::print("Rikiavimo vidurkis", row.sortSecs);
    timer::print("Skirstymo vidurkis", row.splitSecs);
}

void appendBenchmarkCsv(const std::string& csvFilename,
                        const BenchmarkRow& row,
                        int repeats) {
    const bool needHeader = !fileExistsAndNonEmpty(csvFilename);

    std::ofstream out(csvFilename, std::ios::app);
    if (!out) {
        throw std::runtime_error("Nepavyko atidaryti CSV failo: " + csvFilename);
    }

    if (needHeader) {
        out << "container,filename,repeats,sort_key,strategy,total,strong,weak,read_s,sort_s,split_s\n";
    }

    out << kStudentContainerName << ','
        << row.filename << ','
        << repeats << ','
        << sortKeyLabel(row.sortKey) << ','
        << splitStrategyLabel(row.strategy) << ','
        << row.total << ','
        << row.strong << ','
        << row.weak << ','
        << std::fixed << std::setprecision(6)
        << row.readSecs << ','
        << row.sortSecs << ','
        << row.splitSecs << '\n';
}
