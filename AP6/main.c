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
uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     功率最大
uchar RSSI;
// char RSSI_dB;
 uint RSSI_sum=0;
// int RSSI_dB_sum=0;

void main()
{	uchar s=0,ss=0;		//用来测RSSI时计数
	int RSSI_sum=0;
	int m=0;
	uchar biaohao=0;
	uchar bh[9]={0};
	uchar bh_i;
	uchar leng;
	uchar TxBuf[13]={0};	 // 8字节, 如果需要更长的数据包,请正确设置
	uchar RxBuf[13]={0};
	uchar RSSI_mem[10]={0};
	int i=0;		 //测试用，计数变量
    BELL =1;		 //不响铃
	LED1=0;			 //开灯
	LED2=0;
	UART_init();
    back_lcd=0; 	  //液晶背光开
    lcd_init();
	delay1();
    delay1();
    delay1();
    disdate_a(0x00,0x00);
	delay1();
    delay1();
    delay1();
 //   entermenu();              /*-----开机显示  天津大学欢迎您！ -----*/
	delay1();
    delay1();
    delay1();
//    disdate_a(0x00,0x00);
    back_lcd=1;	  		//液晶背光关

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
/*	while (1)									    
	{												
   		if(halRfReceivePacket(RxBuf,&leng))					  //读出数据存到RxBuf中
		{

				Send_int8(RxBuf[0]);
			 	Send_int8(RxBuf[1]);
			 	Send_int8(RxBuf[2]);
			 	Send_int8(RxBuf[3]);
			 	Send_int8(RxBuf[4]);
			 	Send_int8(RxBuf[5]);
//			 	Send_int8(RxBuf[6]);
//			 	Send_int8(RxBuf[7]);
//			 	Send_int8(RxBuf[8]);
//			 	Send_int8(RxBuf[9]);
//			 	Send_int8(RxBuf[10]);
				print_string("\n") ;
		 }}				*/


	while (1)									    
	{												
   		if(halRfReceivePacket(RxBuf,&leng))					  //读出数据存到RxBuf中
		{	 

			if(RxBuf[1]!=0xFF && (RxBuf[2]==7))							//如果是返回给用户的信息
			 {	
			 	if(RxBuf[1]==halSpiReadStatus(CC1101_ADDR))			  //如果本节点离用户节点最近
				{	LED1=~LED1;
					SendPacket(RxBuf,11,1);
					LED1=~LED1;
				}
				else 
				{	LED1=~LED1;
					SendPacket(RxBuf,11,5);
					LED1=~LED1;
				}	  
			 }


			/*先判断是否重复信息*/
			 
			else if(RxBuf[5]==biaohao  &&  ((RxBuf[0]!=0 &&bh[RxBuf[3]-2]==1) || (RxBuf[0]==0 && bh[halSpiReadStatus(CC1101_ADDR)-2])))
			 {	}
			 	else
			 	{

				if(RxBuf[0]==0 && 	RxBuf[3]==0xFF)			 //如果是RSSI采集过程中的广播信号
				{ 	RSSI_mem[s]=RSSI; s++  ;
//					if(RxBuf[4]==0xFF || s==10)	
					if( s>=1)
					{	TxBuf[1]=0xFF;
						TxBuf[3]=halSpiReadStatus(CC1101_ADDR);	  //将本节点地址写入，表示产生RSSI的节点
						
						TxBuf[5]=RxBuf[5];						  //信息的标号

						RSSI_sum=0;								   //求RSSI均值
						for(ss=0;ss<s;ss++)
						{RSSI_sum=RSSI_sum+RSSI_mem[ss];}
						TxBuf[4]=RSSI_sum/s;

 //新加：
						TxBuf[6]=RxBuf[6];
						TxBuf[7]=RxBuf[7];
						TxBuf[8]=RxBuf[8];
						TxBuf[9]=RxBuf[9];
						TxBuf[10]=RxBuf[10];
 //

						Dispchar(i%256,7,48);
						Dispchar(RxBuf[5],5,48);
						delay_ms(18);
						LED2=~LED2;
//改动
						SendPacket(TxBuf,11,7);					  //发送给下一地址的节点 （不同节点在这里目的地址需要改动）
						LED2=~LED2;
						i++;
						bh[halSpiReadStatus(CC1101_ADDR)-2]=1;
						s=0;
						for(s=0;s<6;s++)				 //这里借用s一下
 						{	TxBuf[s]=128;				 //置初值
 						}
						s=0;

					}
					
				}

				else if(RxBuf[2]==5)					//本网络中只有这两种，不是广播信号就上上一节点发来的信号
				{
				LED1=~LED1;	
//改动
				SendPacket(RxBuf,11,7);					  //发送给下一地址的节点  （不同节点在这里目的地址需要改动）
				LED1=~LED1;
				Dispchar(i%256,5,48); 
				bh[RxBuf[3]-2]=1;
				}


				if(biaohao!=RxBuf[5])
				{	biaohao=RxBuf[5];
 					for(bh_i=0;bh_i<9;bh_i++)
 					{
  					bh[bh_i]=0;
					}
				}
			}			
		}
	}
}