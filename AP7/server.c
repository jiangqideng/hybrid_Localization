#include "IO_def.h"
#include <reg52.h>
#include "server.h"
#include "lcd12864a.h"

void UART_init()
{
	SCON = 0x50;                 /* uart in mode 1 (8 bit), REN=1 ����ͨ�ŷ�ʽ1*/
	TMOD = TMOD | 0x20 ;         /* Timer 1 in mode 2 */
	TH1  = 0xFD;                 /* 9600 Bds at 11.0592MHz */
	TL1  = 0xFD;                 /* 9600 Bds at 11.0592MHz */
	//IE =0x90;
	TR1 = 1;                     /* Timer 1 run */
//	IE=0x90;					  //�������жϺʹ����ж�
}

//����λ�����͵����ַ�
void Sendchar(unsigned char cha)
{
	SBUF=cha;                                     
	while(TI==0); //����λ�ڽ��յ�ֹͣλʱ���ڲ�Ӳ����ʹRI��1����������ã��ȴ�����������ɡ�                                              
	TI=0;		  //�����TIλ����
}

//����λ�������ַ���
void print_string(unsigned char* p)
{
	while(*p !='\0')
	{
	  Sendchar(*p++);
	}
}
//����λ�������з���8λ������ʮ���Ʊ�ʾ
void Send_int8(int int8)
{
	unsigned char bw,sw,gw;
	if(int8>0)
	{
		Sendchar('+');
	}
	else
	{
		int8=-int8;
		Sendchar('-');		
	}
	bw=int8/100;
	sw=(int8-100*bw)/10;
	gw=int8-100*bw-10*sw;
	Sendchar(bw+'0');
	Sendchar(sw+'0');
	Sendchar(gw+'0');
}

//���ڽ��մӼ�����������ַ�
uchar UART_ReceiveChar()
{
	uchar c;
	while(RI==0); //����λ�ڽ��յ�ֹͣλʱ���ڲ�Ӳ����ʹRI��1����������ã��ȴ���������ڵ㷢��������ɡ�                                              
	c=SBUF;
	RI=0;		  //�����RIλ����
	return c;
}

void delay_1ms()
{
	unsigned int i;

	for (i=1; i<(unsigned int)(xtal*142-2); ++i);   //xtalΪ����Ƶ�ʣ���λMHz
}

void delay_ms(unsigned int n)
{
	unsigned int i;

	for (i=0; i<n; ++i)
		delay_1ms();
}

