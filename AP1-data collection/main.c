#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "DS18B20.h"
#include "IO_def.h"
#include "server.h"

//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   ������С
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
xdata uchar PaTabel[8] = { 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0 }; //10dBm     �������

void main() {
	uchar i_signal = 0;
	uchar ten;
	uchar xx = 0, yy = 0;
	uchar ss = 1;
	uchar leng = 10;
	uchar tt = 0;
	uchar TxBuf[6] = { 0 }; // 8�ֽ�, �����Ҫ���������ݰ�,����ȷ����
	uchar RxBuf[8] = { 0 };
	uchar biaohao = 10;
	BELL = 1; //������
	LED1 = 0; //����
	LED2 = 0;
//	UART_init();
	back_lcd = 0; //Һ�����⿪
	lcd_init();
	delay1();
	disdate_a(0x00, 0x00);
//  entermenu();              /*-----������ʾ  ����ѧ��ӭ���� -----*/
//  disdate_a(0x00,0x00);
//  back_lcd=1;	  		//Һ�������
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
		WriteOneChar_18B20(0xCC); // ����������кŵĲ���
		WriteOneChar_18B20(0x44); // �����¶Ȫ�

		//ȡ�¶�
		Init_DS18B20();
		WriteOneChar_18B20(0xCC); //����������кŵĲ���
		WriteOneChar_18B20(0xBE); //��ȡ�¶ȼĴ���

		tempL = ReadOneChar_18B20(); //�����¶ȵĵ�λLSB
		tempH = ReadOneChar_18B20(); //�����¶ȵĸ�λMSB

		if (tempH > 0x7f) //���λΪ1ʱ�¶��Ǹ�
				{
			tempL = ~tempL; //����ת����ȡ����һ
			tempH = ~tempH + 1;
			fg = 0; //��ȡ�¶�Ϊ��ʱfg=0
		}
		integer = tempL / 16 + tempH * 16; //��������
		decimal1 = (tempL & 0x0f) * 10 / 16; //С����һλ
		decimal2 = (tempL & 0x0f) * 100 / 16 % 10; //С���ڶ�λ
		decimal = decimal1 * 10 + decimal2; //С��

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
			writeone(0, 6, 52); //��
			if (RxBuf[5] == biaohao) { //���صĵ�һ�����ݰ����ڵ�2345��RSSI ������
				LED2 = ~LED2;
				writedigital(RxBuf[1], 3, 90); //��������ڵ㡣
				ss++;
				LED2 = ~LED2;
			}
			if (RxBuf[5] == biaohao + 1) { //���صĵڶ������ݰ����ڵ�6789��RSSI ��������
				LED2 = ~LED2;
				writedigital(RxBuf[1], 3, 90); //��������ڵ㡣

				draw_position(RxBuf[3], RxBuf[6]);
				ten = RxBuf[3] / 10;
				writedigital(ten, 4, 50); // ʮλ
				ten = RxBuf[3] - ten * 10;
				writedigital(ten, 4, 42); // ��λ
				writedigital(19, 4, 34); // С����
				ten = RxBuf[4] / 10;
				writedigital(ten, 4, 31); // С����һλ
				ten = RxBuf[4] - ten * 10;
				writedigital(ten, 4, 22); // С���ڶ�λ��
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
