# SimpleShell

## Funkcje

* Wykonywanie komend systemowych.

* Wbudowane komendy (built-ins): `cd`, `exit`.

* Obsługa potoków (operator `|`).

* Obsługa procesów w tle (operator `&`).

* Obsługa sygnału przerwania (SIGINT - Ctrl+C).


## Stan projektu

Obecnie w projekcie:

* Brak obsługi przekierowań wejścia/wyjścia (>, <, >>).

* Brak wbudowanych funkcji zarządzania procesami w tle (np. polecenia `jobs`, `fg`, `kill`).


## Kompilacja i uruchomienie
```bash
make
./myshell
