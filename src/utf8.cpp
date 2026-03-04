#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include "utf8.h"

#include <clocale>
#include <cwchar>
#include <iostream>
#include <wchar.h>

namespace utf8 {

static bool conversionWorks() {
    const char* sample = u8"Ąčęėįšųūž";
    std::mbstate_t st{};
    const char* src = sample;
    const std::size_t n = std::mbsrtowcs(nullptr, &src, 0, &st);
    return n != static_cast<std::size_t>(-1);
}

void initUtf8Locale() {
    (void)std::setlocale(LC_ALL, "");
    if (conversionWorks()) return;

    for (const char* candidate : {"C.UTF-8", "C.utf8", "lt_LT.UTF-8", "en_US.UTF-8"}) {
        if (std::setlocale(LC_ALL, candidate) && conversionWorks()) return;
    }

    std::cerr
        << "[ISPEJIMAS] UTF-8 locale neaktyvus; lietuvisku raidziu stulpeliu islygiavimas gali buti netikslus.\n"
        << "           Pabandykite: export LANG=C.UTF-8; export LC_ALL=C.UTF-8\n";
}

std::size_t displayWidth(const std::string& s) {
    if (s.empty()) return 0;

    std::mbstate_t st{};
    const char* src = s.c_str();

    const std::size_t needed = std::mbsrtowcs(nullptr, &src, 0, &st);
    if (needed == static_cast<std::size_t>(-1)) {
        return s.size();
    }

    std::wstring w;
    w.resize(needed);

    st = std::mbstate_t{};
    src = s.c_str();
    const std::size_t converted = std::mbsrtowcs(w.data(), &src, w.size(), &st);
    if (converted == static_cast<std::size_t>(-1)) return s.size();

    std::size_t cols = 0;
    for (wchar_t wc : w) {
        const int cw = ::wcwidth(wc);
        if (cw >= 0) cols += static_cast<std::size_t>(cw);
        else cols += 1;
    }
    return cols;
}

void printPaddedRight(std::ostream& out, const std::string& text, std::size_t widthCols) {
    out << text;
    const std::size_t used = displayWidth(text);
    if (used < widthCols) out << std::string(widthCols - used, ' ');
}

void printPaddedLeft(std::ostream& out, const std::string& text, std::size_t widthCols) {
    const std::size_t used = displayWidth(text);
    if (used < widthCols) out << std::string(widthCols - used, ' ');
    out << text;
}

} // namespace utf8
