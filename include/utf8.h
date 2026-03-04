#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

namespace utf8 {

void initUtf8Locale();

// Terminal/display width of a UTF-8 string (POSIX: mbsrtowcs + wcwidth).
// If conversion fails, falls back to byte length.
std::size_t displayWidth(const std::string& s);

void printPaddedRight(std::ostream& out, const std::string& text, std::size_t widthCols);
void printPaddedLeft(std::ostream& out, const std::string& text, std::size_t widthCols);

} // namespace utf8
