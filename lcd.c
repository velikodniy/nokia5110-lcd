#include <stdint.h>

#include <msp430.h>
#include "font.h"

#include "lcd.h"

#define LOW		0
#define HIGH	1

// Library settings

#define POUT		P1OUT
#define PDIR		P1DIR

#define PIN_SCE   BIT0
#define PIN_RESET BIT1
#define PIN_DC    BIT2
#define PIN_SDIN  BIT3
#define PIN_SCLK  BIT4

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48

// -----------------

#define OUT_HIGH(pin)		POUT |=  (pin)
#define OUT_LOW(pin)		POUT &= ~(pin)
#define OUT_BIT(pin, bit)	if((bit)==0) OUT_LOW((pin)); else OUT_HIGH((pin));


void LcdInit(void) {
	// Set all pins as output
	PDIR |= 
		PIN_SCE		|
		PIN_RESET	|
		PIN_DC		|
		PIN_SDIN	|
		PIN_SCLK;

	OUT_LOW(PIN_RESET);
	OUT_HIGH(PIN_RESET);
  
	LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
	LcdWrite(LCD_C, 0xB1 );  // Set LCD Vop (Contrast). 
	LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
	LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
	LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
	LcdWrite(LCD_C, 0x20 );
	LcdWrite(LCD_C, 0x0C );
}

void LcdWrite(uint8_t dc, uint8_t data) {
	OUT_BIT(PIN_DC, dc);
	OUT_LOW(PIN_SCE);
	for (uint8_t i = 0; i < 8; i++)  {
		OUT_BIT(PIN_SDIN, !!(data & (1 << (7 - i))));
		OUT_HIGH(PIN_SCLK);
		OUT_LOW(PIN_SCLK);		
	}
	OUT_HIGH(PIN_SCE);
}


void LcdCharacter(char character) {
	uint8_t n;
	LcdWrite(LCD_D, 0x00);	// Whitespace before
	for (uint8_t i = 0; i < 5; i++)
	{	
		n = character - 0x20;
		if(character < 0)
			n += 0x100;
		LcdWrite(LCD_D, ASCII[n][i]);
	}
	LcdWrite(LCD_D, 0x00);	// Whitespace after
}

void LcdClear(void) {
	for (uint16_t i = 0; i < LCD_X * LCD_Y / 8; i++)
		LcdWrite(LCD_D, 0x00);
	LcdWrite(LCD_C, 64);
	LcdWrite(LCD_C, 128);
}

void LcdString(char *characters) {
	while (*characters)
		LcdCharacter(*characters++);
}


int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;
	
	LcdInit();

	
    /* Configure ADC Temp Sensor Channel */
    ADC10CTL1 = INCH_10 + ADC10DIV_3;       // Temp Sensor ADC10CLK/4
    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REFON + ADC10ON;
    __delay_cycles(1000);                   // Wait for ADC Ref to settle
	

	for(;;)	{
		LcdClear();
		LcdString("T=");

		unsigned int adc, temp;
                                            // take temperature reading
		ADC10CTL0 |= ENC + ADC10SC;         // Sampling and conversion start
		while (!(ADC10CTL0 & ADC10IFG));
		adc = ADC10MEM;
		temp = ((adc * 27069L - 18169625L) >> 16);
										// formula from http://forum.43oh.com/topic/1954-using-the-internal-temperature-sensor/
		while (ADC10CTL0 & ADC10BUSY);      // wait for non-busy
		ADC10CTL0 &= ~ENC;
		ADC10CTL0 &= ~ADC10IFG;

			char
				c = temp % 10,
				b = (temp / 10) %10,
				a = temp / 100;
			if (a != 0)
				LcdCharacter('0' + a);
			LcdCharacter('0' + b);
			LcdCharacter('0' + c);
			LcdCharacter(0x80);
			LcdCharacter('C');
	}
	return 0;
}