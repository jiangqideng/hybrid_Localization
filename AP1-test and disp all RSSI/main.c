#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "IO_def.h"
#include "server.h"

//***************更多功率参数设置可详细参考DATACC1100英文文档中第48-49页的参数表******************
//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   功率最小
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
xdata uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     功率最大
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
xdata	uchar TxBuf[6]={0};	 // 8字节, 如果需要更长的数据包,请正确设置
xdata	uchar RxBuf[8]={0};
xdata	uchar biaohao=0;
    BELL =1;		 //不响铃
	LED1=0;			 //开灯
	LED2=0;
//	UART_init();
    back_lcd=0; 	  //液晶背光开
    lcd_init();
	delay1();
    delay1();
    delay1();
    disdate_a(0x00,0x00);
	delay1();
    delay1();
    delay1();
//  entermenu();              /*-----开机显示  天津大学欢迎您！ -----*/
	delay1();
    delay1();
    delay1();
//    disdate_a(0x00,0x00);
//   back_lcd=1;	  		//液晶背光关

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
		biaohao++;													   //第一次数据
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
			 Dispchar(ss,3,30);					  //测试，用来观察收到几个返回的数据包
			 if(RxBuf[5]==biaohao)											 //返回的第一个数据包，节点2345的RSSI  ，测试用
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,120);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,120);
//				Dispchar(RxBuf[1],1,30);//附近最近节点。
			 	Dispchar(RxBuf[3],7,120);
				Dispchar(RxBuf[4],5,120);
				Dispchar(RxBuf[6],3,120);
			 	Dispchar(RxBuf[7],1,120);
				Dispchar(RxBuf[1],1,30);//附近最近节点。
				ss++;
				LED2=~LED2;
			 }
			 if(RxBuf[5]==biaohao+1)										//返回的第二个数据包。节点6789的RSSI ，测试用
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,80);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,80);
//				Dispchar(RxBuf[1],1,30);//附近最近节点。
			 	Dispchar(RxBuf[3],7,90);
				Dispchar(RxBuf[4],5,90);
				Dispchar(RxBuf[6],3,90);
			 	Dispchar(RxBuf[7],1,90);
				Dispchar(RxBuf[1],1,30);//附近最近节点。
				ss++;
				LED2=~LED2;
			 }
			 	if(RxBuf[5]==biaohao+2)							 //返回的第三个数据包，上位机发送的位置信息
			 {	LED2=~LED2;
//			 	Dispchar(RxBuf[3],3,40);
//				Dispchar(RxBuf[4],5,40);
//				Dispchar(RxBuf[6],3,40);
//			 	Dispchar(RxBuf[7],1,40);
//				Dispchar(RxBuf[1],1,40);//附近最近节点。
			 	Dispchar(RxBuf[3],7,60);
				Dispchar(RxBuf[4],5,60);
				Dispchar(RxBuf[6],3,60);
			 	Dispchar(RxBuf[7],1,60);
				Dispchar(RxBuf[1],1,30);//附近最近节点。
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