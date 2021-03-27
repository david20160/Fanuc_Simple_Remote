@echo off
tcc RS-232\rs232.c src\main.c -o bin\FSR.exe -IIUP\include -IRS-232 -liup -Wl,-subsystem=windows