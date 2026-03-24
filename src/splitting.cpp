#include "splitting.h"

#include <algorithm>
#include <iterator>

bool isWeakStudent(const StudentRec& s) {
    return s.galVid < 5.0;
}

const char* splitStrategyLabel(SplitStrategy strategy) {
    switch (strategy) {
        case SplitStrategy::Strategy1:
            return "1 strategija (du nauji konteineriai)";
        case SplitStrategy::Strategy2:
            return "2 strategija (vienas naujas konteineris + remove_if)";
        case SplitStrategy::Strategy3:
            return "3 strategija (partition + move)";
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

    auto boundary = std::partition(
        students.begin(),
        students.end(),
        [](const StudentRec& s) {
            return !isWeakStudent(s);
        });

    weak.insert(
        weak.end(),
        std::make_move_iterator(boundary),
        std::make_move_iterator(students.end()));

    students.erase(boundary, students.end());
}
