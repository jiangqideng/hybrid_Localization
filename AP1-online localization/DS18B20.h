#ifndef DS18B20_H
#define DS18B20_H

#include "IO_def.h"


void Init_DS18B20(void);
unsigned char ReadOneChar_18B20(void);
void WriteOneChar_18B20(unsigned char dat);//Ð´Ò»¸ö×Ö½Ú;
void Adjust_res_18B20(char res);
void ReadTemperature(void);
void delay_ms(unsigned int n);



void delay_1ms(void);

//void delay_ms(unsigned int n);
//void delay_n_us(unsigned char n);
/******************************************************************************/
void Delay_us(unsigned char i);

extern unsigned char integer;
extern unsigned char decimal;

extern unsigned char decimal1;
extern unsigned char decimal2;

extern unsigned char tempL;
extern unsigned char tempH;

extern unsigned char fg;

#endif
