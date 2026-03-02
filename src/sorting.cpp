#include "sorting.h"

#include <algorithm>

void sortStudents(std::vector<StudentRec>& students, SortKey key) {
    auto byName = [](const StudentRec& a, const StudentRec& b) {
        if (a.vardas != b.vardas) return a.vardas < b.vardas;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.galVid < b.galVid;
    };

    auto bySurname = [](const StudentRec& a, const StudentRec& b) {
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        if (a.vardas != b.vardas) return a.vardas < b.vardas;
        return a.galVid < b.galVid;
    };

    auto byFinalAvg = [](const StudentRec& a, const StudentRec& b) {
        if (a.galVid != b.galVid) return a.galVid < b.galVid;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.vardas < b.vardas;
    };

    auto byFinalMed = [](const StudentRec& a, const StudentRec& b) {
        if (a.galMed != b.galMed) return a.galMed < b.galMed;
        if (a.pavarde != b.pavarde) return a.pavarde < b.pavarde;
        return a.vardas < b.vardas;
    };

    switch (key) {
        case SortKey::ByName:     std::sort(students.begin(), students.end(), byName);     break;
        case SortKey::BySurname:  std::sort(students.begin(), students.end(), bySurname);  break;
        case SortKey::ByFinalAvg: std::sort(students.begin(), students.end(), byFinalAvg); break;
        case SortKey::ByFinalMed: std::sort(students.begin(), students.end(), byFinalMed); break;
    }
}
