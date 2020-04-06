#include <reg52.h>
#include "lcd.h"
#include "e2prom.h"

sbit key1_1s = P2^4;
sbit key2_1s = P2^5;
sbit key2_3s = P2^6;

sbit beep = P2^3;
u8 code time6_lcd[] = {0,1,3,4,6,7};
u8 code on[] = "c: on ";
u8 code off[] = "c: off";

struct Time;
struct Time_Params;


//-------------------------struct-----------------------------
struct Time
{
	u8 hour;
	u8 min;
	u8 sec;
};

struct Time_Params
{
	u8 clock_act;
	int time_pos;
	u8 time_sta[2];
	u8 time6[6];
	struct Time cur_time;
	struct Time clo_time;
};

struct Time_Params time = {0,5,{0},{0},{0,0,0},{0,0,0}};

//------------------------------------------------------------

//-------------------------function--------------------------
void write_sfm(u8 loc, u8 num)
{
	lcd_write_data(num + '0');
	lcd_write_com(0x80 + 0x40 + loc);
}

void preprocess(u8 *time6)
{
	u8 i;
	if((time6[0]*10 + time6[1]) >= 24)
	{
		time6[0] = 0;
		time6[1] = 0;
	}
	for(i=2; i<5; i=i+2)	//min, sec
		time6[i] = (time6[i] > 5 ? 5 : time6[i]);
}

void time6_time(struct Time *pt, u8* time6)
{
	preprocess(time6);
	pt->sec = time6[4] * 10 + time6[5];
	pt->min = time6[2] * 10 + time6[3];
	pt->hour = time6[0] * 10 + time6[1];
}

void time_time6(struct Time t, u8* time6)
{
	time6[5] = t.sec % 10;
	time6[4] = t.sec / 10;
	time6[3] = t.min % 10;
	time6[2] = t.min / 10;
	time6[1] = t.hour % 10;
	time6[0] = t.hour / 10;
}

void delayms(u16 z)
{
	 u16 x,y;
	 for(x=z;x>0;x--)
		for(y=110;y>0;y--); 
}

void keyscan()
{
	u8 i, j, tmp;
	u16 cnt = 0;
	for(i=0; i<2; i++)
	{
		P1 = ~(0x01 << i);
		tmp = 0x10;
		for(j=0; j<4; j++)
		{
			tmp = (~(0x10 << j)) & 0xf0;
			if((P1 & 0xf0) == tmp)
			{
				delayms(5);
				if((P1 & 0xf0) == tmp)
				{
					beep = 1;
					if(time.time_sta[0] | time.time_sta[1])	//set cur_time	0~7
					{
						time.time6[time.time_pos] = j + i*4;
						write_sfm(time6_lcd[time.time_pos], j + i*4);
					}
				}
				while((P1 & 0xf0) == tmp);
				beep = 0;
			}
		}
	}
	P1 = 0xfb; 
	delayms(1);
	for(j=0; j<2; j++)
	{
		tmp = (~(0x10 << j)) & 0xf0;
		if((P1 & 0xf0) == tmp)
		{
			delayms(5);
			if((P1 & 0xf0) == tmp)
			{
				beep = 1;
				if(time.time_sta[0] | time.time_sta[1])	//set cur_time	8~9
				{
					time.time6[time.time_pos] = j + 8;
					write_sfm(time6_lcd[time.time_pos], j+8);
				}
			}
			while((P1 & 0xf0) == tmp);
			beep = 0;
		}
	}

	switch(P1 & 0xf0)	// ??1??
	{
		case 0xb0:
		{
			delayms(5);
			if((P1 & 0xf0) == 0xb0)
			{
				beep = 1;
				if(time.time_sta[0] || time.time_sta[1])	
				{
					tmp = (time.time6[time.time_pos] < 9 ? ++time.time6[time.time_pos] 
					: time.time6[time.time_pos]);
					write_sfm(time6_lcd[time.time_pos], tmp);
				}
			}
			while((P1 & 0xf0) == 0xb0);
			beep = 0;
			break;
		}
		case 0x70:
		{
			delayms(5);
			if((P1 & 0xf0) == 0x70)
			{
				beep = 1;
				if(time.time_sta[0] || time.time_sta[1])	
				{
					tmp = (time.time6[time.time_pos] > 0 ? --time.time6[time.time_pos]
					: time.time6[time.time_pos]);
					write_sfm(time6_lcd[time.time_pos], tmp);
				}
			}
			while((P1 & 0xf0) == 0x70);
			beep = 0;
			break;
		}
	}
	P1 = 0xf7;
	delayms(1);
	switch(P1 & 0xf0)
	{
		case 0xe0:	//change position
		{
			delayms(5);
			if((P1 & 0xf0) == 0xe0)
			{
				beep = 1;
				if(time.time_sta[0] | time.time_sta[1])
					time.time_pos = (time.time_pos + 1) % 6;
				lcd_write_com(0x0c);
				lcd_write_com(0x80 + 0x40 + time6_lcd[time.time_pos]);
				lcd_write_com(0x0f);
			}
			while((P1 & 0xf0) == 0xe0);
			beep = 0;
			break;
		}
		case 0xd0:
		{
			delayms(5);
			if((P1 & 0xf0) == 0xd0)
			{
				beep = 1;
				if(time.time_sta[0] | time.time_sta[1])
					time.time_pos = ((--time.time_pos) < 0 ? 5 : time.time_pos);
				lcd_write_com(0x0c);
				lcd_write_com(0x80 + 0x40 + time6_lcd[time.time_pos]);
				lcd_write_com(0x0f);
			}
			while((P1 & 0xf0) == 0xd0);
			beep = 0;
			break;
		}
		case 0xb0:	//funtion key 1: set time
		{
			delayms(5);
			if((P1 & 0xf0) == 0xb0)
			{
				beep = 1;
				if(time.time_sta[1] == 0)
				{
					while((P1 & 0xf0) == 0xb0)
					{
						delayms(1);
						cnt++;
						if(cnt >= 1000)		key1_1s = 1;
					}
					key1_1s = 0;
					
					if(time.time_sta[0])
					{
						time.time_sta[0] = 0;
						time.time_pos = 5;
						lcd_write_com(0x0c);
						if(cnt < 1000)	//press 1						
							time6_time(&time.cur_time, time.time6);	// count time

						TR0 = 1;
					}
					else
					{
						TR0 = 0;
						time_time6(time.cur_time, time.time6);
						time.time_sta[0]++;
						lcd_write_com(0x80 + 0x40 + time6_lcd[time.time_pos]);
						lcd_write_com(0x0f);
					}
				}
				beep = 0;
					
			}
			break;
		}
		case 0x70:	//function key2 : set clock
		{
			delayms(5);
			if((P1 & 0xf0) == 0x70)
			{
				beep = 1;
				if(time.time_sta[0] == 0)
				{
					
					while((P1 & 0xf0) == 0x70)
					{
						delayms(1);
						cnt++;
						if(cnt > 1000)
							key2_1s = 1;
						if(cnt > 3000)
							key2_3s = 1;
					}
					key2_1s = 0;
					key2_3s = 0;
					if(cnt > 3000)		//press very long --> activate or deactivate clock 
					{
						time.clock_act = (~time.clock_act) & 0x01;
						if(time.clock_act)
							lcd_write_str(2, 10, on);
						else
							lcd_write_str(2, 10, off);
						time.time_sta[1] = 0;
						time.time_pos = 5;
						lcd_write_com(0x0c);
						TR0 = 1;
					}
					else
					{
						if(time.time_sta[1])	//reset and count
						{
							time.time_sta[1] = 0;
							time.time_pos = 5;
							lcd_write_com(0x0c);
							if(cnt < 1000)		//press last for 1s, cansel set time
								time6_time(&time.clo_time, time.time6);

							TR0 = 1;
						}
						else
						{
							TR0 = 0;
							time_time6(time.cur_time, time.time6);
							time.time_sta[1]++;
							lcd_write_com(0x80 + 0x40 + time6_lcd[time.time_pos]);
							lcd_write_com(0x0f);
							
						}
					}
					
				}
				beep = 0;
			}
			break;
		}
	}
	
}

void write_time(struct Time time)
{
	u8 dat[9];
	dat[0] = time.hour/10 + '0';
	dat[1] = time.hour%10 + '0';
	dat[2] = ':';
	dat[3] = time.min/10 + '0';
	dat[4] = time.min%10 + '0';
	dat[5] = ':';
	dat[6] = time.sec/10 + '0';
	dat[7] = time.sec%10 + '0';
	dat[8] = '\0';
	lcd_write_str(2, 1, dat);

}


u8 th0, tl0;
void configTimer0(u16 xms) 
{
	TMOD = 0x01;
	th0 = (65536 - 1000 * xms) >> 8;
	tl0 = (65536 - 1000 * xms) % 256;
	TH0 = th0;
	TL0 = tl0;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
}

//------------------------------------------------------------
//-----------------------------main----------------------------
void main()
{
	u8 first_time;
	u8 beeptime = 100;	//10s
	u8 code date[] = "2020-4-3 Fri";
	beep = 0;
	key1_1s = 0;
	key2_1s = 0;
	key2_3s = 0;
	lcd_init();
	e2p_init();
	lcd_write_str(1,2,date);
	lcd_write_str(2, 10, off);
	first_time = e2p_read_data(0);
	if(first_time != 'a')
	{
		e2p_write_data(0, 'a');
		e2p_write_data(1, time.cur_time.hour);
		e2p_write_data(2, time.cur_time.min);
		e2p_write_data(3, time.cur_time.sec);
	}
	else
	{
		time.cur_time.hour = e2p_read_data(1);
		time.cur_time.min = e2p_read_data(2);
		time.cur_time.sec = e2p_read_data(3);
	}
	write_time(time.cur_time);
	
	configTimer0(50);
	while(1)
	{
		keyscan();
		if(time.clock_act)
		{
			if((time.cur_time.hour == time.clo_time.hour)
				&& (time.cur_time.min == time.clo_time.min) 
			&& (time.cur_time.sec == time.clo_time.sec))
			{
				TR0 = 0;
				beep = 1;
				while(beeptime--)	delayms(50);
				beep = 0;
				beeptime = 100;
				TR0 = 1;
				time.clock_act = 0;
				lcd_write_str(2, 10, off);
			}
		}
	}
		
}

void int0() interrupt 1
{
	static u8 cnt = 0;
	
	if(++cnt == 20)	//1s
	{
		cnt = 0;
		time.cur_time.sec++;
		e2p_write_data(3,time.cur_time.sec);
		if(time.cur_time.sec >= 60)
		{
			time.cur_time.sec = 0;
			time.cur_time.min++;
			if(time.cur_time.min >= 60)
			{
				time.cur_time.min = 0;
				time.cur_time.hour++;
				if(time.cur_time.hour >= 24)
					time.cur_time.hour = 0;
				e2p_write_data(1, time.cur_time.hour);
			}
			e2p_write_data(2, time.cur_time.min);
			
		}
		write_time(time.cur_time);
	}

	TH0 = th0;
	TL0 = tl0;
}









