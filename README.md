# Studentų galutinio balo skaičiuoklė (v.pradinė → v0.1)

Šiame projekte realizuota C++ programa, kuri leidžia įvesti arba sugeneruoti studentų duomenis, apskaičiuoja galutinį balą (pagal **vidurkį** arba **medianą**) ir išveda rezultatus į ekraną lentelės pavidalu (su **2 skaičiais po kablelio**).

Projektas paruoštas pagal VU pratybų užduoties **v0.1** reikalavimus (terminas: **2026-02-20**).

---

## Funkcionalumas

Programa leidžia:
- įvesti studentų **vardą** ir **pavardę** (`string`);
- įvesti studento **namų darbų pažymius** ir **egzamino pažymį** (`int`);
- skaičiuoti galutinį balą:
  - pagal **vidurkį**: `0.4 * ND_vidurkis + 0.6 * egzaminas`
  - pagal **medianą**: `0.4 * ND_mediana + 0.6 * egzaminas`
- dirbti, kai:
  - **studentų skaičius (m)** iš anksto nežinomas (įvedama iki kol vartotojas baigia),
  - **namų darbų skaičius (n)** iš anksto nežinomas (įvedama iki kol vartotojas pasako „stop“ / pasirenka baigti).
- generuoti atsitiktinius pažymius bei (pasirinktinai) studentų vardus ir pavardes (lietuviški rinkiniai).

---

## Failai

Šiame projekte yra **dvi atskiros realizacijos** (pagal užduotį):

1. `array.cpp`  
   - Duomenų rinkiniai (studentai ir pažymiai) saugomi naudojant **C masyvus** (be `std::vector`).

2. `vector.cpp`  
   - Visi duomenų rinkiniai saugomi naudojant **tik `std::vector`**.

> Pastaba: pagal reikalavimus masyvų versijoje neturi būti naudojami `std::vector`, o vektorių versijoje visur, kur reikia rinkinio, turi būti naudojamas `std::vector`.

---

## Programos meniu (privalomas)

Programa turi meniu su pasirinkimais:

1. **Įvesti ranka** (vardas, pavardė, ND, egzaminas)
2. **Generuoti tik pažymius** (vardas/pavardė įvedami, pažymiai generuojami)
3. **Generuoti vardus, pavardes ir pažymius** (viskas sugeneruojama automatiškai)
4. **Baigti darbą** ir išvesti rezultatus

> Rezultatai išvedami paspaudus **4** (baigti darbą), t. y. kai programa išeina iš meniu ciklo.
