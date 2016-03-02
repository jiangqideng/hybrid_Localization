							  
#define		uchar 	unsigned char
#define 	uint 	unsigned int 

#define 	WRITE_BURST 		0x40							//连续写入
#define 	READ_SINGLE			0x80							//读
#define 	READ_BURST			0xC0							//连续读
#define		BYTES_IN_RXFIFO		0x7F							//接收缓冲区的有效字节数
#define 	CRC_OK				0x80							//CRC校验通过位标志



//******************************************************************************************
void 	SpiInit();													 //SPI初始化函数
void 	CpuInit();													 //
void 	RESET_CC1101();											 	 //复位CC1101
void 	POWER_UP_RESET_CC1101();									 //上电复位CC1101
void 	halSpiWriteReg(uchar addr,uchar value);					 	 //SPI写寄存器
void 	halSpiWriteBurstReg(uchar addr,uchar *buffer,uchar count);	 //SPI连续写配置寄存器
void 	halSpiStrobe(uchar strobe);									 //SPI写命令
uchar	halSpiReadReg(uchar addr);									 //SPI读寄存器
void	halSpiReadBurstReg(uchar addr,uchar *buffer,uchar count); 	 //SPI连续读配置寄存器
uchar	halSpiReadStatus(uchar addr);								 //SPI读状态寄存器
void	halRfWriteRfSettings();										 //配置CC1101的寄存器
void	halRfSendPacket(uchar *txbuffer,uchar size);				 //CC1101发送一组数据
char	halRfReceivePacket(uchar *rxbuffer,uchar *length);			 //CC1101接收一组数据
void 	halWait(uint timeout);										 //
uchar 	SpiTxRxByte(uchar dat);										 //
void 	setRxMode() ;												 //设置接收状态

int 	GetRSSI();

static uchar RfGetRxStatus();

//自己定义的可靠发送与可靠接收的函数
uchar SendPacket(uchar *txbuffer,uchar size,uchar address);
uchar ReceivePacket(uchar *rxbuffer,uchar *length);
//******************************************************************************************

// CC1101 STROBE, CONTROL AND STATUS REGSITER	配置CC1101状态，控制和状态寄存器
#define CC1101_IOCFG2       0x00        // GDO2 output pin configuration	 GDO2输出引脚配置
#define CC1101_IOCFG1       0x01        // GDO1 output pin configuration	 GDO1输出引脚配置
#define CC1101_IOCFG0       0x02        // GDO0 output pin configuration	 GDO0输出引脚配置
#define CC1101_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds	 RX FIFO和TX FIFO阈值
#define CC1101_SYNC1        0x04        // Sync word, high uchar			 同步字，高8位
#define CC1101_SYNC0        0x05        // Sync word, low uchar				 同步字，低8位
#define CC1101_PKTLEN       0x06        // Packet length					 数据包长度
#define CC1101_PKTCTRL1     0x07        // Packet automation control		 数据包自动控制
#define CC1101_PKTCTRL0     0x08        // Packet automation control
#define CC1101_ADDR         0x09        // Device address					 器件地址
#define CC1101_CHANNR       0x0A        // Channel number					 信道数量
#define CC1101_FSCTRL1      0x0B        // Frequency synthesizer control		   频率合成器控制
#define CC1101_FSCTRL0      0x0C        // Frequency synthesizer control
#define CC1101_FREQ2        0x0D        // Frequency control word, high uchar	   频率控制字（控制通信频段）
#define CC1101_FREQ1        0x0E        // Frequency control word, middle uchar
#define CC1101_FREQ0        0x0F        // Frequency control word, low uchar
#define CC1101_MDMCFG4      0x10        // Modem configuration					   调制解调器配置
#define CC1101_MDMCFG3      0x11        // Modem configuration
#define CC1101_MDMCFG2      0x12        // Modem configuration
#define CC1101_MDMCFG1      0x13        // Modem configuration
#define CC1101_MDMCFG0      0x14        // Modem configuration
#define CC1101_DEVIATN      0x15        // Modem deviation setting				   调制解调器偏差配置
#define CC1101_MCSM2        0x16        // Main Radio Control State Machine configuration  主无线控制状态机配置
#define CC1101_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CC1101_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CC1101_FOCCFG       0x19        // Frequency Offset Compensation configuration		频率偏移补偿配置
#define CC1101_BSCFG        0x1A        // Bit Synchronization configuration				位同步控制
#define CC1101_AGCCTRL2     0x1B        // AGC control										AGC控制
#define CC1101_AGCCTRL1     0x1C        // AGC control
#define CC1101_AGCCTRL0     0x1D        // AGC control
#define CC1101_WOREVT1      0x1E        // High INT8U Event 0 timeout						高字节事件0超时
#define CC1101_WOREVT0      0x1F        // Low INT8U Event 0 timeout						低字节事件0超时
#define CC1101_WORCTRL      0x20        // Wake On Radio control							无线唤醒控制
#define CC1101_FREND1       0x21        // Front end RX configuration						前端RX配置
#define CC1101_FREND0       0x22        // Front end TX configuration						前端TX配置
#define CC1101_FSCAL3       0x23        // Frequency synthesizer calibration				频率合成器校准
#define CC1101_FSCAL2       0x24        // Frequency synthesizer calibration
#define CC1101_FSCAL1       0x25        // Frequency synthesizer calibration
#define CC1101_FSCAL0       0x26        // Frequency synthesizer calibration
#define CC1101_RCCTRL1      0x27        // RC oscillator configuration						RC振荡器配置
#define CC1101_RCCTRL0      0x28        // RC oscillator configuration
#define CC1101_FSTEST       0x29        // Frequency synthesizer calibration control		频率合成器校准控制
#define CC1101_PTEST        0x2A        // Production test									生产测试
#define CC1101_AGCTEST      0x2B        // AGC test											AGC测试
#define CC1101_TEST2        0x2C        // Various test settings							各种测试配置
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

#define CC1101_PARTNUM      0x30		//PartNum			组件数目
#define CC1101_VERSION      0x31		//Version			当前版本编号
#define CC1101_FREQEST      0x32		//Freqest			频率偏移估计
#define CC1101_LQI          0x33		//LQI				链路质量解调器评估
#define CC1101_RSSI         0x34		//RSSI 				接收信号强度指示
#define CC1101_MARCSTATE    0x35		//MARCState			接收状态机状态	
#define CC1101_WORTIME1     0x36		//WORTime1			WOR 定时器高字节
#define CC1101_WORTIME0     0x37		//WORTime0			WOR 定时器低字节
#define CC1101_PKTSTATUS    0x38		//PKTStatus			当前GDOx状态和数据包状态
#define CC1101_VCO_VC_DAC   0x39		//VCO_VC_DAC		PLL 校准模块的当前设置
#define CC1101_TXBYTES      0x3A		//TXBytes			TX FIFO中的下溢和字节数
#define CC1101_RXBYTES      0x3B		//RXBytes			RX FIFO中的下溢和字节数

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
//定义数据结构，包含所有有关的cc1101寄存器
typedef struct S_RF_SETTINGS
{
    uchar FSCTRL2;   //自已加的
    uchar FSCTRL1;   // Frequency synthesizer control.			  频率合成器控制
    uchar FSCTRL0;   // Frequency synthesizer control.			  频率合成器控制
    uchar FREQ2;     // Frequency control word, high uchar.		  频率控制字，高字节
    uchar FREQ1;     // Frequency control word, middle uchar.	  频率控制字，中间字节
    uchar FREQ0;     // Frequency control word, low uchar.		  频率控制字，低字节
    uchar MDMCFG4;   // Modem configuration.					  调制解调器配置
    uchar MDMCFG3;   // Modem configuration.					  调制解调器配置
    uchar MDMCFG2;   // Modem configuration.					  调制解调器配置
    uchar MDMCFG1;   // Modem configuration.					  调制解调器配置
    uchar MDMCFG0;   // Modem configuration.					  调制解调器配置
    uchar CHANNR;    // Channel number.							  信道数量
    uchar DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).调制解调器偏差配置（FSK调制可用时）
    uchar FREND1;    // Front end RX configuration.						 前端RX配置
    uchar FREND0;    // Front end RX configuration.						 前端RX配置
    uchar MCSM0;     // Main Radio Control State Machine configuration.	 主无线控制状态机配置
    uchar FOCCFG;    // Frequency Offset Compensation Configuration.	 频率偏移补偿配置
    uchar BSCFG;     // Bit synchronization Configuration.				 位同步配置
    uchar AGCCTRL2;  // AGC control.									 AGC控制
    uchar AGCCTRL1;  // AGC control.									 AGC控制
    uchar AGCCTRL0;  // AGC control.									 AGC控制
    uchar FSCAL3;    // Frequency synthesizer calibration.				 频率合成器校准
    uchar FSCAL2;    // Frequency synthesizer calibration.				 频率合成器校准
    uchar FSCAL1;    // Frequency synthesizer calibration.				 频率合成器校准
    uchar FSCAL0;    // Frequency synthesizer calibration.				 频率合成器校准
    uchar FSTEST;    // Frequency synthesizer calibration control		 频率合成器校准控制
    uchar TEST2;     // Various test settings.							 各种测试设置
    uchar TEST1;     // Various test settings.							 各种测试设置
    uchar TEST0;     // Various test settings.							 各种测试设置
    uchar IOCFG2;    // GDO2 output pin configuration					 GDO2输出引脚配置
    uchar IOCFG0;    // GDO0 output pin configuration					 GDO0输出引脚配置
    uchar PKTCTRL1;  // Packet automation control.						 数据包自动控制
    uchar PKTCTRL0;  // Packet automation control.						 数据包自动控制
    uchar ADDR;      // Device address.									 器件地址
    uchar PKTLEN;    // Packet length.									 数据包长度
} RF_SETTINGS;


