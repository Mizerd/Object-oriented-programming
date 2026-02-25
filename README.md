# v0.2 – Studentų galutinio balo skaičiuoklė (std::vector, C++17, Linux)

Ši programa skirta studentų pažymių apdorojimui ir galutinio balo skaičiavimui dviem būdais:
- Galutinis (Vid.) – pagal namų darbų vidurkį
- Galutinis (Med.) – pagal namų darbų medianą

Programa turi 2 darbo režimus:
1. Nuskaityti studentus iš failo (v0.2 funkcionalumas)
2. Įvesti / generuoti interaktyviai (v0.1 meniu)

------------------------------------------------------------

## Galutinio balo formulė

Naudojami svoriai:

constexpr double kNdWeight = 0.4;
constexpr double kEgzWeight = 0.6;

Galutinis balas skaičiuojamas:

Galutinis = 0.4 * ND_reikšmė + 0.6 * Egz

Kur ND_reikšmė gali būti:
- namų darbų vidurkis → Galutinis (Vid.)
- namų darbų mediana → Galutinis (Med.)

------------------------------------------------------------

## Pagrindinė duomenų struktūra

v0.2 versijoje naudojama struktūra:

struct StudentRec {
    std::string vardas;
    std::string pavarde;
    double galVid;
    double galMed;
};

Skaičiavimai atliekami nuskaitymo metu, todėl saugomi tik galutiniai rezultatai.
Tai sumažina atminties naudojimą dirbant su dideliais failais.

------------------------------------------------------------

## Failo režimas (v0.2)

Pasirinkus režimą „1 - Nuskaityti studentus iš failo“:

1. Įvedamas failo pavadinimas (pvz. studentai1000000.txt)
2. Pasirenkamas rikiavimo kriterijus:
   - pagal vardą
   - pagal pavardę
   - pagal Galutinį vidurkį
   - pagal Galutinę medianą
3. Pasirenkamas išvedimo tipas:
   - į ekraną
   - į failą

### Failo formatas

Failas turi turėti antraštę:

Vardas Pavarde ND1 ND2 ... Egz

Toliau kiekvienoje eilutėje:

Vardas Pavarde ND1 ND2 ... NDk Egz

Namų darbų kiekis nustatomas automatiškai iš antraštės.

------------------------------------------------------------

## Interaktyvus režimas (v0.1)

Pasirinkus režimą „2 - Įvesti / generuoti“:

Galima:
1. Įvesti studentą ranka (vardas, pavardė, ND, egzaminas)
2. Generuoti pažymius
3. Generuoti vardus, pavardes ir pažymius
4. Baigti darbą

Maksimalūs limitai:
- iki 10 000 studentų
- iki 1 000 namų darbų vienam studentui

------------------------------------------------------------

## Skaičiavimų logika

### Vidurkis
Visi ND susumuojami ir padalinami iš jų kiekio.
Jei ND nėra – laikoma 0.

### Mediana
ND vektorius surūšiuojamas.
- Jei kiekis nelyginis – imamas vidurinis elementas
- Jei lyginis – dviejų vidurinių elementų vidurkis

### Galutinio balo skaičiavimas

double calcFinal(double ndValue, int egz) {
    return 0.4 * ndValue + 0.6 * egz;
}

------------------------------------------------------------

## Rikiavimas

Naudojamas std::sort su lambda funkcijomis.

Galima rikiuoti pagal:
- vardą
- pavardę
- galutinį vidurkį
- galutinę medianą

------------------------------------------------------------

## UTF-8 ir lietuviškos raidės

Programa palaiko UTF-8.
Naudojama:
- setlocale()
- mbsrtowcs()
- wcwidth()

Tai leidžia teisingai išlyginti lietuviškas raides lentelėje.

Jei sistema neturi UTF-8 locale, programa pateikia įspėjimą.

------------------------------------------------------------

## Kompiliavimas (Linux)

Kompiliuoti taip:

g++ -std=c++17 -O2 -g vector.cpp -o vector

Paleisti:

./vector

------------------------------------------------------------

## Testavimo aplinka

Sistema: Fedora Linux  
Standartas: C++17  
Kompiliatorius: g++  

------------------------------------------------------------

## Vykdymo laiko matavimas

Laikas matuotas naudojant:

/usr/bin/time -p ./vector

Kiekvienas testas buvo atliktas 5 kartus ir apskaičiuotas vidurkis.

### studentai1000000.txt
real: 19.51 s  
user: 1.47 s  
sys: 0.15 s  

### studentai100000.txt
real: 17.96 s  
user: 0.20 s  
sys: 0.03 s  

### studentai10000.txt
real: 12.78 s  
user: 0.01 s  
sys: 0.00 s  

### Laikų paaiškinimas

real – bendras vykdymo laikas (įskaitant failų skaitymą ir rašymą)  
user – CPU laikas, skirtas programos skaičiavimams  
sys – operacinės sistemos laikas (failai, sisteminiai kvietimai)

Didžioji dalis „real“ laiko skiriama rezultatų įrašymui į failą.

------------------------------------------------------------

## Išvados

- Programa efektyviai apdoroja didelius duomenų kiekius naudojant std::vector.
- Skaičiavimų dalis (user laikas) yra labai greita.
- Didžiausias laiko sąnaudų šaltinis – failų įrašymas (I/O operacijos).
- v0.2 versija pritaikyta darbui su dideliais duomenų failais ir teisingu UTF-8 atvaizdavimu.
