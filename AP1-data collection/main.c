#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "DS18B20.h"
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
{		uchar i_signal=0;
		uchar ten;
		uchar xx=0,yy=0;
		uchar ss=1;
xdata	uchar leng=10;
xdata	uchar tt=0;
xdata	uchar TxBuf[6]={0};	 // 8字节, 如果需要更长的数据包,请正确设置
xdata	uchar RxBuf[8]={0};
xdata	uchar biaohao=10;
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

draw_map();
writedigital(22,4,65);
writedigital(23,2,65);
writedigital(16,4,57);
writedigital(16,2,57);
writedigital(21,3,120);
writedigital(21,3,112);
writedigital(16,3,104);
writedigital(20,7,120);
writedigital(16,7,113);

/*for (xx=1;xx<=77;xx++)
{ yy=1;
draw_position(xx,yy);	   delay_ms(100)	;
} 
for (yy=1;yy<=56;yy++)
{ xx=77;
draw_position(xx,yy);		delay_ms(100)	;
}
for (xx=77;xx>=1;xx--)
{ yy=56;
draw_position(xx,yy);	   delay_ms(100)	;
} 
for (yy=56;yy>=1;yy--)
{ xx=1;
draw_position(xx,yy);  delay_ms(100)	;
}	*/

	Init_DS18B20();
//	Adjust_res_18B20(0x3f);	  //0.25
//	ReadTemperature();


	TMOD = TMOD | 0x10;
	TH1=0x3c;
	TL1=0xb0;
	TF1=0;
	TR1=1;

	TxBuf[1]=0xFF;
	TxBuf[3]=0xFF;								 
		biaohao++;													   //第一次数据

		if (KEY1==0)
		{biaohao=biaohao%3;}
		else if (biaohao<10)
		{biaohao=10;}

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
//		writedigital(biaohao%10,7,92);
		Dispchar(biaohao%256,7,103);
		tt=0;

	while(1)
	{	
		//ReadTemperature();
		Init_DS18B20();
		WriteOneChar_18B20(0xCC); // 跳过读序号列号的操作
		WriteOneChar_18B20(0x44); // 启动温度

				//取温度
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
		decimal=decimal1*10+decimal2; //小数

				//显示温度
		ten = integer/10;
		writedigital(ten,1,120);	  // 十位
		ten = integer-ten*10;
		writedigital(ten,1,112);	  // 个位
		writedigital(19,1,104);	  // 小数点
		ten = decimal/10;
		writedigital(ten,1,101);	  // 小数第一位
		ten = decimal-ten*10;
//		writedigital(ten,1,92);	  // 小数第二位
		writeone(1,1,85);         // ℃				


		 if(TF1==1)
		 {	tt++;
		 	if(tt==20)
			{		biaohao++;

		if (KEY1==0)
		{biaohao=biaohao%3;}
		else if (biaohao<10)
		{biaohao=10;}

					TxBuf[5]=biaohao; 			
					LED1=~LED1;
					i_signal=i_signal+1;
					if(i_signal==2) 
					{	writeone(0,6,60);		  //清空		当
						writeone(0,6,44);			//		  前
						writeone(0,6,28);			  //		位
						writeone(0,6,12);				//		  置
						writeone(2,6,52);		//	  	无
						writeone(3,6,36);		  //	信
						writeone(4,6,20);			//	号	
						i_signal=0;
					}
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
//		writedigital(biaohao%10,7,92);
		Dispchar(biaohao%256,7,103);
					tt=0;					
			}
			TH1=0x3c;
			TL1=0xb0;
			TF1=0;			 								 
		 }
		 	
			
			if(ReceivePacket(RxBuf,&leng))		
			{ 			writeone(0,6,52);		//	  	无	   //清空
						writeone(0,6,36);		  //	信
						writeone(0,6,20);			//	号	
						writeone(5,6,60);		  //		当
						writeone(6,6,44);			//		  前
						writeone(7,6,28);			  //		位
						writeone(8,6,12);				//		  置

//			 Dispchar(ss,3,30);					  //测试，用来观察收到几个返回的数据包
			 if(RxBuf[5]==biaohao)											 //返回的第一个数据包，节点2345的RSSI  ，测试用
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,120);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,120);
//				Dispchar(RxBuf[1],1,30);//附近最近节点。
		//	 	Dispchar(RxBuf[3],7,120);
		//		Dispchar(RxBuf[4],5,120);
		//		Dispchar(RxBuf[6],3,120);
		//	 	Dispchar(RxBuf[7],1,120);
				writedigital(RxBuf[1],3,90);//附近最近节点。
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
		//	 	Dispchar(RxBuf[3],7,90);
		//		Dispchar(RxBuf[4],5,90);
		//		Dispchar(RxBuf[6],3,90);
		//	 	Dispchar(RxBuf[7],1,90);
				writedigital(RxBuf[1],3,90);//附近最近节点。

				draw_position(RxBuf[3],RxBuf[6]);
						ten = RxBuf[3]/10;
						writedigital(ten,4,50);	  // 十位
						ten = RxBuf[3]-ten*10;
						writedigital(ten,4,42);	  // 个位
						writedigital(19,4,34);	  // 小数点
						ten = RxBuf[4]/10;
						writedigital(ten,4,31);	  // 小数第一位
						ten = RxBuf[4]-ten*10;
						writedigital(ten,4,22);	  // 小数第二位

						ten = RxBuf[6]/10;
						writedigital(ten,2,50);	  // 十位
						ten = RxBuf[6]-ten*10;
						writedigital(ten,2,42);	  // 个位
						writedigital(19,2,34);	  // 小数点
						ten = RxBuf[7]/10;
						writedigital(ten,2,31);	  // 小数第一位
						ten = RxBuf[7]-ten*10;
						writedigital(ten,2,22);	  // 小数第二位



				ss++;
				LED2=~LED2;


					biaohao++;
		if (KEY1==0)
		{biaohao=biaohao%3;}
		else if (biaohao<10)
		{biaohao=10;}

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
		//		writedigital(biaohao%10,7,92);
				Dispchar(biaohao%256,7,103);
					tt=0;

			 }
/*			 	if(RxBuf[5]==biaohao+2)							 //返回的第三个数据包，上位机发送的位置信息
			 {	LED2=~LED2;
//			 	Dispchar(RxBuf[3],3,40);
//				Dispchar(RxBuf[4],5,40);
//				Dispchar(RxBuf[6],3,40);
//			 	Dispchar(RxBuf[7],1,40);
//				Dispchar(RxBuf[1],1,40);//附近最近节点。
		//	 	Dispchar(RxBuf[3],7,60);
		//		Dispchar(RxBuf[4],5,60);
		//		Dispchar(RxBuf[6],3,60);
		//	 	Dispchar(RxBuf[7],1,60);
		//		Dispchar(RxBuf[1],3,120);//附近最近节点。
				writedigital(RxBuf[1],3,90);
				ss++;
				LED2=~LED2;

//				draw_position()



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
//		writedigital(biaohao%10,7,92);
		Dispchar(biaohao%256,7,103);
					tt=0;					


			 } */
			 }
	}
	}