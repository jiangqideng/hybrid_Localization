/****************************************************************************
 **             - IO_def.h -
 **
 **     系统硬件属性定义部分：
 **			cpu类型 IO管脚定义
 **     系统时钟等
 **
 **    
 **     File version: $2008.2.1$
 **
 ***************************************************************************/

#ifndef IO_def_h
#define IO_def_h
#include<reg52.h>
#include<intrins.h>

sfr		P4		 =0xC0;
sfr		WDT_CONTR=0xC1;

#define uint32_t unsigned long int
#define uint16_t unsigned int
#define uint8_t  unsigned char

#define xtal 11
	
sbit DQ=P2^2;

// 液晶管脚
//sbit LCD_12864s_CS1 =P1^4;
//sbit LCD_12864s_RES =P1^3;
//sbit LCD_12864s_A0P=P1^2;
//sbit LCD_12864s_WR =P1^1;//写
//sbit LCD_12864s_RD =P1^0;//读
//#define LCD_12864s_DB          P0
//#define LCD_12864s_DB_PIN      P0


#define BYTE_BIT0	0x01
#define BYTE_BIT1	0x02
#define BYTE_BIT2	0x04
#define BYTE_BIT3	0x08
#define BYTE_BIT4	0x10
#define BYTE_BIT5	0x20
#define BYTE_BIT6	0x40
#define BYTE_BIT7	0x80


//*****************************************************************************************
sbit 	GDO0	=P0^4;				 //通用数字输出引脚
sbit 	GDO2	=P3^3;				 //通用数字输出引脚
sbit	MISO	=P1^1;				 //主入从出
sbit	MOSI	=P1^2;				 //主出从入
sbit	SCK		=P0^2;				 //时钟信号
sbit	CSN		=P1^0;				 //串行配置接口，片选

sbit    PAC     =P1^3;				 //

//*****************************************************************************************
sbit    KEY1    =P1^7;
sbit    KEY2    =P1^6;
//*****************************************************************************************
sbit    LED1  = P4^0;
sbit    LED2   =P4^3;

//sbit	JDQ= P2^5;

sbit BELL =P3^7;


#endif  //IO_def_h


