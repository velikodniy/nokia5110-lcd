#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>

void LcdInit(void);
void LcdCharacter(char);
void LcdClear(void);
void LcdString(char*);
void LcdWrite(uint8_t, uint8_t);

#endif