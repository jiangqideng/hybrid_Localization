							  
#define		uchar 	unsigned char
#define 	uint 	unsigned int 

#define 	WRITE_BURST 		0x40							//����д��
#define 	READ_SINGLE			0x80							//��
#define 	READ_BURST			0xC0							//������
#define		BYTES_IN_RXFIFO		0x7F							//���ջ���������Ч�ֽ���
#define 	CRC_OK				0x80							//CRCУ��ͨ��λ��־



//******************************************************************************************
void 	SpiInit();													 //SPI��ʼ������
void 	CpuInit();													 //
void 	RESET_CC1101();											 	 //��λCC1101
void 	POWER_UP_RESET_CC1101();									 //�ϵ縴λCC1101
void 	halSpiWriteReg(uchar addr,uchar value);					 	 //SPIд�Ĵ���
void 	halSpiWriteBurstReg(uchar addr,uchar *buffer,uchar count);	 //SPI����д���üĴ���
void 	halSpiStrobe(uchar strobe);									 //SPIд����
uchar	halSpiReadReg(uchar addr);									 //SPI���Ĵ���
void	halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count); 	 //SPI���������üĴ���
uchar	halSpiReadStatus(uchar addr);								 //SPI��״̬�Ĵ���
void	halRfWriteRfSettings();										 //����CC1101�ļĴ���
void	halRfSendPacket(uchar *txbuffer,uchar size);				 //CC1101����һ������
char	halRfReceivePacket(uchar *rxbuffer,uchar *length);			 //CC1101����һ������
void 	halWait(uint timeout);										 //
uchar 	SpiTxRxByte(uchar dat);										 //
void 	setRxMode() ;												 //���ý���״̬

int 	GetRSSI();

static uchar RfGetRxStatus();

//�Լ�����Ŀɿ�������ɿ����յĺ���
uchar SendPacket(uchar *txbuffer,uchar size,uchar address);
uchar ReceivePacket(uchar *rxbuffer,uchar *length);
//******************************************************************************************

// CC1101 STROBE, CONTROL AND STATUS REGSITER	����CC1101״̬�����ƺ�״̬�Ĵ���
#define CC1101_IOCFG2       0x00        // GDO2 output pin configuration	 GDO2�����������
#define CC1101_IOCFG1       0x01        // GDO1 output pin configuration	 GDO1�����������
#define CC1101_IOCFG0       0x02        // GDO0 output pin configuration	 GDO0�����������
#define CC1101_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds	 RX FIFO��TX FIFO��ֵ
#define CC1101_SYNC1        0x04        // Sync word, high uchar			 ͬ���֣���8λ
#define CC1101_SYNC0        0x05        // Sync word, low uchar				 ͬ���֣���8λ
#define CC1101_PKTLEN       0x06        // Packet length					 ���ݰ�����
#define CC1101_PKTCTRL1     0x07        // Packet automation control		 ���ݰ��Զ�����
#define CC1101_PKTCTRL0     0x08        // Packet automation control
#define CC1101_ADDR         0x09        // Device address					 ������ַ
#define CC1101_CHANNR       0x0A        // Channel number					 �ŵ�����
#define CC1101_FSCTRL1      0x0B        // Frequency synthesizer control		   Ƶ�ʺϳ�������
#define CC1101_FSCTRL0      0x0C        // Frequency synthesizer control
#define CC1101_FREQ2        0x0D        // Frequency control word, high uchar	   Ƶ�ʿ����֣�����ͨ��Ƶ�Σ�
#define CC1101_FREQ1        0x0E        // Frequency control word, middle uchar
#define CC1101_FREQ0        0x0F        // Frequency control word, low uchar
#define CC1101_MDMCFG4      0x10        // Modem configuration					   ���ƽ��������
#define CC1101_MDMCFG3      0x11        // Modem configuration
#define CC1101_MDMCFG2      0x12        // Modem configuration
#define CC1101_MDMCFG1      0x13        // Modem configuration
#define CC1101_MDMCFG0      0x14        // Modem configuration
#define CC1101_DEVIATN      0x15        // Modem deviation setting				   ���ƽ����ƫ������
#define CC1101_MCSM2        0x16        // Main Radio Control State Machine configuration  �����߿���״̬������
#define CC1101_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CC1101_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CC1101_FOCCFG       0x19        // Frequency Offset Compensation configuration		Ƶ��ƫ�Ʋ�������
#define CC1101_BSCFG        0x1A        // Bit Synchronization configuration				λͬ������
#define CC1101_AGCCTRL2     0x1B        // AGC control										AGC����
#define CC1101_AGCCTRL1     0x1C        // AGC control
#define CC1101_AGCCTRL0     0x1D        // AGC control
#define CC1101_WOREVT1      0x1E        // High INT8U Event 0 timeout						���ֽ��¼�0��ʱ
#define CC1101_WOREVT0      0x1F        // Low INT8U Event 0 timeout						���ֽ��¼�0��ʱ
#define CC1101_WORCTRL      0x20        // Wake On Radio control							���߻��ѿ���
#define CC1101_FREND1       0x21        // Front end RX configuration						ǰ��RX����
#define CC1101_FREND0       0x22        // Front end TX configuration						ǰ��TX����
#define CC1101_FSCAL3       0x23        // Frequency synthesizer calibration				Ƶ�ʺϳ���У׼
#define CC1101_FSCAL2       0x24        // Frequency synthesizer calibration
#define CC1101_FSCAL1       0x25        // Frequency synthesizer calibration
#define CC1101_FSCAL0       0x26        // Frequency synthesizer calibration
#define CC1101_RCCTRL1      0x27        // RC oscillator configuration						RC��������
#define CC1101_RCCTRL0      0x28        // RC oscillator configuration
#define CC1101_FSTEST       0x29        // Frequency synthesizer calibration control		Ƶ�ʺϳ���У׼����
#define CC1101_PTEST        0x2A        // Production test									��������
#define CC1101_AGCTEST      0x2B        // AGC test											AGC����
#define CC1101_TEST2        0x2C        // Various test settings							���ֲ�������
#define CC1101_TEST1        0x2D        // Various test settings
#define CC1101_TEST0        0x2E        // Various test settings

// Strobe commands
#define CC1101_SRES         0x30        // Reset chip.
#define CC1101_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CC1101_SXOFF        0x32        // Turn off crystal oscillator.
#define CC1101_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CC1101_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CC1101_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CC1101_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CC1101_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CC1101_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CC1101_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CC1101_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CC1101_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CC1101_SWORRST      0x3C        // Reset real time clock.
#define CC1101_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // INT8Us for simpler software.

#define CC1101_PARTNUM      0x30		//PartNum			�����Ŀ
#define CC1101_VERSION      0x31		//Version			��ǰ�汾���
#define CC1101_FREQEST      0x32		//Freqest			Ƶ��ƫ�ƹ���
#define CC1101_LQI          0x33		//LQI				��·�������������
#define CC1101_RSSI         0x34		//RSSI 				�����ź�ǿ��ָʾ
#define CC1101_MARCSTATE    0x35		//MARCState			����״̬��״̬	
#define CC1101_WORTIME1     0x36		//WORTime1			WOR ��ʱ�����ֽ�
#define CC1101_WORTIME0     0x37		//WORTime0			WOR ��ʱ�����ֽ�
#define CC1101_PKTSTATUS    0x38		//PKTStatus			��ǰGDOx״̬�����ݰ�״̬
#define CC1101_VCO_VC_DAC   0x39		//VCO_VC_DAC		PLL У׼ģ��ĵ�ǰ����
#define CC1101_TXBYTES      0x3A		//TXBytes			TX FIFO�е�������ֽ���
#define CC1101_RXBYTES      0x3B		//RXBytes			RX FIFO�е�������ֽ���

#define CC1101_PATABLE      0x3E		//PATABLE
#define CC1101_TXFIFO       0x3F		//TXFIFO
#define CC1101_RXFIFO       0x3F		//RXFIFO

// Bit masks corresponding to STATE[2:0] in the status byte returned on MISO
/*#define CC1101_STATE_BM                 0x70
#define CC1101_FIFO_BYTES_AVAILABLE_BM  0x0F
#define CC1101_STATE_TX_BM              0x20
#define CC1101_STATE_TX_UNDERFLOW_BM    0x70
#define CC1101_STATE_RX_BM              0x10
#define CC1101_STATE_RX_OVERFLOW_BM     0x60
#define CC1101_STATE_IDLE_BM            0x00
 */

// RF_SETTINGS is a data structure which contains all relevant CC1101 registers
//�������ݽṹ�����������йص�cc1101�Ĵ���
typedef struct S_RF_SETTINGS
{
    uchar FSCTRL2;   //���Ѽӵ�
    uchar FSCTRL1;   // Frequency synthesizer control.			  Ƶ�ʺϳ�������
    uchar FSCTRL0;   // Frequency synthesizer control.			  Ƶ�ʺϳ�������
    uchar FREQ2;     // Frequency control word, high uchar.		  Ƶ�ʿ����֣����ֽ�
    uchar FREQ1;     // Frequency control word, middle uchar.	  Ƶ�ʿ����֣��м��ֽ�
    uchar FREQ0;     // Frequency control word, low uchar.		  Ƶ�ʿ����֣����ֽ�
    uchar MDMCFG4;   // Modem configuration.					  ���ƽ��������
    uchar MDMCFG3;   // Modem configuration.					  ���ƽ��������
    uchar MDMCFG2;   // Modem configuration.					  ���ƽ��������
    uchar MDMCFG1;   // Modem configuration.					  ���ƽ��������
    uchar MDMCFG0;   // Modem configuration.					  ���ƽ��������
    uchar CHANNR;    // Channel number.							  �ŵ�����
    uchar DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).���ƽ����ƫ�����ã�FSK���ƿ���ʱ��
    uchar FREND1;    // Front end RX configuration.						 ǰ��RX����
    uchar FREND0;    // Front end RX configuration.						 ǰ��RX����
    uchar MCSM0;     // Main Radio Control State Machine configuration.	 �����߿���״̬������
    uchar FOCCFG;    // Frequency Offset Compensation Configuration.	 Ƶ��ƫ�Ʋ�������
    uchar BSCFG;     // Bit synchronization Configuration.				 λͬ������
    uchar AGCCTRL2;  // AGC control.									 AGC����
    uchar AGCCTRL1;  // AGC control.									 AGC����
    uchar AGCCTRL0;  // AGC control.									 AGC����
    uchar FSCAL3;    // Frequency synthesizer calibration.				 Ƶ�ʺϳ���У׼
    uchar FSCAL2;    // Frequency synthesizer calibration.				 Ƶ�ʺϳ���У׼
    uchar FSCAL1;    // Frequency synthesizer calibration.				 Ƶ�ʺϳ���У׼
    uchar FSCAL0;    // Frequency synthesizer calibration.				 Ƶ�ʺϳ���У׼
    uchar FSTEST;    // Frequency synthesizer calibration control		 Ƶ�ʺϳ���У׼����
    uchar TEST2;     // Various test settings.							 ���ֲ�������
    uchar TEST1;     // Various test settings.							 ���ֲ�������
    uchar TEST0;     // Various test settings.							 ���ֲ�������
    uchar IOCFG2;    // GDO2 output pin configuration					 GDO2�����������
    uchar IOCFG0;    // GDO0 output pin configuration					 GDO0�����������
    uchar PKTCTRL1;  // Packet automation control.						 ���ݰ��Զ�����
    uchar PKTCTRL0;  // Packet automation control.						 ���ݰ��Զ�����
    uchar ADDR;      // Device address.									 ������ַ
    uchar PKTLEN;    // Packet length.									 ���ݰ�����
} RF_SETTINGS;


