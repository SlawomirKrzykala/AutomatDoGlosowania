#ifndef LCD
#define LCD

#include "LCD.c"

void uintToTabChar(uint8_t num, uint8_t *tabChar, uint8_t size);


void timeToTabChar(uint16_t s, uint8_t *tabChar);


void TWI_init(void);

void TWI_start(void);


void TWI_stop(void);

void TWI_write(uint8_t byte);


void LCD_write(uint8_t data, uint8_t rs);


void LCD_init(void);


void LCD_writeText(uint8_t *napis);

void LCD_writeNumber(uint8_t num);


void LCD_writeTime(uint16_t time);


void LCD_setPosition(uint8_t position);


void LCD_clear();
 #endif