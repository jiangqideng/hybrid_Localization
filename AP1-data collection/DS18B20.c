#include "DS18B20.h"
#include <intrins.h>

#include "DS18B20.h"
#include "cc1100.h"


unsigned char tempL=0; //设全局变量
unsigned char tempH=0; 
unsigned char integer = 0;//测量到的温度的整数部分
unsigned char decimal1 = 0;//小数第一位
unsigned char decimal2 = 0;//小数第二位
unsigned char decimal = 0;//两位小数
unsigned char fg =0;



void delay_n_us(unsigned char n)
{
	unsigned char i = 0;
	for (i=0; i<n-1; ++i)
	{
		_nop_();
	}
}
/******************************************************************************/
void Delay_us(unsigned char i)//延时函数		 num = 1  18.44 us 4   48us  8 65us	  10  77.04us   100   663us
{
	while(i--);

}	
/******************************************************************************/
void Init_DS18B20(void)//初始化ds1820
{
	unsigned char x=0;

	DQ = 1;    //DQ复位
	//Delay_us(8);  //稍做延时
	delay_n_us(10);
	DQ = 0;    //单片机将DQ拉低
	//Delay_us(80); //精确延时 大于 480us
	delay_n_us(480);
	DQ = 1;    //拉高总线
//	Delay_us(8);	  //65us
	delay_n_us(65);

	//x = DQ;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	while (DQ == 0);


	Delay_us(20);
}
/******************************************************************************/
unsigned char ReadOneChar_18B20(void)//读一个字节
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; // 给脉冲信号
		dat>>=1;
		DQ = 1; // 给脉冲信号
		if(DQ)
		dat|=0x80;
		//Delay_us(4);
		delay_n_us(60);
	}
	return(dat);
}
/******************************************************************************/
void WriteOneChar_18B20(unsigned char dat)//写一个字节
{										//数据线从高电平拉至低电平，产生写起始信号。15us之内将所需写的位送到数据线上
	unsigned char i=0;
	for (i=8; i>0; i--)
	{
		DQ = 0;
		DQ = dat&0x01;
		//Delay_us(5);
	    delay_n_us(60);
		DQ = 1;
		dat>>=1;
	}
}
/******************************************************************************/
void Adjust_res_18B20(char res) ///res 分别等于 0x1f, 0x3f, 0x5f 温度读数分辨率分别对应
                           //              0.5, 0.25, 0.125   
{
  Init_DS18B20();        //复位
  WriteOneChar_18B20(0xcc);  //跳过Rom
 WriteOneChar_18B20(0x4e);  //写暂存器
 WriteOneChar_18B20(0x02);  //写TH 
 WriteOneChar_18B20(0x01);  //写TL
  //WriteOneChar_18B20(0x5f);  //写结构寄存器
  WriteOneChar_18B20(res);
  Init_DS18B20();       //复位
  WriteOneChar_18B20(0xcc); //跳过Rom 
  WriteOneChar_18B20(0x48); //把暂存器内容写到EPRam中
}

/********************************************************************************/
void ReadTemperature(void)//读取温度
{
	//unsigned char a=0;
	//unsigned char b=0;
	//unsigned int t=0;
	//float tt=0;
	Init_DS18B20();
	WriteOneChar_18B20(0xCC); // 跳过读序号列号的操作
	WriteOneChar_18B20(0x44); // 启动温度转换
	delay_ms(800);
	Init_DS18B20();
	WriteOneChar_18B20(0xCC); //跳过读序号列号的操作
	WriteOneChar_18B20(0xBE); //读取温度寄存器

	tempL=ReadOneChar_18B20(); //读出温度的低位LSB
 	tempH=ReadOneChar_18B20(); //读出温度的高位MSB 
  
	if(tempH>0x7f)      //最高位为1时温度是负
	{
		tempL=~tempL;         //补码转换，取反加一
		tempH=~tempH+1;       
		fg=0;      //读取温度为负时fg=0
    }
	integer = tempL/16+tempH*16;      //整数部分
	decimal1 = (tempL&0x0f)*10/16; //小数第一位
	decimal2 = (tempL&0x0f)*100/16%10;//小数第二位
	decimal=decimal1*10+decimal2; //小数两位
	/*
	a=ReadOneChar_18B20();  //读低8位
	b=ReadOneChar_18B20(); //读高8位
	t=b;
	t<<=8;
	t=t|a;
	tt=t*0.0625;
	t= tt*10+0.5; //放大10倍输出并四舍五入
	return(t);
	*/


}
/******************************************************************************/