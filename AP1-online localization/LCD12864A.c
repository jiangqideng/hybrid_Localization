/****************�����HGO12864A���Գ���**************************************************/
/******************************************************************/
#include <reg52.h>
#include <intrins.h>
#include <absacc.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "LCD12864A.h"

/*  �����ַ��ⶨ��  */
unsigned char code success[16]={ 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,    /*" "=00H*/
0x00,0x00,0x00,0x4F,0x00,0x00,0x00,0x00,    /*"!"=01H*/

};
 // ���� ����ʽ ˳��

/*---16*16�����ֿⶨ��------------*/
unsigned char code largedigtal[500]={ 0x00,

0x00,0x07,0x08,0x10,0x10,0x10,0x08,0x07,    /* 0 ��ڵ�ַ0*/
0x00,0xf0,0x08,0x04,0x04,0x04,0x08,0xf0,

0x00,0x08,0x18,0x1f,0x00,0x00,0x00,0x00,    /* 1 ��ڵ�ַ1*/
0x00,0x04,0x04,0xfc,0x04,0x04,0x00,0x00,

0x00,0x0e,0x10,0x10,0x10,0x11,0x0e,0x00,    /* 2 ��ڵ�ַ2*/
0x00,0x1c,0x34,0x64,0xc4,0x84,0x0c,0x00,

0x00,0x0c,0x10,0x10,0x10,0x11,0x0e,0x00,    /* 3 ��ڵ�ַ3*/
0x00,0x18,0x04,0x04,0x84,0x88,0x70,0x00,

0x00,0x01,0x02,0x04,0x08,0x1f,0x00,0x00,    /* 4 ��ڵ�ַ4*/
0x00,0xe0,0x20,0x24,0x24,0xfc,0x24,0x20,

0x00,0x1f,0x10,0x11,0x11,0x11,0x10,0x00,    /* 5 ��ڵ�ַ5*/
0x00,0x88,0x84,0x04,0x04,0x08,0xf0,0x00,

0x00,0x07,0x08,0x11,0x11,0x11,0x11,0x08,    /* 6 ��ڵ�ַ6*/
0x00,0xf0,0x88,0x04,0x04,0x04,0x08,0xf0,

0x00,0x18,0x10,0x10,0x10,0x11,0x1c,0x10,    /* 7 ��ڵ�ַ7*/
0x00,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,

0x00,0x0E,0x11,0x10,0x10,0x11,0x0E,0x00,
0x00,0x38,0x44,0x84,0x84,0x44,0x38,0x00,    /*"8",��ڵ�ַ8*/

0x00,0x07,0x08,0x10,0x10,0x08,0x07,0x00,
0x00,0x00,0x8C,0x44,0x44,0x88,0xF0,0x00,    /*"9",��ڵ�ַ9*/

0x00,0x00,0x03,0x1C,0x07,0x00,0x00,0x00,0x04,0x3C,0xC4,0x40,0x40,0xE4,0x1C,0x04,/*"A",10*/

0x10,0x1F,0x11,0x11,0x11,0x0E,0x00,0x00,0x04,0xFC,0x04,0x04,0x04,0x88,0x70,0x00,/*"B",11*/

0x03,0x0C,0x10,0x10,0x10,0x10,0x1C,0x00,0xE0,0x18,0x04,0x04,0x04,0x08,0x10,0x00,/*"C",12*/

0x10,0x1F,0x10,0x10,0x10,0x08,0x07,0x00,0x04,0xFC,0x04,0x04,0x04,0x08,0xF0,0x00,/*"D",13*/

0x10,0x1F,0x11,0x11,0x17,0x10,0x08,0x00,0x04,0xFC,0x04,0x04,0xC4,0x04,0x18,0x00,/*"E",14*/

0x10,0x1F,0x11,0x11,0x17,0x10,0x08,0x00,0x04,0xFC,0x04,0x00,0xC0,0x00,0x00,0x00,/*"F",15*/

0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,
0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x00,    /*":",��ڵ�ַ16*/

0x00,0x38,0x0F,0x10,0x10,0x10,0x0C,0x00,0x00,0x00,0xF0,0x08,0x04,0x0C,0x08,0x00,/*"��",17*/
//0x00,0x00,0x00,0x01,0x01,0x11,0x1F,0x00,0x00,0x70,0x88,0x04,0x04,0x08,0xFC,0x04,/*"d",17*/

0x10,0x1F,0x11,0x11,0x11,0x0E,0x00,0x00,0x04,0xFC,0x04,0x04,0x04,0x88,0x70,0x00,/*"B",18*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00,0x00,/*".",19*/

//0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xF8,0x80,0x80,0x80,0x00,/*"+",20*/

0x01,0x01,0x00,0x01,0x01,0x01,0x00,0x00,0x04,0xFC,0x84,0x00,0x00,0x04,0xFC,0x04,/*"n",20*/

//0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,/*"-",21*/

0x10,0x1F,0x0C,0x03,0x00,0x10,0x1F,0x10,0x04,0xFC,0x04,0x00,0xE0,0x18,0xFC,0x00,/*"N",21*/


0x02,0x03,0x01,0x00,0x01,0x03,0x02,0x00,0x10,0x30,0xE0,0xC0,0xE0,0x30,0x10,0x00,/*"x",22*/

0x02,0x03,0x00,0x00,0x00,0x03,0x02,0x00,0x02,0x82,0xE6,0x1C,0xF0,0x80,0x00,0x00,/*"y",23*/

};


/*  ���Ĳ˵����� */
/*  16*16���ֶ���*/

unsigned char code hanzi[300]={ 0x00,



0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*"��",0*/

 
0x60,0x90,0x90,0x67,0x1F,0x30,0x20,0x40,0x40,0x40,0x40,0x40,0x20,0x78,0x00,0x00,
0x00,0x00,0x00,0xE0,0xF8,0x0C,0x04,0x02,0x02,0x02,0x02,0x02,0x04,0x08,0x00,0x00,/*"��",1*/


0x00,0x02,0x42,0x42,0x42,0x43,0x7E,0x42,0x43,0x42,0x42,0x42,0x02,0x02,0x00,0x00,
0x01,0x02,0x04,0x08,0x30,0xC0,0x00,0x00,0xFC,0x02,0x02,0x02,0x02,0x0E,0x00,0x00,/*"��",2*/

0x00,0x01,0x06,0x1F,0xE0,0x00,0x20,0x24,0x24,0xA4,0x64,0x24,0x24,0x24,0x20,0x00,
0x80,0x00,0x00,0xFF,0x00,0x00,0x00,0x9F,0x92,0x92,0x92,0x92,0x92,0x9F,0x00,0x00,/*"��",3*/

0x01,0x01,0x01,0x7D,0x45,0x45,0x45,0x45,0x45,0x45,0x45,0x7D,0x01,0x01,0x01,0x00,
0x00,0x00,0x00,0x60,0xA0,0x20,0x20,0x20,0x22,0x21,0x22,0x3C,0x00,0x00,0x00,0x00,/*"��",4*/

0x00,0x02,0x42,0x22,0x1A,0x02,0x02,0xFE,0x02,0x02,0x0A,0x12,0x63,0x00,0x00,0x00,
0x00,0x02,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0xFF,0x00,0x00,0x00,/*"��",5*/

0x10,0x10,0x17,0x94,0x74,0x14,0x17,0x10,0x10,0x13,0x30,0xD0,0x17,0x10,0x10,0x00,
0x00,0x00,0xFF,0x90,0x92,0x91,0xFE,0x00,0x00,0xF0,0x02,0x01,0xFE,0x00,0x00,0x00,/*"ǰ",6*/

0x00,0x01,0x06,0x1F,0xE0,0x08,0x09,0x08,0x88,0x68,0x08,0x08,0x0B,0x08,0x00,0x00,
0x80,0x00,0x00,0xFF,0x02,0x02,0x82,0x7A,0x02,0x02,0x0E,0x72,0x82,0x02,0x02,0x00,/*"λ",7*/

0x00,0xE8,0xA8,0xAB,0xAA,0xEA,0xAA,0xBE,0xAA,0xEA,0xAA,0xAB,0xA8,0xE8,0x00,0x00,
0x02,0x02,0x02,0xFE,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xFE,0x02,0x02,0x02,0x00,/*"��",8*/


};


/*************************************************/
 /*��ʱ*/
/*************************************************/
void delay_lcd()    
 {unsigned char i,j;
   for(i=32;i>0;i--)
    for(j=255;j>0;j--);
 }

 /*************************************************/
 /*��ʱ*/
/*************************************************/
void delay1()    
 {unsigned char i,j;
   for(i=255;i>0;i--)
    {
	for(j=255;j>0;j--) 
	{nop();
	 nop();
	 nop();
	 nop();
	 nop();
	 nop();
	 nop();
	 nop();}
	}
 }
/*************************************************/
//��ʼ��
/*************************************************/
void lcd_init(void)
{
  rst=0;
  delay_lcd();
  rst=1;
  delay_lcd();
  wcom(0xa3);        //LCD BIAS SET ( A2 1/8;A3 1/65DUTY)
  wcom(0xa0);        //ADC  select,NORMAL   0-->127
  wcom(0xaf);        // DISPLAY ON/OFF
  wcom(0x2f);        //power control(VB,VR,VF=1,1,1)
  wcom(0x23);        // VOLT REGULATOR RADIO SET 
  wcom(0x81);        //ELECTRONIC VOLUME mode setting 100B �Աȶ�����
  wcom(0x28);        //REFERENCE VOLT REG  set
  wcom(0xc0);        //SHL SELECT
  wcom(0xaf);        //DISPLAY ON/OFF
  wcom(0x40);        // DISPLAY START LINE SET
  nop();
  nop();
}



/*************************************************/
//д����
/*************************************************/

void wcom(unsigned char com)
 { 
   cs1=0;
   nop();
   a0=0;                   //a0='0'��д������
   nop();
   write(com);
   nop();
   nop(); 
   cs1=1;
}

/*************************************************/
//д����
/*************************************************/

void wdata(unsigned char dat)
 {
   cs1=0;
   nop();
   a0=1;                  //a0='1',д������
   nop();
   write(dat);  
   nop();
   nop(); 
   cs1=1;
}
/*************************************************
//������λ���, SPI��ʽд��8bit ����
/*************************************************/

void write(unsigned char dat)
 {  unsigned char s,temp;
    int i;
    sclk=0;
    s=dat;

    for(i=8;i>0;i--)
      {sclk=0;
       nop();
       nop();
       temp=s & 0x80;                   //���͸�λ�����͵�λ
       if(temp)
            {di=1;}
       else {di=0;}

       sclk=1;   
       s=s<<1;  
      }
 } 


/********************************************/
/*  �����ַ�д�뺯��:CW_PR    16*16         */
/*            д��һ������                  */
/*                                          */
/********************************************/
void writeone(unsigned char cod,unsigned char page,unsigned char seg)
{ unsigned char zmseg;

        wcom(0xaf);// ����ʾ
	    wcom(0xb0+page);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=16;zmseg>0;zmseg--)
        {   
			wdata(hanzi[cod*32+zmseg]);  // ����д��16������
        }
	    wcom(0xb0+page-1);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=32;zmseg>16;zmseg--)
        {   
			wdata(hanzi[cod*32+zmseg]);  // ����д��16������
        }
}





/********************************************/
/*  �����ַ�д�뺯��:CW_PR  8*8             */

void cwpr(unsigned char cod,unsigned char page,unsigned char seg)
{ unsigned char zmseg;

        wcom(0xaf);// ����ʾ
	    wcom(0xb0+page);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=0;zmseg<8;zmseg++)
        {   
			wdata(success[cod*8+zmseg]);  // ����д��8������

}
}


/********************************************/
/*  �����ַ�д�뺯��:CW_PR    16*16         */

void cwpr16(unsigned char cod,unsigned char page,unsigned char seg)
{ unsigned char zmseg;

        wcom(0xaf);// ����ʾ
	    wcom(0xb0+page);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=0;zmseg<16;zmseg++)
        {   
			wdata(largedigtal[cod*32+zmseg]);  // ����д��16������
        }
	    wcom(0xb0+page-1);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=16;zmseg<32;zmseg++)
        {   
			wdata(largedigtal[cod*32+zmseg]);  // ����д��16������
        }

}



/********************************************/
/*  �����ַ�д�뺯��:CW_PR    16*8         */
/*  д�����ֺ�Ӣ����ĸ    16�� 8��                  */
/********************************************/
void writedigital(unsigned char cod,unsigned char page,unsigned char seg)
{ unsigned char zmseg;

        wcom(0xaf);// ����ʾ
	    wcom(0xb0+page);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=8;zmseg>0;zmseg--)
        {   
			wdata(largedigtal[cod*16+zmseg]);  // ����д��16������
        }
	    wcom(0xb0+page-1);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 			
		for(zmseg=16;zmseg>8;zmseg--)
        {   
			wdata(largedigtal[cod*16+zmseg]);  // ����д��16������
        }

}






/*16*16����д���ӳ���*/
void ccwpr(unsigned char cod,unsigned char page,unsigned char seg)
{ unsigned char zmseg;

        wcom(0xaf);//����ʾ
        wcom(0xb0+page); //�趨ҳ��ַ                                            
        wcom(0x10+(0x0f&(seg>>4)));  //�е�ַ ����λ          		
        wcom(0x0f&seg);        		 //�е�ַ ����λ	
		for(zmseg=0;zmseg<=15;zmseg++)
        {   
//			wdata(CCTAB[cod*32+zmseg]);  //����д��16���ֽ�����

        }
		
        wcom(0xb0+page+1);               //������һҳ
                                 
        wcom(0x10+(0x0f&(seg>>4)));  
		
        wcom(0x0f&seg); 

       for(zmseg=16;zmseg<=31;zmseg++)  //����д��16���ֽ�����
        {   
//			wdata(CCTAB[cod*32+zmseg]);

}

}

/*************************************************/
//дdisdate_a����
/*************************************************/
void disdate_a(unsigned char dat_a,unsigned char dat_b)
{
   int seg;
   unsigned char page;
   for(page=0xb0;page<0xb9;page++)                     //дҳ��ַ��9ҳ  0xb8----0xb9
      { wcom(page);                                    //���º��ֿɸ�
        wcom(0x10);                                    //���е�ַ����λ
        wcom(0x00);                                    //���е�ַ����λ
                   
        for(seg=0;seg<64;seg++)
           { wdata(dat_a);
             wdata(dat_b);
           }
      } 
   wcom(0xaf); /*����ʾָ��*/          
}



/*************************************************/
//дdisdate_b����ȫ��д����
/*************************************************/
void disdate_b(unsigned char *p)
{
   int seg;
  unsigned char page;
   wcom(0xaf);/*����ʾ*/
   for(page=0xb0;page<0xb8;page++)                    //дҳ��ַ��8ҳ  0xb0----0xb8
	 {
          wcom(page);                      
          wcom(0x10); 
          wcom(0x00); 
                     
          for(seg=0;seg<128;seg++)
             { 
              wdata(*p++);
             }              
         }           
 }


 void clr_blank()               /*-----����Ļ-----*/
{
    writeone(15,7,112); 
    writeone(15,7,96); 
    writeone(15,7,80);         /*----����ʾ------*/
    writeone(15,7,64);
    writeone(15,7,48);
    writeone(15,7,32);
    writeone(15,7,16);  
    writeone(15,7,0);


	writeone(15,5,112); 
    writeone(15,5,96); 
    writeone(15,5,80);         /*----����ʾ------*/
    writeone(15,5,64);
    writeone(15,5,48);
    writeone(15,5,32);
    writeone(15,5,16);  
    writeone(15,5,0);

    writeone(15,3,112); 
    writeone(15,3,96); 
    writeone(15,3,80);         /*----����ʾ------*/
    writeone(15,3,64);
    writeone(15,3,48);
    writeone(15,3,32);
    writeone(15,3,16);  
    writeone(15,3,0);

    writeone(15,1,112); 
    writeone(15,1,96); 
    writeone(15,1,80);         /*----����ʾ------*/
    writeone(15,1,64);
    writeone(15,1,48);
    writeone(15,1,32);
    writeone(15,1,16);  
    writeone(15,1,0);

}
    

/****************************************************************
//��Һ������ʾһ���з�����������4λ����һλΪ����λ
//1<=page<=7,24<=seg<=120
****************************************************************/
void Dispchar(int dat,unsigned char page,unsigned char seg)			   //����ڵ�ĳ���ֻ��ʾ���Ĳ��֣�Ϊ�˹۲�������ݣ�
{
	unsigned char baiwei,shiwei,gewei;
//	if(dat>=0)
//		writedigital(20,page,seg);
//	else
//	{
//		writedigital(21,page,seg);
//		dat=-dat;
//	}
	baiwei=dat/100;
	shiwei=(dat-baiwei*100)/10;
	gewei=dat-baiwei*100-shiwei*10;
	writedigital(baiwei,page,seg);
	writedigital(shiwei,page,seg-8);
	writedigital(gewei,page,seg-16);
}


void draw_dot(unsigned char cod,unsigned char page,unsigned char seg)
{ 

        wcom(0xaf);// ����ʾ
	    wcom(0xb0+page);   // �趨ҳ��ַ���趨page������0xB0,�� 9 page                                       
        wcom(0x10+(0x0f&(seg>>4))); // �趨�е�ַ, �趨�е�ַ����0x01 , ����ֹ��Χ 0~131    , д����λ�е�ַ      		
        wcom(0x0f&seg);             // д����λ�е�ַ 		
		wdata(cod);	

}

void draw_position(xx,yy)
{
draw_map();

if (yy==1 && xx>3 && xx<75)					 //��
{	
	draw_dot(215,0,82-xx);	
	draw_dot(255,0,81-xx);
	draw_dot(215,0,80-xx);
}
if (yy==56 && xx>3 && xx<75)					 //��
{	
	draw_dot(235,7,82-xx);	
	draw_dot(255,7,81-xx);
	draw_dot(235,7,80-xx);
}
	if(yy==56 && xx==3)
	{	
	draw_dot(232,7,82-xx);	
	draw_dot(255,7,81-xx);
	draw_dot(235,7,80-xx);
	}
	if(yy==56 && xx==75)
	{	
	draw_dot(232,7,80-xx);	
	draw_dot(255,7,81-xx);
	draw_dot(235,7,82-xx);
	}
	if(yy==1 && xx==3)
	{	
	draw_dot(23,0,82-xx);	
	draw_dot(255,0,81-xx);
	draw_dot(215,0,80-xx);
	}
	if(yy==1 && xx==75)
	{	
	draw_dot(23,0,80-xx);	
	draw_dot(255,0,81-xx);
	draw_dot(215,0,82-xx);
	}



	if(yy==56 && xx==2)
	{	
	draw_dot(232,7,82-xx);	
	draw_dot(252,7,81-xx);
	draw_dot(235,7,80-xx);
	}
	if(yy==56 && xx==76)
	{	
	draw_dot(232,7,80-xx);	
	draw_dot(252,7,81-xx);
	draw_dot(235,7,82-xx);
	}
	if(yy==1 && xx==2)
	{	
	draw_dot(23,0,82-xx);	
	draw_dot(63,0,81-xx);
	draw_dot(215,0,80-xx);
	}
	if(yy==1 && xx==76)
	{	
	draw_dot(23,0,80-xx);	
	draw_dot(63,0,81-xx);
	draw_dot(215,0,82-xx);
	}


	if(yy==56 && xx==1)
	{	
	draw_dot(232,7,82-xx);	
	draw_dot(252,7,81-xx);
	draw_dot(232,7,80-xx);
	}
	if(yy==56 && xx==77)
	{	
	draw_dot(232,7,80-xx);	
	draw_dot(252,7,81-xx);
	draw_dot(232,7,82-xx);
	}
	if(yy==1 && xx==1)
	{	
	draw_dot(23,0,82-xx);	
	draw_dot(63,0,81-xx);
	draw_dot(23,0,80-xx);
	}
	if(yy==1 && xx==77)
	{	
	draw_dot(23,0,80-xx);	
	draw_dot(63,0,81-xx);
	draw_dot(23,0,82-xx);
	}

	if (xx==1 || xx==77)
	{  	if(yy==2)
		{draw_dot(39,(yy+3)/8,81-xx);
		draw_dot(39,(yy+3)/8,82-xx);
		draw_dot(39,(yy+3)/8,80-xx);
		}
		if(yy==3)
		{draw_dot(71,(yy+3)/8,81-xx);
		draw_dot(71,(yy+3)/8,82-xx);
		draw_dot(71,(yy+3)/8,80-xx);
		}
		if(yy==4)
		{draw_dot(135,(yy+3)/8,81-xx);
		draw_dot(135,(yy+3)/8,82-xx);
		draw_dot(135,(yy+3)/8,80-xx);
		}
		if(yy==53)
		{draw_dot(225,(yy+3)/8,81-xx);
		draw_dot(225,(yy+3)/8,82-xx);
		draw_dot(225,(yy+3)/8,80-xx);
		}
		if(yy==54)
		{draw_dot(226,(yy+3)/8,81-xx);
		draw_dot(226,(yy+3)/8,82-xx);
		draw_dot(226,(yy+3)/8,80-xx);
		}
		if(yy==55)
		{draw_dot(228,(yy+3)/8,81-xx);
		draw_dot(228,(yy+3)/8,82-xx);
		draw_dot(228,(yy+3)/8,80-xx);
		}
	}

	if (yy>=5 && yy<=52)
	{
	draw_dot((1<<((yy+3)%8)),(yy+3)/8,81-xx);	
	draw_dot((1<<((yy+3)%8)),(yy+3)/8,82-xx);
	draw_dot((1<<((yy+3)%8)),(yy+3)/8,80-xx);	
	}
}


void draw_map()
{
unsigned char xx;
unsigned char yy;
for(yy=0;yy<=7;yy++)
{	draw_dot(255,yy,0);
	draw_dot(255,yy,1);	
	draw_dot(255,yy,2);	
	draw_dot(255,yy,82);
	draw_dot(255,yy,83);
	draw_dot(255,yy,84);	
}
for(yy=1;yy<=6;yy++)
{	draw_dot(0,yy,3);
	draw_dot(0,yy,4);	
	draw_dot(0,yy,5);	
	draw_dot(0,yy,79);
	draw_dot(0,yy,80);
	draw_dot(0,yy,81);	
}
for(xx=6;xx<=78;xx++)
{	draw_dot(227,7,xx);
	draw_dot(199,0,xx);		
}
draw_dot(7,0,3);
draw_dot(7,0,4);
draw_dot(7,0,5);
draw_dot(7,0,79);
draw_dot(7,0,80);
draw_dot(7,0,81);
draw_dot(224,7,3);
draw_dot(224,7,4);
draw_dot(224,7,5);
draw_dot(224,7,79);
draw_dot(224,7,80);
draw_dot(224,7,81);
for(yy=1;yy<=6;yy++)
{	
	draw_dot(255,yy,6);	
	draw_dot(255,yy,7);	
	draw_dot(255,yy,77);
	draw_dot(255,yy,78);		
}
	draw_dot(207,0,65);		//APs	 9
	draw_dot(207,0,18);		//		 8
	draw_dot(127,0,2);
	draw_dot(251,6,2);
	draw_dot(243,7,18);
	draw_dot(243,7,71);
	draw_dot(239,6,82);
	draw_dot(247,2,82);

}