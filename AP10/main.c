#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "IO_def.h"
#include "server.h"

//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   ������С
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
uchar PaTabel[8] = { 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0 }; //10dBm     �������

void main() {
	uchar TxBuf[13] = { 0 }; // 8�ֽ�, �����Ҫ���������ݰ�,����ȷ����
	uchar RxBuf[13] = { 0 };
	uchar RSSI_mem[10] = { 0 };
	BELL = 1; //������
	LED1 = 0; //����
	LED2 = 0;
	UART_init();
	back_lcd = 0; //Һ�����⿪
	lcd_init();
	delay1();
	disdate_a(0x00, 0x00);
	//   entermenu();              /*-----������ʾ  ����ѧ��ӭ���� -----*/
	back_lcd = 1; //Һ�������
	CpuInit();
	POWER_UP_RESET_CC1101();
	halRfWriteRfSettings();
	halSpiWriteBurstReg(CC1101_PATABLE, PaTabel, 8); // CC1101_PATABLE=0011 1111
	BELL = 1;
	LED1 = 1;
	LED2 = 1;
	while (1) {
		if (halRfReceivePacket(RxBuf, &leng)) { //�������ݴ浽RxBuf��
			if (RxBuf[1] != 0xFF && (RxBuf[2] == 9)) { //����Ƿ��ظ��û�����Ϣ
				if (RxBuf[1] == halSpiReadStatus(CC1101_ADDR)) //������ڵ����û��ڵ����
						{
					LED1 = ~LED1;
					SendPacket(RxBuf, 11, 1);
					LED1 = ~LED1;
				} else {
					LED1 = ~LED1;
					SendPacket(RxBuf, 11, 3);
					LED1 = ~LED1;
				}
			}
			//���ж��Ƿ��ظ���Ϣ
			else if (RxBuf[5] == biaohao
					&& ((RxBuf[0] != 0 && bh[RxBuf[3] - 2] == 1)
							|| (RxBuf[0] == 0
									&& bh[halSpiReadStatus(CC1101_ADDR) - 2]))) {
			} else {
				if (RxBuf[0] == 0 && RxBuf[3] == 0xFF) { //�����RSSI�ɼ������еĹ㲥�ź�
					RSSI_mem[s] = RSSI;
					s++;
					if (s >= 1) {
						TxBuf[1] = 0xFF;
						TxBuf[3] = halSpiReadStatus(CC1101_ADDR); //�����ڵ��ַд�룬��ʾ����RSSI�Ľڵ�
						TxBuf[5] = RxBuf[5]; //��Ϣ�ı��

						RSSI_sum = 0; //��RSSI��ֵ
						for (ss = 0; ss < s; ss++) {
							RSSI_sum = RSSI_sum + RSSI_mem[ss];
						}
						TxBuf[4] = RSSI_sum / s;

						TxBuf[6] = RxBuf[6];
						TxBuf[7] = RxBuf[7];
						TxBuf[8] = RxBuf[8];
						TxBuf[9] = RxBuf[9];
						TxBuf[10] = RxBuf[10];

						Dispchar(i % 256, 7, 48);
						Dispchar(RxBuf[5], 5, 48);
						delay_ms(81);
						LED2 = ~LED2;
						SendPacket(TxBuf, 11, 3); //���͸���һ��ַ�Ľڵ� ����ͬ�ڵ�������Ŀ�ĵ�ַ��Ҫ�Ķ���
						LED2 = ~LED2;
						i++;
						bh[halSpiReadStatus(CC1101_ADDR) - 2] = 1;
						s = 0;
						for (s = 0; s < 6; s++) { //�������sһ��
							TxBuf[s] = 128; //�ó�ֵ
						}
						s = 0;
					}
				} else if (RxBuf[2] == 2) { //��������ֻ�������֣����ǹ㲥�źž�����һ�ڵ㷢�����ź�
					LED1 = ~LED1;
					SendPacket(RxBuf, 11, 3); //���͸���һ��ַ�Ľڵ�  ����ͬ�ڵ�������Ŀ�ĵ�ַ��Ҫ�Ķ���
					LED1 = ~LED1;
					Dispchar(i % 256, 5, 48);
					bh[RxBuf[3] - 2] = 1;
				}
				if (biaohao != RxBuf[5]) {
					biaohao = RxBuf[5];
					for (bh_i = 0; bh_i < 9; bh_i++) {
						bh[bh_i] = 0;
					}
				}
			}
		}
	}
}
