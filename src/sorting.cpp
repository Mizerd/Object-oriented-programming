#include "sorting.h"

#include <algorithm>
#include <deque>
#include <list>
#include <vector>

namespace {

template <typename Compare>
void sortImpl(std::vector<StudentRec>& students, Compare comp) {
    std::sort(students.begin(), students.end(), comp);
}

template <typename Compare>
void sortImpl(std::deque<StudentRec>& students, Compare comp) {
    std::sort(students.begin(), students.end(), comp);
}

template <typename Compare>
void sortImpl(std::list<StudentRec>& students, Compare comp) {
    students.sort(comp);
}

} // namespace

const char* sortKeyLabel(SortKey key) {
    switch (key) {
        case SortKey::ByName:     return "vardas";
        case SortKey::BySurname:  return "pavardė";
        case SortKey::ByFinalAvg: return "galutinis_vidurkis";
        case SortKey::ByFinalMed: return "galutinė_mediana";
    }
    return "nežinomas";
}

void sortStudents(StudentContainer& students, SortKey key) {
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
        case SortKey::ByName:     sortImpl(students, byName);     break;
        case SortKey::BySurname:  sortImpl(students, bySurname);  break;
        case SortKey::ByFinalAvg: sortImpl(students, byFinalAvg); break;
        case SortKey::ByFinalMed: sortImpl(students, byFinalMed); break;
    }
}
