#pragma once

#include <string>

struct Weights {
    double nd = 0.4;
    double egz = 0.6;
};

struct StudentRec {
    std::string vardas;
    std::string pavarde;
    double galVid = 0.0;
    double galMed = 0.0;
};
