#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "IO_def.h"
#include "server.h"

//***************���๦�ʲ������ÿ���ϸ�ο�DATACC1100Ӣ���ĵ��е�48-49ҳ�Ĳ�����******************
//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   ������С
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
xdata uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     �������
uchar RSSI;
extern int j;
/*int RSSI_to_RSSI_dB(uchar rssi)
{	if(rssi>=128)
		return (RSSI-255)/2-74;
	else
		return RSSI/2-74; 
 }	*/

void main()
{
		uchar ss=1;
xdata	uchar leng=10;
xdata	uchar tt=0;
xdata	uchar TxBuf[6]={0};	 // 8�ֽ�, �����Ҫ���������ݰ�,����ȷ����
xdata	uchar RxBuf[8]={0};
xdata	uchar biaohao=0;
    BELL =1;		 //������
	LED1=0;			 //����
	LED2=0;
//	UART_init();
    back_lcd=0; 	  //Һ�����⿪
    lcd_init();
	delay1();
    delay1();
    delay1();
    disdate_a(0x00,0x00);
	delay1();
    delay1();
    delay1();
//  entermenu();              /*-----������ʾ  ����ѧ��ӭ���� -----*/
	delay1();
    delay1();
    delay1();
//    disdate_a(0x00,0x00);
//   back_lcd=1;	  		//Һ�������

	CpuInit();
	POWER_UP_RESET_CC1101();
	halRfWriteRfSettings();
	halSpiWriteBurstReg(CC1101_PATABLE, PaTabel, 8);	// CC1101_PATABLE=0011 1111
	LED1=1;
	//TxBuf[0] = 8 ;
//	TxBuf[1] = 1 ;
//	TxBuf[2] = 1 ;
//	halRfSendPacket(TxBuf,8);	// Transmit Tx buffer data
//	delay_ms(200);
    BELL = 1;	
	LED1=1;
	LED2=1;

	TMOD = TMOD | 0x10;
	TH1=0x3c;
	TL1=0xb0;
	TF1=0;
	TR1=1;

	TxBuf[1]=0xFF;
	TxBuf[3]=0xFF;								 
		biaohao++;													   //��һ������
		TxBuf[5]=biaohao; 			
		LED1=~LED1;
//		SendPacket(TxBuf,8,0);
					{	TxBuf[4]=0x00;
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						TxBuf[4]=0xFF;
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	
					}
		LED1=~LED1;
		Dispchar(biaohao%256,7,30);
		tt=0;

	while(1)
	{	


		 if(TF1==1)
		 {	tt++;
		 	if(tt==20)
			{		biaohao++;
					TxBuf[5]=biaohao; 			
					LED1=~LED1;
//					SendPacket(TxBuf,8,0);
					{	TxBuf[4]=0x00;
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						halRfSendPacket(TxBuf,6);	delay_ms(8);
						TxBuf[4]=0xFF;
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	delay_ms(6);
						halRfSendPacket(TxBuf,6);	
					}
					LED1=~LED1;
					Dispchar(biaohao%256,7,30);
					tt=0;					
			}
			TH1=0x3c;
			TL1=0xb0;
			TF1=0;			 								 
		 }
		 	
			
			if(ReceivePacket(RxBuf,&leng))		
			{ 
			 Dispchar(ss,3,30);					  //���ԣ������۲��յ��������ص����ݰ�
			 if(RxBuf[5]==biaohao)											 //���صĵ�һ�����ݰ����ڵ�2345��RSSI  ��������
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,120);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,120);
//				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
			 	Dispchar(RxBuf[3],7,120);
				Dispchar(RxBuf[4],5,120);
				Dispchar(RxBuf[6],3,120);
			 	Dispchar(RxBuf[7],1,120);
				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
				ss++;
				LED2=~LED2;
			 }
			 if(RxBuf[5]==biaohao+1)										//���صĵڶ������ݰ����ڵ�6789��RSSI ��������
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,80);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,80);
//				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
			 	Dispchar(RxBuf[3],7,90);
				Dispchar(RxBuf[4],5,90);
				Dispchar(RxBuf[6],3,90);
			 	Dispchar(RxBuf[7],1,90);
				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
				ss++;
				LED2=~LED2;
			 }
			 	if(RxBuf[5]==biaohao+2)							 //���صĵ��������ݰ�����λ�����͵�λ����Ϣ
			 {	LED2=~LED2;
//			 	Dispchar(RxBuf[3],3,40);
//				Dispchar(RxBuf[4],5,40);
//				Dispchar(RxBuf[6],3,40);
//			 	Dispchar(RxBuf[7],1,40);
//				Dispchar(RxBuf[1],1,40);//��������ڵ㡣
			 	Dispchar(RxBuf[3],7,60);
				Dispchar(RxBuf[4],5,60);
				Dispchar(RxBuf[6],3,60);
			 	Dispchar(RxBuf[7],1,60);
				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
				ss++;
				LED2=~LED2;



					biaohao++;
					TxBuf[5]=biaohao; 			
					LED1=~LED1;
//					SendPacket(TxBuf,8,0);
					{	TxBuf[4]=0x00;
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						halRfSendPacket(TxBuf,6);	delay_ms(10);
						TxBuf[4]=0xFF;
						halRfSendPacket(TxBuf,6);	
					}
					LED1=~LED1;
					Dispchar(biaohao%256,7,30);
					tt=0;					


			 }
			 }
	}
	}