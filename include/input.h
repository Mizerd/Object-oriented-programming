#pragma once

#include <string>

namespace inp {

// Nuskaito eilutę (naudojamas std::getline). Jei įvesties srautas uždaromas (EOF) – meta išimtį.
std::string readLinePrompted(const std::string& prompt);

// Tas pats, bet dar nukerpa tarpus iš abiejų pusių (ASCII tarpai).
std::string readLineTrimmedPrompted(const std::string& prompt);

// Saugus pasirinkimas iš intervalo, naudojant išimtis neteisingam formatui.
int readIntInRange(const std::string& prompt, int minVal, int maxVal);

// Saugus realaus skaičiaus nuskaitymas iš intervalo (priima ir kablelį, ir tašką).
double readDoubleInRange(const std::string& prompt, double minVal, double maxVal);

// Nuskaityti vieną raidę iš leistinų rinkinio (pvz. "tn").
// allowed turi būti mažosiomis; funkcija įvestį verčia į mažąsias (ASCII).
// Jei vartotojas paspaudžia ENTER ir defaultValue != '\0' – grąžinama defaultValue.
char readCharFromSet(const std::string& prompt, const std::string& allowed, char defaultValue = '\0');

// Įvedimas: "vardas pavardė". Grąžina false, jei įvesta 0 (baigti).
bool readNameSurname(std::string& vardas, std::string& pavarde);

} // namespace inp
