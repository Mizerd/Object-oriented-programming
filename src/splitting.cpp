#include "splitting.h"

#include <algorithm>

bool isWeakStudent(const StudentRec& s) {
    return s.galVid < 5.0;
}

const char* splitStrategyLabel(SplitStrategy strategy) {
    switch (strategy) {
        case SplitStrategy::Strategy1:
            return "1 strategija (du nauji konteineriai)";
        case SplitStrategy::Strategy2:
            return "2 strategija (vienas naujas konteineris + pasalinimas is bendro)";
        case SplitStrategy::Strategy3:
            return "3 strategija (stable_partition)";
    }
    return "nezinoma strategija";
}

void splitStudentsStrategy1(const StudentContainer& students,
                            StudentContainer& strong,
                            StudentContainer& weak) {
    strong.clear();
    weak.clear();

    reserveIfSupported(strong, students.size());
    reserveIfSupported(weak, students.size());

    for (const auto& s : students) {
        if (isWeakStudent(s)) {
            weak.push_back(s);
        } else {
            strong.push_back(s);
        }
    }
}

void splitStudentsStrategy2(StudentContainer& students,
                            StudentContainer& weak) {
    weak.clear();
    reserveIfSupported(weak, students.size());

    auto newEnd = std::remove_if(
        students.begin(),
        students.end(),
        [&](const StudentRec& s) {
            if (isWeakStudent(s)) {
                weak.push_back(s);
                return true;
            }
            return false;
        });

    students.erase(newEnd, students.end());
}

void splitStudentsStrategy3(StudentContainer& students,
                            StudentContainer& weak) {
    weak.clear();
    reserveIfSupported(weak, students.size());

    auto boundary = std::stable_partition(
        students.begin(),
        students.end(),
        [](const StudentRec& s) { return !isWeakStudent(s); });

    weak.insert(weak.end(), boundary, students.end());
    students.erase(boundary, students.end());
}
