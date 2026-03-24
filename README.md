# Studentų galutinio balo skaičiuoklė – v1.0

## Trumpai apie projektą

Tai yra v1.0 projekto versija, kuri pratęsia v0.4 realizaciją ir pritaiko ją konteinerių tyrimui su trimis STL konteineriais:

- `std::vector`
- `std::list`
- `std::deque`

Programa leidžia:
- nuskaityti studentų duomenis iš failo,
- apskaičiuoti galutinį balą pagal vidurkį arba medianą,
- surūšiuoti studentus,
- padalinti juos į `kietiakai` ir `vargšiukai`,
- atlikti benchmark tyrimą, matuojant tik `read + sort + split`.

## Kas įgyvendinta v1.0

- vienas projekto kodas kompiliuojamas su trimis konteinerių tipais;
- realizuotos 3 skirstymo strategijos;
- benchmark režimas matuoja tik užduotyje reikalaujamas fazes;
- benchmark rezultatai eksportuojami į CSV failus;
- palaikomas darbas su dideliais failais, įskaitant 10 000 000 įrašų.

## Projekto struktūra

```text
.
├── include/
│   ├── benchmark.h
│   ├── constants.h
│   ├── fileGenerator.h
│   ├── fileio.h
│   ├── grades.h
│   ├── input.h
│   ├── menu.h
│   ├── sorting.h
│   ├── splitting.h
│   ├── student_container.h
│   ├── table.h
│   ├── timer.h
│   ├── types.h
│   └── utf8.h
├── src/
│   ├── benchmark.cpp
│   ├── fileGenerator.cpp
│   ├── fileio.cpp
│   ├── grades.cpp
│   ├── input.cpp
│   ├── main.cpp
│   ├── menu.cpp
│   ├── sorting.cpp
│   ├── splitting.cpp
│   ├── table.cpp
│   └── utf8.cpp
├── Makefile
├── benchmark_vector.csv
├── benchmark_list.csv
├── benchmark_deque.csv
└── README.md
```

## Kompiliavimas

```bash
make
```

Sugeneruojami 3 vykdomieji failai:

```bash
./vector
./list
./deque
```

Išvalymas:

```bash
make clean
```

## Paleidimas

### Įprastas režimas

Paleidus vieną iš vykdomųjų failų galima:
- nuskaityti studentus iš failo,
- įvesti duomenis ranka / generuoti per v0.1 meniu,
- generuoti testinius failus,
- išsaugoti `kietiakai.txt` ir `vargsiukai.txt`.

### Benchmark režimas

Meniu punktas:

```text
4 - Benchmark / tyrimas (tik read + sort + split)
```

Šis režimas nerašo rezultatų į `kietiakai.txt` / `vargsiukai.txt`, o matuoja tik:
- duomenų nuskaitymą,
- bendro konteinerio rūšiavimą,
- studentų skirstymą į dvi grupes.

## Testavimo duomenys

Benchmark testai atlikti su anksčiau sugeneruotais failais, kad visi konteineriai ir strategijos būtų lyginami vienodomis sąlygomis.

Naudotų failų rinkinys:
- `studentai1000.txt`
- `studentai10000.txt`
- `studentai100000.txt`
- `studentai1000000.txt`
- `studentai10000000.txt`

Šių failų atsisiuntimo nuoroda:
- https://drive.proton.me/urls/NGTEYQY8FR#gHv5pii4BrHt

## Skirstymo strategijos

### 1 strategija
Bendras studentų konteineris paliekamas nepakeistas, o duomenys kopijuojami į du naujus konteinerius:
- `kietiakai`
- `vargšiukai`

**Pliusas:** paprasta ir aiški realizacija.  
**Minusas:** didelės papildomos atminties sąnaudos.

### 2 strategija
Naudojamas vienas naujas konteineris `vargšiukai`, o silpnesni studentai pašalinami iš bendro konteinerio.

**Pliusas:** mažesnės papildomos atminties sąnaudos.  
**Minusas:** šalinimo logika yra jautri konteinerio tipui.

### 3 strategija
Trečia strategija yra optimizuota 2 strategijos versija, paremta tuo pačiu principu, bet naudojanti efektyvesnius STL veiksmus.

**Tikslas:** sumažinti skirstymo kainą ir išlaikyti tą pačią bendrą logiką visiems konteineriams.

## Testavimo sistema

- **OS:** Fedora Linux 43, KDE Plasma Desktop Edition
- **CPU:** Intel Core Ultra 5 228V
- **RAM:** 32 GiB RAM (30.7 GiB usable)
- **Diskas:** NVMe SSD
- **Grafika:** Intel Graphics (integrated)

![Sistemos informacija](c0e7a4ba-2cc3-415f-ab12-bd8cc3b9944c.png)

## Benchmark metodika

- Matuotas rikiavimas pagal `galutinis_vidurkis`
- Kiekvienas testas kartotas **20 kartų**
- Rezultatuose pateikiamas **vidurkis**
- Matuotos tik šios fazės:
  - `read`
  - `sort`
  - `split`


- [benchmark_vector.csv](benchmark_vector.csv)
- [benchmark_list.csv](benchmark_list.csv)
- [benchmark_deque.csv](benchmark_deque.csv)

## Strategijų palyginimas

### `std::vector`

| Failas | 1 strategija | 2 strategija | 3 strategija |
|---|---:|---:|---:|
| 10 000 000 | 9.186616 s | 9.063602 s | 9.571146 s |

**Greičiausia strategija:** **2 strategija**

### `std::list`

| Failas | 1 strategija | 2 strategija | 3 strategija |
|---|---:|---:|---:|
| 10 000 000 | 16.480180 s | 15.361767 s | 15.947850 s |

**Greičiausia strategija:** **2 strategija**

### `std::deque`

| Failas | 1 strategija | 2 strategija | 3 strategija |
|---|---:|---:|---:|
| 10 000 000 | 9.154252 s | 8.897489 s | 9.259165 s |

**Greičiausia strategija:** **2 strategija**

## Konteinerių palyginimas

Žemiau lyginami konteineriai naudojant kiekvieno konteinerio greičiausią strategiją.

- `vector` → **2 strategija**
- `list` → **2 strategija**
- `deque` → **2 strategija**

| Failas | Vector total | List total | Deque total | Vector read | List read | Deque read | Vector sort | List sort | Deque sort | Vector split | List split | Deque split |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 10 000 000 | 9.063602 s | 15.361767 s | 8.897489 s | 5.893578 s | 6.473022 s | 5.288783 s | 2.995026 s | 8.739139 s | 3.408395 s | 0.174998 s | 0.149606 s | 0.200311 s |


## Rezultatų aptarimas

Iš pateiktų benchmark rezultatų matyti, kad:
- **`std::vector`** išlieka labai stiprus bendro našumo požiūriu dėl greito nuskaitymo ir rūšiavimo.
- **`std::list`** skirstymo fazėje gali būti labai greitas, tačiau bendrą laiką stipriai padidina rūšiavimo kaštas.
- **`std::deque`** dažniausiai yra tarpinis variantas tarp `vector` ir `list`.
- 1 strategija dažniausiai yra lėčiausia dėl duomenų dubliavimo į du naujus konteinerius.
- 2 ir 3 strategijos naudoja mažiau papildomos atminties ir dažniausiai veikia greičiau.
- Šiame benchmark rinkinyje 3 strategija kai kuriais atvejais pagerina 2 strategijos rezultatą, tačiau bendras laimėtojas priklauso nuo konkretaus konteinerio ir duomenų kiekio.

## Ekrano nuotraukos

Programos paleidimas:
![Programos paleidimas](screenshots/V1.0_startup.png)

`std::vector` benchmark:
![Vector benchmark](screenshots/V1.0_benchmark_vector.png)
![Vector benchmark overview](screenshots/V1.0_benchmark_vector_over.png)

`std::list` benchmark:
![List benchmark](screenshots/V1.0_benchmark_list.png)
![List benchmark overview](screenshots/V1.0_benchmark_list_over.png)

`std::deque` benchmark:
![Deque benchmark](screenshots/V1.0_benchmark_deque.png)
![Deque benchmark overview](screenshots/v1.0_benchmark_deque_over.png)

## v0.4 ir v1.0 skirtumai

### v0.4
- viena realizacija su `std::vector`
- duomenų skaitymas, skirstymas, rūšiavimas ir išvedimas į failus
- matuojamas bendras programos veikimo laikas

### v1.0
- tas pats kodas kompiliuojamas su `std::vector`, `std::list` ir `std::deque`
- pridėtas atskiras benchmark režimas pagal užduoties reikalavimus
- realizuotos 3 skirstymo strategijos
- rezultatai eksportuojami į CSV, todėl juos lengva pridėti į GitHub ir panaudoti README analizėje

## Naudojimo instrukcija

1. Sukompiliuoti projektą su `make`.
2. Paleisti vieną iš vykdomųjų failų: `./vector`, `./list` arba `./deque`.
3. Jei reikia, sugeneruoti testinius failus.
4. Paleisti benchmark režimą.
5. Surinkti CSV rezultatus.
6. Įkelti README, CSV ir ekrano nuotraukas į GitHub repozitoriją.
