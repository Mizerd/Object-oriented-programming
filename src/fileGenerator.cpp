#include "fileGenerator.h"
#include "timer.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

void generateTestFiles(int ndCount) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> grade(1, 10);

    const std::vector<int> sizes = {1000, 10000, 100000, 1000000, 10000000};

    std::cout << "\nGeneruojami testiniai failai\n";
    std::cout << "ND skaicius kiekvienam studentui: " << ndCount << "\n";
    std::cout << "------------------------------------------------------------\n";

    const auto totalStart = timer::now();

    for (int n : sizes) {
        const auto start = timer::now();

        const std::string filename = "studentai" + std::to_string(n) + ".txt";
        std::ofstream out(filename);

        if (!out) {
            std::cerr << "Nepavyko sukūrti failo: " << filename << "\n";
            continue;
        }

        out << std::left
        << std::setw(20) << "Vardas"
        << std::setw(20) << "Pavardė";

        for (int i = 1; i <= ndCount; ++i) {
            out << std::setw(6) << ("ND" + std::to_string(i));
        }

        out << std::setw(6) << "Egz" << "\n";

        for (int i = 1; i <= n; ++i) {
            out << std::left
            << std::setw(20) << ("Vardas" + std::to_string(i))
            << std::setw(20) << ("Pavardė" + std::to_string(i));

            for (int j = 0; j < ndCount; ++j) {
                out << std::setw(6) << grade(gen);
            }

            out << std::setw(6) << grade(gen) << "\n";
        }

        out.close();

        const auto end = timer::now();
        const double secs = timer::seconds(start, end);

        std::cout << std::left
        << "Sukūrtas failas: "
        << std::setw(22) << filename
        << " | įrašų: " << std::setw(10) << n
        << " | laikas: " << std::fixed << std::setprecision(6)
        << secs << " s\n";
    }

    const auto totalEnd = timer::now();
    std::cout << "------------------------------------------------------------\n";
    timer::print("Bendras failų generavimo laikas", timer::seconds(totalStart, totalEnd));
    std::cout << "------------------------------------------------------------\n";
}
