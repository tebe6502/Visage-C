Visage C v1.0 (Tebe/Madteam)
----------------------------

Wersja przegladarki interlacowych obrazkow w jezyku C. Jesli ktos nie wiedzial jak "ugryzc"
HIP, INP lub TIP to teraz bedzie mu latwiej.

RIP'ow nie dodalem jeszcze ze wzgledu na ich duza liczbe kombinacji i znacznej wielkosci
procedury dekompresji w asm, no ale jesli bedzie zainteresowanie programem to czemu nie :)

Uzytkowanie programu jest troche inne niz w przypadku Visage_2.7, musimy wpisac nazwe pliku
z grafika (HIP, INP lub TIP). Mozna uzywac malych liter, jak i duzych.

- zrodlo programu znajduje sie w VISAGE.C
- przerwania DLI dla HIP, TIP w pliku VIEW.ASM
- VISC6502.COM  wykonywalny, scalony program dla CPU6502
- VISC816.COM   wykonywalny, scalony program Atari dla CPU65816

aby scalic program w VISC.COM nalezy polaczyc (np. SuperPacker'em Bewesoft'a):

VISAGE.EXE	(skompilowany plik VISAGE.C)
VIEW.OBJ	(skompilowany przez X-asm plik VIEW.ASM)
RUN 2E01	(adres startu dla domyslnych ustawien linkera cc65)

domyslnie VIEW.OBJ znajduje sie pod adresem $600, aby wywolac przerwanie DLI dla HIP
wykonujemy "jsr $600", dla TIP'a "jsr $603"

takie "sztywne" wywolania sa umieszczone w VISAGE.C, wiec jesli ktos chce zmienic adresy
dla VIEW.ASM musi poprawic tez plik VISAGE.C
