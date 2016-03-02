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
//函数名：delay(unsigned int s)
//输入：时间
//输出：无
//功能描述：普通廷时,内部用
//*****************************************************************************************		
void delay(unsigned int s)
{
	unsigned int i;
	for(i=0; i<s; i++);
	for(i=0; i<s; i++);
}


void halWait(uint timeout) {	  //16.276us*timeout
    do {
        _nop_();				  //一个机器指令周期，12/11.0592=1.085us
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
//函数名：CpuInit()
//输入：无
//输出：无
//功能描述：SPI初始化程序
/*****************************************************************************************/
void CpuInit()
{
	SpiInit();
	delay(5000);
}
	

//*****************************************************************************************
//函数名：SpisendByte(uchar dat)
//输入：发送的数据
//输出：无
//功能描述：SPI发送一个字节
//*****************************************************************************************
uchar SpiTxRxByte(uchar dat)	   //dat为要传输的8位数据
{
	uchar i,temp;				   //temp存储接收到的8位数据
	temp = 0;
	
	SCK = 0;
	for(i=0; i<8; i++)
	{
		if(dat & 0x80)			   // 1000  0000
		{
			MOSI = 1;
		}
		else MOSI = 0;
		dat <<= 1;				   //dat左移一位(不是循环左移)

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
//函数名：void RESET_CC1101(void)
//输入：无
//输出：无
//功能描述：复位CC1101
//*****************************************************************************************
void RESET_CC1101(void) 
{
	CSN = 0; 
	while (MISO);					//等待MISO变低
    SpiTxRxByte(CC1101_SRES); 		//写入复位命令
	while (MISO); 					//等待MISO变低
    CSN = 1; 
}

//*****************************************************************************************
//函数名：void POWER_UP_RESET_CC1101() 
//输入：无
//输出：无
//功能描述：上电复位CC1101
//*****************************************************************************************
void POWER_UP_RESET_CC1101() 
{
	CSN = 1; 
	halWait(1); 
	CSN = 0; 
	halWait(1); 
	CSN = 1; 
	halWait(41); 
	RESET_CC1101();   		//复位CC1101
}

//*****************************************************************************************
//函数名：void halSpiWriteReg(uchar addr, uchar value)
//输入：地址和配置字
//输出：无
//功能描述：SPI写寄存器
//*****************************************************************************************
void halSpiWriteReg(uchar addr,uchar value) 
{
    CSN = 0;				
    while (MISO);
    SpiTxRxByte(addr);		//写地址
    SpiTxRxByte(value);		//写入配置
    CSN = 1;
}

//*****************************************************************************************
//函数名：void halSpiWriteBurstReg(uchar addr, uchar *buffer,uchar count) 
//输入：地址，写入缓冲区，写入个数
//输出：无
//功能描述：SPI连续写配置寄存器
//*****************************************************************************************
void halSpiWriteBurstReg(uchar addr, uchar *buffer,uchar count) 
{
    uchar i, temp;
	temp = addr | WRITE_BURST;			//WRITE_BURST=0100 0000，将addr的第二位置1，其它位不变
    CSN = 0;
    while (MISO);
    SpiTxRxByte(temp);
    for (i = 0; i < count; i++)
 	{
        SpiTxRxByte(buffer[i]);
    }
    CSN = 1;							  //突发存取必须设置CSN为高终止
}

//*****************************************************************************************
//函数名：void halSpiStrobe(uchar strobe)
//输入：命令
//输出：无
//功能描述：SPI写命令
//*****************************************************************************************
void halSpiStrobe(uchar strobe) 
{
    CSN = 0;
    while (MISO);
    SpiTxRxByte(strobe);		//写入命令
    CSN = 1;
}





//*****************************************************************************************
//函数名：uchar halSpiReadReg(uchar addr)
//输入：地址
//输出：该寄存器的配置字
//功能描述：SPI读寄存器
//*****************************************************************************************
uchar halSpiReadReg(uchar addr) 
{
	uchar temp, value;
    temp = addr|READ_SINGLE;		//读寄存器命令	 READ_SINGLE=1000 0000,将addr的第一位置1，其它位不变
	CSN = 0;
	while (MISO);
	SpiTxRxByte(temp);				//传输地址
	value = SpiTxRxByte(0);			//接收数据，存储在value中
	CSN = 1;
	return value;
}


//*****************************************************************************************
//函数名：void halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count) 
//输入：地址，读出数据后暂存的缓冲区，读出配置个数
//输出：无
//功能描述：SPI连续读配置寄存器
//*****************************************************************************************
void halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count) 
{
    uchar i,temp;
	temp = addr | READ_BURST;		//写入要读的配置寄存器地址和读命令，READ_BURST=1100 0000
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
//函数名：uchar halSpiReadStatus(uchar addr)
//输入：地址
//输出：该状态寄存器当前值
//功能描述：SPI读状态寄存器
//*****************************************************************************************
uchar halSpiReadStatus(uchar addr) 
{
    uchar value,temp;
	temp = addr | READ_BURST;		//写入要读的状态寄存器的地址同时写入读命令，READ_BURST=1100 0000
    CSN = 0;
    while (MISO);
    SpiTxRxByte(temp);
	value = SpiTxRxByte(0);
	CSN = 1;
	return value;
}
//*****************************************************************************************
//函数名：void halRfWriteRfSettings(RF_SETTINGS *pRfSettings)
//输入：无
//输出：无
//功能描述：配置CC1101的寄存器
//*****************************************************************************************
void halRfWriteRfSettings(void) 
{

	halSpiWriteReg(CC1101_FSCTRL0,  rfSettings.FSCTRL2);//自已加的
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
//函数名：void halRfSendPacket(INT8U *txBuffer, INT8U size)
//输入：发送的缓冲区，发送数据个数
//输出：无
//功能描述：CC1100发送一组数据
//*****************************************************************************************

void halRfSendPacket(uchar *txBuffer, uchar size) 		//txBuffer发送的缓冲区，size，发送数据个数
{													
	halSpiWriteReg(CC1101_TXFIFO, size);		
    halSpiWriteBurstReg(CC1101_TXFIFO, txBuffer, size);	//往TX_FIFO寄存器写入要发送的数据
	
	halSpiStrobe(CC1101_SIDLE); 	//测试 ,靠，以前这个语句咋就没有呢，搞了这么久一直是不是异常，都是这家伙
															
    halSpiStrobe(CC1101_STX);		//进入发送模式发送数据	
															
    // Wait for GDO0 to be set -> sync transmitted
    while (!GDO0);
    // Wait for GDO0 to be cleared -> end of packet
    while (GDO0);
											
	halSpiStrobe(CC1101_SFTX);		//CC1101_SFTX=0011 1011		清空TX_FIFO
}


void setRxMode()
{
    halSpiStrobe(CC1101_SRX);		//进入接收状态
}



/*static uchar RfGetRxStatus()
{
	uchar temp, spiRxStatus1,spiRxStatus2;
	uchar i=4;// 循环测试次数
    temp = CC1101_SNOP|READ_SINGLE;//读寄存器命令
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
	uchar i=(*length)*20;  // 具体多少要根据datarate和length来决定		

    halSpiStrobe(CC1101_SRX);		//进入接收状态
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
    if ((halSpiReadStatus(CC1101_RXBYTES) & BYTES_IN_RXFIFO)) 		//如果接的字节数不为0 ,BYTES_IN_RXFIFO=0111 1111
	{
        packetLength = halSpiReadReg(CC1101_RXFIFO);				//读出第一个字节，此字节为该帧数据长度		
        if (packetLength <= *length) 								//如果帧数据长度小于等于用户设置的长度 
		{
			
			halSpiReadBurstReg(CC1101_RXFIFO, rxBuffer, packetLength); //读出所有接收到的数据，存到rxBuffer中
            *length = packetLength;									//把接收数据长度的修改为当前数据的长度
//            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            halSpiReadBurstReg(CC1101_RXFIFO, status, 2); 			//读出CRC校验位和RSSI值
			halSpiStrobe(CC1101_SFRX);								//清洗接收缓冲区
//			RSSI_dB=GetRSSI();											//因为函数返回值可能为负值，所以暂不采用此语句
			RSSI=status[0];											//按照正数传输，将正确值的处理交给中心计算机
//			Dispchar(GetRSSI(),7,120);								//测试用
//			Dispchar(status[1],7,30);
//			Dispchar(status[0],5,30); 								//测试用
//			print_string("信号强度:");
//			Send_int8(GetRSSI());
//			print_string("信号强度2:");
//			Send_int8(status[0]);
//			if(rxBuffer[0]!=6&&rxBuffer[0]!=10)
//				return -1;
            if(status[1] & CRC_OK)
				return 1;						//如果校验成功返回接收成功
			else
				return 0;
        }
		 else 
		{
            *length = packetLength;
            halSpiStrobe(CC1101_SFRX);								//清洗接收缓冲区
            return 0;
        }
    } 
	else
 	return 0;
}

/*****************************************************************************************
//函数名：char GetRSSI()
//输入：无
//输出：带符号的RSSI值
//功能描述：RSSI为2的补码，使之转化成16进制数
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
	uchar m=0;//重发次数
	TMOD = TMOD | 0x01;
//	TH0=0x8A;
//	TL0=0xD0;
			TH0=0xd8;
			TL0=0xf0;
//			TH0=0xc5;
//			TL0=0x68;
		 
	TF0=0;

	txbuffer[0]=address;					 //设定第一个字节为目的节点地址，第三字节为本节点地址
	txbuffer[2]=halSpiReadStatus(CC1101_ADDR);
	halRfSendPacket(txbuffer,size);
		TR0=1;
	while(m)								//括号里若是m，则表示重发m次
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
 