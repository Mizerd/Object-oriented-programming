#pragma once

namespace cfg {
inline constexpr double kDefaultNdWeight = 0.4;
inline constexpr double kDefaultEgzWeight = 0.6;

inline constexpr int kMaxStudentsGenerate = 10000000;  // allow up to 10M
inline constexpr int kMaxNdPerStudent     = 1000;
}
