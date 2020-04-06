#include "lcd.h"


//LCD driver---------------------
void lcd_delayms(u16 z)
{
	 u16 x,y;
	 for(x=z;x>0;x--)
		for(y=110;y>0;y--); 
}

void lcd_write_com(u8 com)
{
	RDA = 0;
	P3 = com;
	lcd_delayms(5);
	LCDEN = 1;
	lcd_delayms(5);
	LCDEN = 0;
}

void lcd_write_data(u8 dat)
{
	RDA = 1;
	P3 = dat;
	lcd_delayms(5);
	LCDEN = 1;
	lcd_delayms(5);
	LCDEN = 0;
}

void lcd_write_str(u8 row, u8 col, u8 *pstr)
{
	u8 loc;
	if(row == 1)
		loc = 0x80 + (col - 1);
	else
		loc = 0x80 + 0x40 + (col - 1);
	
	lcd_write_com(loc);
	while(*pstr != '\0')
		lcd_write_data(*pstr++);
	
}


void lcd_init()
{
	LCDEN = 0;
	lcd_write_com(0x38);
	lcd_write_com(0x0c);
	lcd_write_com(0x06);
	lcd_write_com(0x01);
}
