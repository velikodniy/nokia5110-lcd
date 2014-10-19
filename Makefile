all: all-msp430

all-msp430:
	msp430-gcc -Wall --std=c99 -mmcu=msp430g2553 -Os -o lcd.elf lcd.c

prog:
	mspdebug rf2500 "prog lcd.elf" exit

clean:
	rm lcd.elf

.PHONY: all all-msp430
