#pragma once

#include "types.h"

#include <cstddef>
#include <deque>
#include <list>
#include <string_view>
#include <vector>

#if defined(STUDENT_CONTAINER_LIST)
using StudentContainer = std::list<StudentRec>;
inline constexpr std::string_view kStudentContainerName = "std::list";
#elif defined(STUDENT_CONTAINER_DEQUE)
using StudentContainer = std::deque<StudentRec>;
inline constexpr std::string_view kStudentContainerName = "std::deque";
#else
using StudentContainer = std::vector<StudentRec>;
inline constexpr std::string_view kStudentContainerName = "std::vector";
#endif

template <typename T, typename Alloc>
inline void reserveIfSupported(std::vector<T, Alloc>& c, std::size_t n) {
    c.reserve(n);
}

template <typename Container>
inline void reserveIfSupported(Container&, std::size_t) {
}
