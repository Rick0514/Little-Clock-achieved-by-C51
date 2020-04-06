#ifndef _LCD_H
#define _LCD_H
#include <reg52.h>

sbit RDA = P2^0;
sbit LCDEN = P2^2;

#ifndef DATA_TYPE
#define DATA_TYPE
typedef unsigned int u16;
typedef unsigned char u8;
#endif

void lcd_delayms(u16 z);
void lcd_write_com(u8 com);
void lcd_write_data(u8 dat);
void lcd_write_str(u8 row, u8 col, u8 *pstr);
void lcd_init();


#endif