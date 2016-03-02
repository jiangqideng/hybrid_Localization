#include "IO_def.h"
#include <reg52.h>
#include "server.h"
#include "lcd12864a.h"
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

void my_delay(unsigned int n)
{	uchar TH_temp=TH1;
	uchar TL_temp=TL1;
 	while ((TH1*256+TL1-TH_temp*256-TL_temp)<=n) {}
}
