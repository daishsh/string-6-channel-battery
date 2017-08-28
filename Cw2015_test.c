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
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X20;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XF3;		    //��������ʼֵ���ã�ע�Ⲩ������4800��
	TL1=0XF3;
//	ES=1;						//�򿪽����ж�
//	EA=1;						//�����ж�
	TR1=1;					    //�򿪼�����
}

void Delay10ms(unsigned int c)   //��� 0us
{
    unsigned char a, b;

	//--c�Ѿ��ڴ��ݹ�����ʱ���Ѿ���ֵ�ˣ�������for����һ��Ͳ��ø�ֵ��--//
    for (;c>0;c--)
	{
		for (b=38;b>0;b--)
		{
			for (a=130;a>0;a--);
		}          
	}       
}