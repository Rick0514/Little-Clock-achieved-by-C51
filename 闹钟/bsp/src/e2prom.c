#include "e2prom.h"

//AT24C512 driver---------------------
void e2p_delay()  //5us
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void e2p_delayms(u16 z)
{
	 u16 x,y;
	 for(x=z;x>0;x--)
		for(y=110;y>0;y--); 
}

void e2p_init()
{
	SDA = 1;
	e2p_delay();
	SCL = 1;
	e2p_delay();
}

void e2p_start()
{
	SDA = 1;
	e2p_delay();
	SCL = 1;
	e2p_delay();
	SDA = 0;
	e2p_delay();
}

void e2p_stop()
{
	SDA = 0;
	e2p_delay();
	SCL = 1;
	e2p_delay();
	SDA = 1;
	e2p_delay();
}

void e2p_at_ack()
{
	u8 i;
	SCL = 1;
	e2p_delay();
	while((SDA == 1) && (i++ < 50)){};
	SCL = 0;
	e2p_delay();
}

void e2p_write_byte(u8 dat)
{
	u8 mask;
	for(mask = 0x80; mask != 0; mask >>= 1)
	{
		SCL = 0;
		e2p_delay();
		if(mask & dat)
			SDA = 1;
		else
			SDA = 0;
		e2p_delay();
		SCL = 1;
		e2p_delay();
	}	
	SCL = 0;
	e2p_delay();
	SDA = 1;
	e2p_delay();
}

u8 e2p_read_byte()
{
	u8 dat = 0;
	u8 i;
	SCL = 0;
	e2p_delay();
	SDA = 1;
	e2p_delay();
	for(i = 0; i < 8; i++)
	{
		SCL = 1;
		e2p_delay();
		dat = ((dat << 1) | SDA);
		SCL = 0;
		e2p_delay();
	}
	return dat;
}

void e2p_write_data(u8 addr, u8 dat)
{
	u8 com = 0xa0;
	e2p_start();
	e2p_write_byte(com);
	e2p_at_ack();
	e2p_write_byte(addr);
	e2p_at_ack();
	e2p_write_byte(dat);
	e2p_at_ack();
	e2p_stop();
	e2p_delayms(1);
}

u8 e2p_read_data(u8 addr)
{
	u8 dat;
	e2p_start();
	e2p_write_byte(0xa0);
	e2p_at_ack();
	e2p_write_byte(addr);
	e2p_at_ack();
	e2p_start();
	e2p_write_byte(0xa1);
	e2p_at_ack();
	dat = e2p_read_byte();
	e2p_stop();
	return dat;
}
