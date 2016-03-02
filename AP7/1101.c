#include <reg52.h>
#include <intrins.h>
#include "cc1101.h"
#include "IO_def.h"
#include "lcd12864a.h"
#include "server.h"
extern uchar RSSI;
extern char RSSI_dB;
extern int j=0;
/////////////////////////////////////////////////////////////////
const RF_SETTINGS rfSettings = 
{
	0x00,
    0x08,   // FSCTRL1   Frequency synthesizer control.
    0x00,   // FSCTRL0   Frequency synthesizer control.
    0x10,   // FREQ2     Frequency control word, high byte.		 433Mhz
    0xA7,   // FREQ1     Frequency control word, middle byte.
    0x62,   // FREQ0     Frequency control word, low byte.
    0x5B,   // MDMCFG4   Modem configuration.
    0xF8,   // MDMCFG3   Modem configuration.
    0x03,   // MDMCFG2   Modem configuration.
    0x22,   // MDMCFG1   Modem configuration.
    0xF8,   // MDMCFG0   Modem configuration.

    0x00,   // CHANNR    Channel number.
    0x47,   // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
    0xB6,   // FREND1    Front end RX configuration.
    0x10,   // FREND0    Front end RX configuration.
    0x18,   // MCSM0     Main Radio Control State Machine configuration.
    0x1D,   // FOCCFG    Frequency Offset Compensation Configuration.
    0x1C,   // BSCFG     Bit synchronization Configuration.
    0xC7,   // AGCCTRL2  AGC control.
    0x00,   // AGCCTRL1  AGC control.
    0xB2,   // AGCCTRL0  AGC control.

    0xEA,   // FSCAL3    Frequency synthesizer calibration.
    0x2A,   // FSCAL2    Frequency synthesizer calibration.
    0x00,   // FSCAL1    Frequency synthesizer calibration.
    0x11,   // FSCAL0    Frequency synthesizer calibration.
    0x59,   // FSTEST    Frequency synthesizer calibration.
    0x81,   // TEST2     Various test settings.
    0x35,   // TEST1     Various test settings.
    0x09,   // TEST0     Various test settings.
    0x0B,   // IOCFG2    GDO2 output pin configuration.
    0x06,   // IOCFG0D   GDO0 output pin configuration. Refer to SmartRF?Studio User Manual for detailed pseudo register explanation.

    0x06,   // PKTCTRL1  Packet automation control.
    0x05,   // PKTCTRL0  Packet automation control.
    0x07,   // ADDR      Device address.
    0xFF,   // PKTLEN    Packet length.
};

//*****************************************************************************************
//��������delay(unsigned int s)
//���룺ʱ��
//�������
//������������ͨ͢ʱ,�ڲ���
//*****************************************************************************************		
void delay(unsigned int s)
{
	unsigned int i;
	for(i=0; i<s; i++);
	for(i=0; i<s; i++);
}


void halWait(uint timeout) {	  //16.276us*timeout
    do {
        _nop_();				  //һ������ָ�����ڣ�12/11.0592=1.085us
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_(); 
    } while (--timeout);
}


void SpiInit()
{
	CSN=0;
	SCK=0;
	CSN=1;
}

/*****************************************************************************************
//��������CpuInit()
//���룺��
//�������
//����������SPI��ʼ������
/*****************************************************************************************/
void CpuInit()
{
	SpiInit();
	delay(5000);
}
	

//*****************************************************************************************
//��������SpisendByte(uchar dat)
//���룺���͵�����
//�������
//����������SPI����һ���ֽ�
//*****************************************************************************************
uchar SpiTxRxByte(uchar dat)	   //datΪҪ�����8λ����
{
	uchar i,temp;				   //temp�洢���յ���8λ����
	temp = 0;
	
	SCK = 0;
	for(i=0; i<8; i++)
	{
		if(dat & 0x80)			   // 1000  0000
		{
			MOSI = 1;
		}
		else MOSI = 0;
		dat <<= 1;				   //dat����һλ(����ѭ������)

		SCK = 1; 
		_nop_();
		_nop_();

		temp <<= 1;
		if(MISO)temp++; 
		SCK = 0;
		_nop_();
		_nop_();	
	}
	return temp;
}

//*****************************************************************************************
//��������void RESET_CC1101(void)
//���룺��
//�������
//������������λCC1101
//*****************************************************************************************
void RESET_CC1101(void) 
{
	CSN = 0; 
	while (MISO);					//�ȴ�MISO���
    SpiTxRxByte(CC1101_SRES); 		//д�븴λ����
	while (MISO); 					//�ȴ�MISO���
    CSN = 1; 
}

//*****************************************************************************************
//��������void POWER_UP_RESET_CC1101() 
//���룺��
//�������
//�����������ϵ縴λCC1101
//*****************************************************************************************
void POWER_UP_RESET_CC1101() 
{
	CSN = 1; 
	halWait(1); 
	CSN = 0; 
	halWait(1); 
	CSN = 1; 
	halWait(41); 
	RESET_CC1101();   		//��λCC1101
}

//*****************************************************************************************
//��������void halSpiWriteReg(uchar addr, uchar value)
//���룺��ַ��������
//�������
//����������SPIд�Ĵ���
//*****************************************************************************************
void halSpiWriteReg(uchar addr,uchar value) 
{
    CSN = 0;				
    while (MISO);
    SpiTxRxByte(addr);		//д��ַ
    SpiTxRxByte(value);		//д������
    CSN = 1;
}

//*****************************************************************************************
//��������void halSpiWriteBurstReg(uchar addr, uchar *buffer,uchar count) 
//���룺��ַ��д�뻺������д�����
//�������
//����������SPI����д���üĴ���
//*****************************************************************************************
void halSpiWriteBurstReg(uchar addr, uchar *buffer,uchar count) 
{
    uchar i, temp;
	temp = addr | WRITE_BURST;			//WRITE_BURST=0100 0000����addr�ĵڶ�λ��1������λ����
    CSN = 0;
    while (MISO);
    SpiTxRxByte(temp);
    for (i = 0; i < count; i++)
 	{
        SpiTxRxByte(buffer[i]);
    }
    CSN = 1;							  //ͻ����ȡ��������CSNΪ����ֹ
}

//*****************************************************************************************
//��������void halSpiStrobe(uchar strobe)
//���룺����
//�������
//����������SPIд����
//*****************************************************************************************
void halSpiStrobe(uchar strobe) 
{
    CSN = 0;
    while (MISO);
    SpiTxRxByte(strobe);		//д������
    CSN = 1;
}





//*****************************************************************************************
//��������uchar halSpiReadReg(uchar addr)
//���룺��ַ
//������üĴ�����������
//����������SPI���Ĵ���
//*****************************************************************************************
uchar halSpiReadReg(uchar addr) 
{
	uchar temp, value;
    temp = addr|READ_SINGLE;		//���Ĵ�������	 READ_SINGLE=1000 0000,��addr�ĵ�һλ��1������λ����
	CSN = 0;
	while (MISO);
	SpiTxRxByte(temp);				//�����ַ
	value = SpiTxRxByte(0);			//�������ݣ��洢��value��
	CSN = 1;
	return value;
}


//*****************************************************************************************
//��������void halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count) 
//���룺��ַ���������ݺ��ݴ�Ļ��������������ø���
//�������
//����������SPI���������üĴ���
//*****************************************************************************************
void halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count) 
{
    uchar i,temp;
	temp = addr | READ_BURST;		//д��Ҫ�������üĴ�����ַ�Ͷ����READ_BURST=1100 0000
    CSN = 0;
    while (MISO);
	SpiTxRxByte(temp);   
    for (i = 0; i < count; i++) 
	{
        buffer[i] = SpiTxRxByte(0);
    }
    CSN = 1;
}


//*****************************************************************************************
//��������uchar halSpiReadStatus(uchar addr)
//���룺��ַ
//�������״̬�Ĵ�����ǰֵ
//����������SPI��״̬�Ĵ���
//*****************************************************************************************
uchar halSpiReadStatus(uchar addr) 
{
    uchar value,temp;
	temp = addr | READ_BURST;		//д��Ҫ����״̬�Ĵ����ĵ�ַͬʱд������READ_BURST=1100 0000
    CSN = 0;
    while (MISO);
    SpiTxRxByte(temp);
	value = SpiTxRxByte(0);
	CSN = 1;
	return value;
}
//*****************************************************************************************
//��������void halRfWriteRfSettings(RF_SETTINGS *pRfSettings)
//���룺��
//�������
//��������������CC1101�ļĴ���
//*****************************************************************************************
void halRfWriteRfSettings(void) 
{

	halSpiWriteReg(CC1101_FSCTRL0,  rfSettings.FSCTRL2);//���Ѽӵ�
    // Write register settings
    halSpiWriteReg(CC1101_FSCTRL1,  rfSettings.FSCTRL1);
    halSpiWriteReg(CC1101_FSCTRL0,  rfSettings.FSCTRL0);
    halSpiWriteReg(CC1101_FREQ2,    rfSettings.FREQ2);
    halSpiWriteReg(CC1101_FREQ1,    rfSettings.FREQ1);
    halSpiWriteReg(CC1101_FREQ0,    rfSettings.FREQ0);
    halSpiWriteReg(CC1101_MDMCFG4,  rfSettings.MDMCFG4);
    halSpiWriteReg(CC1101_MDMCFG3,  rfSettings.MDMCFG3);
    halSpiWriteReg(CC1101_MDMCFG2,  rfSettings.MDMCFG2);
    halSpiWriteReg(CC1101_MDMCFG1,  rfSettings.MDMCFG1);
    halSpiWriteReg(CC1101_MDMCFG0,  rfSettings.MDMCFG0);
    halSpiWriteReg(CC1101_CHANNR,   rfSettings.CHANNR);
    halSpiWriteReg(CC1101_DEVIATN,  rfSettings.DEVIATN);
    halSpiWriteReg(CC1101_FREND1,   rfSettings.FREND1);
    halSpiWriteReg(CC1101_FREND0,   rfSettings.FREND0);
    halSpiWriteReg(CC1101_MCSM0 ,   rfSettings.MCSM0 );
    halSpiWriteReg(CC1101_FOCCFG,   rfSettings.FOCCFG);
    halSpiWriteReg(CC1101_BSCFG,    rfSettings.BSCFG);
    halSpiWriteReg(CC1101_AGCCTRL2, rfSettings.AGCCTRL2);
	halSpiWriteReg(CC1101_AGCCTRL1, rfSettings.AGCCTRL1);
    halSpiWriteReg(CC1101_AGCCTRL0, rfSettings.AGCCTRL0);
    halSpiWriteReg(CC1101_FSCAL3,   rfSettings.FSCAL3);
	halSpiWriteReg(CC1101_FSCAL2,   rfSettings.FSCAL2);
	halSpiWriteReg(CC1101_FSCAL1,   rfSettings.FSCAL1);
    halSpiWriteReg(CC1101_FSCAL0,   rfSettings.FSCAL0);
    halSpiWriteReg(CC1101_FSTEST,   rfSettings.FSTEST);
    halSpiWriteReg(CC1101_TEST2,    rfSettings.TEST2);
    halSpiWriteReg(CC1101_TEST1,    rfSettings.TEST1);
    halSpiWriteReg(CC1101_TEST0,    rfSettings.TEST0);
    halSpiWriteReg(CC1101_IOCFG2,   rfSettings.IOCFG2);
    halSpiWriteReg(CC1101_IOCFG0,   rfSettings.IOCFG0);    
    halSpiWriteReg(CC1101_PKTCTRL1, rfSettings.PKTCTRL1);
    halSpiWriteReg(CC1101_PKTCTRL0, rfSettings.PKTCTRL0);
    halSpiWriteReg(CC1101_ADDR,     rfSettings.ADDR);
    halSpiWriteReg(CC1101_PKTLEN,   rfSettings.PKTLEN);
}

//*****************************************************************************************
//��������void halRfSendPacket(INT8U *txBuffer, INT8U size)
//���룺���͵Ļ��������������ݸ���
//�������
//����������CC1100����һ������
//*****************************************************************************************

void halRfSendPacket(uchar *txBuffer, uchar size) 		//txBuffer���͵Ļ�������size���������ݸ���
{													
	halSpiWriteReg(CC1101_TXFIFO, size);		
    halSpiWriteBurstReg(CC1101_TXFIFO, txBuffer, size);	//��TX_FIFO�Ĵ���д��Ҫ���͵�����
	
	halSpiStrobe(CC1101_SIDLE); 	//���� ,������ǰ������զ��û���أ�������ô��һֱ�ǲ����쳣��������һ�
															
    halSpiStrobe(CC1101_STX);		//���뷢��ģʽ��������	
															
    // Wait for GDO0 to be set -> sync transmitted
    while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while (GDO0);
											
	halSpiStrobe(CC1101_SFTX);		//CC1101_SFTX=0011 1011		���TX_FIFO
}


void setRxMode()
{
    halSpiStrobe(CC1101_SRX);		//�������״̬
}



/*static uchar RfGetRxStatus()
{
	uchar temp, spiRxStatus1,spiRxStatus2;
	uchar i=4;// ѭ�����Դ���
    temp = CC1101_SNOP|READ_SINGLE;//���Ĵ�������
	CSN = 0;
	while (MISO);
	SpiTxRxByte(temp);
	spiRxStatus1 = SpiTxRxByte(0);
	do
	{
		SpiTxRxByte(temp);
		spiRxStatus2 = SpiTxRxByte(0);
		if(spiRxStatus1 == spiRxStatus2)
		{
			if( (spiRxStatus1 & CC1101_STATE_BM) == CC1101_STATE_RX_OVERFLOW_BM)
			{
               halSpiStrobe(CC1101_SFRX);
			   return 0;
			}
		    return 1;
		}
		spiRxStatus1=spiRxStatus2;
	}
	while(i--);
	CSN = 1;
    return 0;	
}  */

char halRfReceivePacket(uchar *rxBuffer, uchar *length) 
{
    uchar status[2];
    uchar packetLength;
	uchar i=(*length)*20;  // �������Ҫ����datarate��length������		

    halSpiStrobe(CC1101_SRX);		//�������״̬
	//delay(5);
    //while (!GDO1);
    //while (GDO1);
	delay(2);
	while (GDO0)
	{
		delay(2);
		--i;
		if(i<1)
		   return -1; 	    
	}	 
    if ((halSpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)) 		//����ӵ��ֽ�����Ϊ0 ,BYTES_IN_RXFIFO=0111 1111
	{
        packetLength = halSpiReadReg(CC1101_RXFIFO);				//������һ���ֽڣ����ֽ�Ϊ��֡���ݳ���		
        if (packetLength <= *length) 								//���֡���ݳ���С�ڵ����û����õĳ��� 
		{
			
			halSpiReadBurstReg(CC1101_RXFIFO, rxBuffer, packetLength); //�������н��յ������ݣ��浽rxBuffer��
            *length = packetLength;									//�ѽ������ݳ��ȵ��޸�Ϊ��ǰ���ݵĳ���
//            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            halSpiReadBurstReg(CC1101_RXFIFO, status, 2); 			//����CRCУ��λ��RSSIֵ
			halSpiStrobe(CC1101_SFRX);								//��ϴ���ջ�����
//			RSSI_dB=GetRSSI();											//��Ϊ��������ֵ����Ϊ��ֵ�������ݲ����ô����
			RSSI=status[0];											//�����������䣬����ȷֵ�Ĵ��������ļ����
//			Dispchar(GetRSSI(),7,120);								//������
//			Dispchar(status[1],7,30);
//			Dispchar(status[0],5,30); 								//������
//			print_string("�ź�ǿ��:");
//			Send_int8(GetRSSI());
//			print_string("�ź�ǿ��2:");
//			Send_int8(status[0]);
//			if(rxBuffer[0]!=6&&rxBuffer[0]!=10)
//				return -1;
            if(status[1] & CRC_OK)
				return 1;						//���У��ɹ����ؽ��ճɹ�
			else
				return 0;
        }
		 else 
		{
            *length = packetLength;
            halSpiStrobe(CC1101_SFRX);								//��ϴ���ջ�����
            return 0;
        }
    } 
	else
 	return 0;
}

/*****************************************************************************************
//��������char GetRSSI()
//���룺��
//����������ŵ�RSSIֵ
//����������RSSIΪ2�Ĳ��룬ʹ֮ת����16������
*****************************************************************************************/
int GetRSSI()
{
	uchar RSSI;
	char  RSSI_dB;
	RSSI=halSpiReadStatus(CC1101_RSSI);
	if(RSSI>=128)
		RSSI_dB=(RSSI-255)/2-74;
	else
		RSSI_dB=RSSI/2-74;
	return RSSI_dB;
}

uchar SendPacket(uchar *txbuffer,uchar size,uchar address)
{	
	xdata uchar rxbuffer[8]={0};
	uchar length=8;
	uchar m=0;//�ط�����
	TMOD = TMOD | 0x01;
//	TH0=0x8A;
//	TL0=0xD0;
			TH0=0xd8;
			TL0=0xf0;
//			TH0=0xc5;
//			TL0=0x68;
		 
	TF0=0;

	txbuffer[0]=address;					 //�趨��һ���ֽ�ΪĿ�Ľڵ��ַ�������ֽ�Ϊ���ڵ��ַ
	txbuffer[2]=halSpiReadStatus(CC1101_ADDR);
	halRfSendPacket(txbuffer,size);
		TR0=1;
	while(m)								//����������m�����ʾ�ط�m��
	{
		if(halRfReceivePacket(rxbuffer,&length))
		{
			if(rxbuffer[1]==0x00)										
				return 1;

		}
		if(TF0==1)
		{	j++;
			m--;
			Dispchar(j,7,98);
			halRfSendPacket(txbuffer,size);
			TH0=0xd8;
			TL0=0xf0;
//			TH0=0xc5;
//			TL0=0x68;
			  
			TF0=0;
		}		
	}
	return 0;	
}
  
uchar ReceivePacket(uchar *rxbuffer,uchar *length)
{
	xdata uchar txbuffer[8]={0};
	uchar size=3;
	if(halRfReceivePacket(rxbuffer,length))
	{	txbuffer[0]=rxbuffer[2];
		txbuffer[1]=0x00;				  
		txbuffer[2]=halSpiReadStatus(CC1101_ADDR);
		if(rxbuffer[0]!=0)
		{halRfSendPacket(txbuffer,size); }
		return 1;
	}
	else
	return 0;
}
 