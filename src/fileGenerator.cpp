#include "fileGenerator.h"

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
    std::cout << "---------------------------------------------\n";

    for (int n : sizes) {
        const std::string filename = "studentai" + std::to_string(n) + ".txt";
        std::ofstream out(filename);

        if (!out) {
            std::cerr << "Nepavyko sukurti failo: " << filename << "\n";
            continue;
        }

        out << std::left
        << std::setw(20) << "Vardas"
        << std::setw(20) << "Pavarde";

        for (int i = 1; i <= ndCount; ++i) {
            out << std::setw(6) << ("ND" + std::to_string(i));
        }

        out << std::setw(6) << "Egz" << "\n";

        for (int i = 1; i <= n; ++i) {
            out << std::left
            << std::setw(20) << ("Vardas" + std::to_string(i))
            << std::setw(20) << ("Pavarde" + std::to_string(i));

            for (int j = 0; j < ndCount; ++j) {
                out << std::setw(6) << grade(gen);
            }

            out << std::setw(6) << grade(gen) << "\n";
        }

        std::cout << std::left
        << "Sukurtas failas: "
        << std::setw(22) << filename
        << " | irasu: " << n << "\n";
    }

    std::cout << "---------------------------------------------\n";
}
