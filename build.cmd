@echo off

d:\msp430\bin\msp430-gcc.exe -Wall --std=c99 -mmcu=msp430g2553 -Os -o lcd.elf lcd.c
pause