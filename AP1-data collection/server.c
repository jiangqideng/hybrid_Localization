#include "IO_def.h"
#include <reg52.h>
#include "server.h"
#include "lcd12864a.h"

void UART_init()
{
	SCON = 0x50;                 /* uart in mode 1 (8 bit), REN=1 串口通信方式1*/
	TMOD = TMOD | 0x20 ;         /* Timer 1 in mode 2 */
	TH1  = 0xFD;                 /* 9600 Bds at 11.0592MHz */
	TL1  = 0xFD;                 /* 9600 Bds at 11.0592MHz */
	//IE =0x90;
	TR1 = 1;                     /* Timer 1 run */
//	IE=0x90;					  //开启总中断和串口中断
}

//向上位机发送单个字符
void Sendchar(unsigned char cha)
{
	SBUF=cha;                                     
	while(TI==0); //串行位在接收到停止位时，内部硬件会使RI置1，该语句作用：等待发送数据完成。                                              
	TI=0;		  //软件将TI位清零
}

//向上位机发送字符串
void print_string(unsigned char* p)
{
	while(*p !='\0')
	{
	  Sendchar(*p++);
	}
}
//向上位机发送有符号8位整数，十进制表示
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

//串口接收从计算机传来的字符
uchar UART_ReceiveChar()
{
	uchar c;
	while(RI==0); //串行位在接收到停止位时，内部硬件会使RI置1，该语句作用：等待计算机往节点发送数据完成。                                              
	c=SBUF;
	RI=0;		  //软件将RI位清零
	return c;
}

void delay_1ms()
{
	unsigned int i;

	for (i=1; i<(unsigned int)(xtal*142-2); ++i);   //xtal为晶振频率，单位MHz
}

void delay_ms(unsigned int n)
{
	unsigned int i;

	for (i=0; i<n; ++i)
		delay_1ms();
}

