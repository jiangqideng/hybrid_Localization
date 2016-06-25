#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "DS18B20.h"
#include "IO_def.h"
#include "server.h"

//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   功率最小
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
xdata uchar PaTabel[8] = { 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0 }; //10dBm     功率最大

void main() {
	uchar i_signal = 0;
	uchar ten;
	uchar xx = 0, yy = 0;
	uchar ss = 1;
	uchar leng = 10;
	uchar tt = 0;
	uchar TxBuf[6] = { 0 }; // 8字节, 如果需要更长的数据包,请正确设置
	uchar RxBuf[8] = { 0 };
	uchar biaohao = 10;
	BELL = 1; //不响铃
	LED1 = 0; //开灯
	LED2 = 0;
//	UART_init();
	back_lcd = 0; //液晶背光开
	lcd_init();
	delay1();
	disdate_a(0x00, 0x00);
//  entermenu();              /*-----开机显示  天津大学欢迎您！ -----*/
//  disdate_a(0x00,0x00);
//  back_lcd=1;	  		//液晶背光关
	CpuInit();
	POWER_UP_RESET_CC1101();
	halRfWriteRfSettings();
	halSpiWriteBurstReg(CC1101_PATABLE, PaTabel, 8);
	BELL = 1;
	LED2 = 1;

	draw_map();

	Init_DS18B20();

	TMOD = TMOD | 0x10;
	TH1 = 0x3c;
	TL1 = 0xb0;
	TF1 = 0;
	TR1 = 1;
	
	TxBuf[1] = 0xFF;
	TxBuf[3] = 0xFF;
	TxBuf[5] = biaohao;
	LED1 = ~LED1;
	LED1 = ~LED1;
//	writedigital(biaohao%10,7,92);
	Dispchar(biaohao % 256, 7, 103);
	tt = 0;

	while (1) {
		//ReadTemperature();
		Init_DS18B20();
		WriteOneChar_18B20(0xCC); // 跳过读序号列号的操作
		WriteOneChar_18B20(0x44); // 启动温度

		//取温度
		Init_DS18B20();
		WriteOneChar_18B20(0xCC); //跳过读序号列号的操作
		WriteOneChar_18B20(0xBE); //读取温度寄存器

		tempL = ReadOneChar_18B20(); //读出温度的低位LSB
		tempH = ReadOneChar_18B20(); //读出温度的高位MSB

		if (tempH > 0x7f) //最高位为1时温度是负
				{
			tempL = ~tempL; //补码转换，取反加一
			tempH = ~tempH + 1;
			fg = 0; //读取温度为负时fg=0
		}
		integer = tempL / 16 + tempH * 16; //整数部分
		decimal1 = (tempL & 0x0f) * 10 / 16; //小数第一位
		decimal2 = (tempL & 0x0f) * 100 / 16 % 10; //小数第二位
		decimal = decimal1 * 10 + decimal2; //小数

		if (TF1 == 1) {
			tt++;
			if (tt == 20) {
				biaohao++;
				if (KEY1 == 0) {
					biaohao = biaohao % 3;
				} else if (biaohao < 10) {
					biaohao = 10;
				}

				TxBuf[5] = biaohao;
				LED1 = ~LED1;
				i_signal = i_signal + 1;
				LED1 = ~LED1;
				Dispchar(biaohao % 256, 7, 103);
				tt = 0;
			}
			TH1 = 0x3c;
			TL1 = 0xb0;
			TF1 = 0;
		}

		if (ReceivePacket(RxBuf, &leng)) {
			writeone(0, 6, 52); //无
			if (RxBuf[5] == biaohao) { //返回的第一个数据包，节点2345的RSSI 测试用
				LED2 = ~LED2;
				writedigital(RxBuf[1], 3, 90); //附近最近节点。
				ss++;
				LED2 = ~LED2;
			}
			if (RxBuf[5] == biaohao + 1) { //返回的第二个数据包。节点6789的RSSI ，测试用
				LED2 = ~LED2;
				writedigital(RxBuf[1], 3, 90); //附近最近节点。

				draw_position(RxBuf[3], RxBuf[6]);
				ten = RxBuf[3] / 10;
				writedigital(ten, 4, 50); // 十位
				ten = RxBuf[3] - ten * 10;
				writedigital(ten, 4, 42); // 个位
				writedigital(19, 4, 34); // 小数点
				ten = RxBuf[4] / 10;
				writedigital(ten, 4, 31); // 小数第一位
				ten = RxBuf[4] - ten * 10;
				writedigital(ten, 4, 22); // 小数第二位
				ss++;
				LED2 = ~LED2;
				biaohao++;
				if (KEY1 == 0) {
					biaohao = biaohao % 3;
				} else if (biaohao < 10) {
					biaohao = 10;
				}
				TxBuf[5] = biaohao;
				LED1 = ~LED1;
				TxBuf[4] = 0x00;
				
				TxBuf[4] = 0xFF;
				halRfSendPacket(TxBuf, 6);
				LED1 = ~LED1;
				//writedigital(biaohao%10,7,92);
				Dispchar(biaohao % 256, 7, 103);
				tt = 0;
			}
		}
	}
}
