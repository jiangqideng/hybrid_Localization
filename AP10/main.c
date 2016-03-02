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


void main()
{	
uchar IMUs=0;
uchar q=0;
uchar myRSSI;
//uchar RSSI_mem[9]={128,128,128,128,128,128,128,128,128};
	int m=0;
	uchar biaohao=0;
	uchar bh[9]={0};
	uchar bh_i=0;
	uchar leng;
	uchar TxBuf[15]={0};	 // 20字节, 如果需要更长的数据包,请正确设置
	uchar RxBuf[15]={0};
	uchar return_to_user[13]={0};	   
	int i=0,k=0;		 //测试用，计数变量
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
	while (1)									    
	{												
   		if(halRfReceivePacket(RxBuf,&leng))					  //如果能接收到数据，读出数据存到RxBuf中
		{	LED2=~LED2;
			myRSSI=RSSI;


			 if(RxBuf[5]==biaohao  &&  ((RxBuf[0]!=0 &&bh[RxBuf[3]-2]==1) || (RxBuf[0]==0 && bh[8]==1)))	//先判断是否重复信息
			 {}
			 else  if(RxBuf[4]!=0)
			 {
			  		if(RxBuf[5]!=biaohao)
					{	biaohao=RxBuf[5];
 						for(bh_i=0;bh_i<9;bh_i++)
 						{
  						bh[bh_i]=0;
//						RSSI_mem[bh_i]=128;							//将RSSI_mem清为初值
 						} 
					}


				 if(RxBuf[0]==0 && RxBuf[1]==0xff)			 //如果是广播信号
				 {/*	i++;
	//				Dispchar(i%256,7,48);
	//				Dispchar(RxBuf[5],7,98);
					// print_string("***节点");
	//				Send_int8(halSpiReadStatus(CC1101_ADDR));//本节点地址
					//print_string(" 标号");
	//				Send_int8(RxBuf[5]);
					//print_string("  RSSI=");
	//				Send_int8(myRSSI);
	//				print_string("***\n");
	
					Sendchar (halSpiReadStatus(CC1101_ADDR)) ;
					Sendchar (RxBuf[5]) ;
					Sendchar (myRSSI) ;
	
					bh[8]=1;
					RSSI_mem[8]=myRSSI;		*/
				}	   								
		
				else if(RxBuf[2]==9 && RxBuf[0]==10 && RxBuf[1]==0xff)
				{	//k++;
	//				Dispchar(k%256,5,48);
					//print_string("***节点");
	//				Send_int8(RxBuf[3]);
					//print_string(" 标号");
	//				Send_int8(RxBuf[5]);
					//print_string("  RSSI=");
	//				Send_int8(RxBuf[4]);
	//				print_string("***\n");
	
				   	Sendchar (RxBuf[3]) ;
 					Sendchar (RxBuf[4]) ;
					Sendchar (RxBuf[5]) ;
					if (IMUs==0)				   //如果还没把惯导数据传给PC
					{
					Sendchar (0xff);				//标志字节，代表惯导数据来了
				   	Sendchar (RxBuf[6]) ;
 					Sendchar (RxBuf[7]) ;
					Sendchar (RxBuf[8]) ;
 					Sendchar (RxBuf[9]) ;
					Sendchar (RxBuf[10]) ;
					IMUs=1;	
					}
	
					bh[RxBuf[3]-2]=1;
//					RSSI_mem[RxBuf[3]-2]=RxBuf[4];			   //将RSSI信息保存	
				}
						
			}LED2=~LED2;
}
		   if(RI==1)
		  {	  	IMUs=0;				//将标志字节置0
		  		
				if(SBUF!=255)			   
				{	return_to_user[q]=SBUF;
					q++;
				}
				else 					   //代表上位机的定位信息接收完毕。
				{q=0; RI=0;	
				
				TxBuf[1]=return_to_user[0];					 //最近参考节点
				TxBuf[5]=biaohao;			   //表示返回的第一个数据包，此标号与采集的信息标号相同	  							
				TxBuf[3]=return_to_user[1];
				TxBuf[4]=return_to_user[2];
				TxBuf[6]=return_to_user[3];
				TxBuf[7]=return_to_user[4];			
				
//				LED1=~LED1;
//				SendPacket(TxBuf,8,9);
//				LED1=~LED1;

//				delay_ms(100);
				
				TxBuf[1]=return_to_user[0];					 //最近参考节点
				TxBuf[5]=biaohao;			   //表示返回的第二个数据包
				TxBuf[3]=return_to_user[5];
				TxBuf[4]=return_to_user[6];
				TxBuf[6]=return_to_user[7];
				TxBuf[7]=return_to_user[8];		
				  
				LED1=~LED1;

				TxBuf[0]=9;
				TxBuf[2]=10;
				halRfSendPacket(TxBuf,11);
				LED1=~LED1;				
				
//				delay_ms(100);

				TxBuf[1]=return_to_user[0];					 //最近参考节点
				TxBuf[5]=biaohao+2;			   //表示返回的第三个数据包
				TxBuf[3]=return_to_user[9];
				TxBuf[4]=return_to_user[10];
				TxBuf[6]=return_to_user[11];
				TxBuf[7]=return_to_user[12];		
				  
//				LED1=~LED1;
//				SendPacket(TxBuf,8,9);
//				LED1=~LED1;



				q=0;
				}
				RI=0;
		  }		   
}	 
}