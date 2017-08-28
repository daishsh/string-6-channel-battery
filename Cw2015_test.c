#include <reg51.h>
#include "Cellwise CW201x Driver for MCU.h"
extern STRUCT_CW_BATTERY   cw_bat;
unsigned char cw_bat_init(void);
void cw_bat_work();

unsigned char enter = '\n';
void UsartConfiguration();
void Delay10ms(unsigned int c);

void show_enter(){
  	SBUF = enter;
	while(!TI);
	TI=0;
}

void show_number(unsigned int ret){ 
	unsigned int one = 0;
	unsigned int yushu = 0;
	one = ret / 100000;
	yushu = ret % 100000;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	one = yushu / 10000;
	yushu = yushu % 10000;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	one = yushu / 1000;
	yushu = yushu % 1000;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	one = yushu / 100;
	yushu = yushu % 100;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	one = yushu / 10;
	yushu = yushu % 10;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	one = yushu / 1;
	yushu = yushu % 1;
	SBUF = one + '0';
	while(!TI);
	TI=0;
	show_enter();
}

int main(void)
{
	unsigned char ret;
	
	UsartConfiguration();
	ret = cw_bat_init();
	
	while(1)
	{
		Delay10ms(100); 
		cw_bat_work();
		show_enter();
		show_number(cw_bat.capacity);
		show_enter();
		show_number(cw_bat.voltage);
		show_enter();
		show_number(cw_bat.time_to_empty);
		show_enter();
	}

	return 0;
}

void UsartConfiguration()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF3;		    //计数器初始值设置，注意波特率是4800的
	TL1=0XF3;
//	ES=1;						//打开接收中断
//	EA=1;						//打开总中断
	TR1=1;					    //打开计数器
}

void Delay10ms(unsigned int c)   //误差 0us
{
    unsigned char a, b;

	//--c已经在传递过来的时候已经赋值了，所以在for语句第一句就不用赋值了--//
    for (;c>0;c--)
	{
		for (b=38;b>0;b--)
		{
			for (a=130;a>0;a--);
		}          
	}       
}