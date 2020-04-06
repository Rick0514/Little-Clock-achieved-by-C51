#ifndef _E2PROM_H
#define _E2PROM_H
#include <reg52.h>
#include <intrins.h>

sbit SCL = P2^0;
sbit SDA = P2^1;


#ifndef DATA_TYPE
#define DATA_TYPE
typedef unsigned int u16;
typedef unsigned char u8;
#endif


void e2p_delay();
void e2p_delayms(u16 z);
void e2p_init();
void e2p_start();
void e2p_stop();
void e2p_at_ack();
void e2p_write_byte(u8 dat);
u8 e2p_read_byte();
void e2p_write_data(u8 addr, u8 dat);
u8 e2p_read_data(u8 addr);



#endif