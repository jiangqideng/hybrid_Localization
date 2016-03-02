#include <reg52.h>
#include <intrins.h>
#include <stdlib.h>
#include "lcd12864a.h"
#include "CC1101.h"
#include "IO_def.h"
#include "server.h"
#include "IMU.h"
//#include  <math.h>    //Keil library  
//#include  <stdio.h>   //Keil library

//***************更多功率参数设置可详细参考DATACC1100英文文档中第48-49页的参数表******************
//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   功率最小
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
code uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     功率最大
extern int j;
xdata uchar TxBuf[13]={0};	 // 实际上最后的混合系统只使用11个字节
xdata uchar RxBuf[13]={0};
int position_x=100;
int position_y=100;
uchar biaohao=0;			//每组信息的标号

//****************************************
typedef unsigned short ushort;
sbit    SCL=P2^5;			//IIC时钟引脚定义
sbit    SDA=P2^6;			//IIC数据引脚定义
//****************************************
#define	SlaveAddress_ADXL345   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改                             //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
#define	SlaveAddress_HMC   0x3C	  //定义器件在IIC总线中的从地址
//#define	SlaveAddress_ITG   0xD0	  //	注释掉了，不用ITG

//****************************
typedef unsigned char  BYTE;
typedef unsigned short WORD;

BYTE BUF_ADXL[6];                        //接收数据缓存区  
BYTE BUF_HMC[6];                         //接收数据缓存区     	
//BYTE BUF_ITG[6];                       //接收数据缓存区    

xdata int  HMC_y;						//y轴磁力计数据
xdata int  HMC_z;  						//z轴磁力计数据
xdata uchar angle ;						//计算出的角度

void Init_ADXL345(void);            	 //初始化ADXL345
void Init_HMC5883(void);           		 //初始化5883
//void InitITG3205();                 	//初始化ITG3205

void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_ADXL345();                                  //连续的读取内部寄存器数据
//------------------------------------
void Delay5us();

void ADXL345_Start();
void ADXL345_Stop();
void ADXL345_SendACK(bit ack);
bit  ADXL345_RecvACK();
void ADXL345_SendByte(BYTE dat);
BYTE ADXL345_RecvByte();
void ADXL345_ReadPage();
void ADXL345_WritePage();
//-----------------------------------

/*******************************/
void  Single_Write_HMC5883(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_Read_HMC5883(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_HMC5883();                                  //连续的读取内部寄存器数据					
/*******************************/
void HMC5883_Start();
void HMC5883_Stop();
void HMC5883_SendACK(bit ack);
bit  HMC5883_RecvACK();
void HMC5883_SendByte(BYTE dat);
BYTE HMC5883_RecvByte();
void HMC5883_ReadPage();
void HMC5883_WritePage();	

//void  Single_WriteITG3205(uchar REG_Address,uchar REG_data);   //单个写入数据
//uchar Single_ReadITG3205(uchar REG_Address);                   //单个读取内部寄存器数据
//void ITG3205_Start();
//void ITG3205_Stop();
//void ITG3205_SendACK(bit ack);
//bit  ITG3205_RecvACK();
//void ITG3205_SendByte(BYTE dat);
//BYTE ITG3205_RecvByte();
//void ITG3205_ReadPage();
//void ITG3205_WritePage();
//****************************************			

/**************************************
延时5微秒(STC90C52RC@12M)
不同的工作环境,需要调整此函数，注意时钟过快时需要修改
当改用1T的MCU时,请调整此延时函数
**************************************/
void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}


/**************************************
起始信号
**************************************/
void ADXL345_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void ADXL345_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit ADXL345_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void ADXL345_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    ADXL345_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE ADXL345_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//******单字节写入*******************************************

void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(SlaveAddress_ADXL345);   //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    ADXL345_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    ADXL345_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  uchar REG_data;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress_ADXL345);           //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress_ADXL345+1);         //发送设备地址+读信号
    REG_data=ADXL345_RecvByte();              //读出寄存器数据
	ADXL345_SendACK(1);   
	ADXL345_Stop();                           //停止信号
    return REG_data; 
}
//*********************************************************
//
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//
//*********************************************************
void Multiple_Read_ADXL345(void)
{   uchar i;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress_ADXL345);           //发送设备地址+写信号
    ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress_ADXL345+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF_ADXL[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
           ADXL345_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //回应ACK
       }
   }
    ADXL345_Stop();                          //停止信号
//    delay_ms(5);
}


//*****************************************************************

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x0C);   //速率设定为200Hz 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
//***********************************************************************

/**************************************
起始信号
**************************************/
void HMC5883_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

/**************************************
停止信号
**************************************/
void HMC5883_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void HMC5883_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

/**************************************
接收应答信号
**************************************/
bit HMC5883_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void HMC5883_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    HMC5883_RecvACK();
}

/**************************************
从IIC总线接收一个字节数据
**************************************/
BYTE HMC5883_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//***************************************************

void Single_Write_HMC5883(uchar REG_Address,uchar REG_data)
{
    HMC5883_Start();                  //起始信号
    HMC5883_SendByte(SlaveAddress_HMC);   //发送设备地址+写信号
    HMC5883_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf 
    HMC5883_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf
    HMC5883_Stop();                   //发送停止信号
}

//********单字节读取内部寄存器*************************
uchar Single_Read_HMC5883(uchar REG_Address)
{  uchar REG_data;
    HMC5883_Start();                          //起始信号
    HMC5883_SendByte(SlaveAddress_HMC);           //发送设备地址+写信号
    HMC5883_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    HMC5883_Start();                          //起始信号
    HMC5883_SendByte(SlaveAddress_HMC+1);         //发送设备地址+读信号
    REG_data=HMC5883_RecvByte();              //读出寄存器数据
	HMC5883_SendACK(1);   
	HMC5883_Stop();                           //停止信号
    return REG_data; 
}
//******************************************************
//
//连续读出HMC5883内部角度数据，地址范围0x3~0x5
//
//******************************************************
void Multiple_Read_HMC5883(void)
{   uchar i;
    HMC5883_Start();                          //起始信号
    HMC5883_SendByte(SlaveAddress_HMC);           //发送设备地址+写信号
    HMC5883_SendByte(0x03);                   //发送存储单元地址，从0x3开始	
    HMC5883_Start();                          //起始信号
    HMC5883_SendByte(SlaveAddress_HMC+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF_HMC[i] = HMC5883_RecvByte();          //BUF[0]存储数据
        if (i == 5)
        {
           HMC5883_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          HMC5883_SendACK(0);                //回应ACK
       }
   }
    HMC5883_Stop();                          //停止信号
}

//初始化HMC5883，根据需要请参考pdf进行修改****
void Init_HMC5883()
{
     Single_Write_HMC5883(0x02,0x00);  //
}

//**************************************/




//**************************************/
/*void ITG3205_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}

//**************************************
//停止信号
//**************************************
void ITG3205_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}

//**************************************
//发送应答信号
//入口参数:ack (0:ACK 1:NAK)
//*************************************
void ITG3205_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}

//**************************************
//接收应答信号
//**************************************
bit ITG3205_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}

//**************************************
//向IIC总线发送一个字节数据
//**************************************
void ITG3205_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    ITG3205_RecvACK();
}

//**************************************
//从IIC总线接收一个字节数据
//**************************************
BYTE ITG3205_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

//单字节写入*******************************************

void Single_WriteITG3205(uchar REG_Address,uchar REG_data)
{
    ITG3205_Start();                  //起始信号
    ITG3205_SendByte(SlaveAddress_ITG);   //发送设备地址+写信号
    ITG3205_SendByte(REG_Address);    //内部寄存器地址，
    ITG3205_SendByte(REG_data);       //内部寄存器数据，
    ITG3205_Stop();                   //发送停止信号
}

//单字节读取*****************************************
uchar Single_ReadITG3205(uchar REG_Address)
{  uchar REG_data;
    ITG3205_Start();                          //起始信号
    ITG3205_SendByte(SlaveAddress_ITG);           //发送设备地址+写信号
    ITG3205_SendByte(REG_Address);            //发送存储单元地址，从0开始	
    ITG3205_Start();                          //起始信号
    ITG3205_SendByte(SlaveAddress_ITG+1);         //发送设备地址+读信号
    REG_data=ITG3205_RecvByte();              //读出寄存器数据
	ITG3205_SendACK(1);   
	ITG3205_Stop();                           //停止信号
    return REG_data; 
}

//初始化ITG3205，根据需要请参考pdf进行修改************************
void InitITG3205()
{
   Single_WriteITG3205(PWR_M, 0x80);   //
   Single_WriteITG3205(SMPL, 0x07);    //
   Single_WriteITG3205(DLPF, 0x1E);    //±2000°
   Single_WriteITG3205(INT_C, 0x00 );  //
   Single_WriteITG3205(PWR_M, 0x00);   //
}		   */
//***********************************************************************


//*********************************************************

//**************************************
//在1602上显示10位数据

//**************************************
//显示温度
//**************************************
//void display_temp()
//{ 
//	Temp_h=Single_ReadI2C(TEMP_OUT_H); //读取温度
//	Temp_l=Single_ReadI2C(TEMP_OUT_L); //读取温度
//	Temperature=Temp_h<<8|Temp_l;     //合成温度
//	Temperature = 35+ ((double) (Temperature + 13200)) / 280; // 计算出温度
//	lcd_printf(dis,Temperature);     //转换数据显示
//	DisplayListChar(11,1,dis,4);     //启始列，行，显示数组，显示位数
//}
	   

//步伐检测的程序从matalb那边移植过来，去那边看注释//
 uchar IMU_i=0;
 uchar IMU_j=0;
 uchar TH1_now,TL1_now;

int u_u_gate=8000;
int u_d_gate=450;
int d_u_gate=-450;
xdata int d_d_gate=-8000;
xdata int gate_init=0;
xdata int path_sum=0;		  //matlab里只有变量path
xdata uchar path_num=0; 
xdata int samepath=20;
xdata uchar path_f=20;
xdata int path_d=0;
xdata int path_max_old=0;
xdata int path_min_old=0;
xdata uchar pre_match=0;
xdata int abnormal=80;
xdata int buf_z[50]={0};   		//matlab里这个变量叫buf
xdata int buf_sum[40]={0};
xdata int max_y=0;
xdata int min_y=0;
xdata int accel_y=0;
xdata int accel_x=0;
int path_distance_sum=0;
xdata uchar path_distance=0;
xdata uchar path_d_old=0;
xdata uchar test=0;
xdata int  maxmin_sum=0;
xdata int  path_d_sum=0;

void path_test()
{	uchar disp_result[4]=0;
	uchar num_u=0;
	uchar num_d=0;
	int path_max=0;
	int path_min=0;
	int maxmin=0;
	uchar path_false=0;
	uchar j=0;
	if(TF1==1)	{IMU_j++;TH1=0x00;TL1=0x00;TF1=0;}	
	Multiple_Read_ADXL345();						 //这个语句运行0.57ms
	TH1_now=TH1;TL1_now=TL1;						//记录当前数据与上一个数据间的时间差
	TH1=0x00;
	TL1=0x00;
	TF1=0;

/*	  BUF_ITG[0]= Single_ReadITG3205(GX_L);
      BUF_ITG[1]= Single_ReadITG3205(GX_H);	//x
      BUF_ITG[2]= Single_ReadITG3205(GY_L);
      BUF_ITG[3]= Single_ReadITG3205(GY_H);	//y
      BUF_ITG[4]= Single_ReadITG3205(GZ_L);
      BUF_ITG[5]= Single_ReadITG3205(GZ_H);	//z		*/

	  Multiple_Read_HMC5883();      //连续读出数据，存储在BUF_HMC中	   //这个语句运行0.56ms

	  HMC_z=BUF_HMC[2] << 8 | BUF_HMC[3];	
	  HMC_y=BUF_HMC[4] << 8 | BUF_HMC[5];
	  	if(HMC_z>=32768)
		{	HMC_z=HMC_z-65536;			
		}
		if(HMC_y>=32768)
		{	HMC_y=HMC_y-65536;			
		}
	  HMC_z=HMC_z+167;
	  HMC_y=HMC_y+397;	
//	  angle= atan2((double)HMC_y,(double)HMC_z) * (180 / 3.14159265) + 180; // angle in degrees	 
/////////////////////////判断方向////////////////////////////////////////////////////
	 if (HMC_z>150 && HMC_y<243 && HMC_y>-243)      //222
	  	angle=2;
		else if (HMC_z<-150 && HMC_y<243 && HMC_y>-243)     //-222
			angle =4;
				else if (HMC_y>0)			//245
					angle=1;
					else 
						angle=3;


/*	if (position_x>=11 && position_x<=67)		 //此时的方向只可能为1或3
		if (HMC_y>0)
			angle=1;
		else 
			angle=3;
	if (position_y>=11 && position_y<=46)		 //此时的方向只可能为2或4
		if(HMC_z>0)
			angle=2;
		else
			angle=4;		  */
//	  writedigital(angle,6,65);
/////////////////////////////////////////////////////////////////////////////////////////////

	TxBuf[1]=TH1_now;
	TxBuf[2]=TL1_now;
//	TxBuf[3]=BUF_ITG[5];
//	TxBuf[4]=BUF_ITG[4];
//	TxBuf[3]=BUF_ADXL[1];
//	TxBuf[4]=BUF_ADXL[0];
//	TxBuf[1]=BUF_ADXL[5];
//	TxBuf[2]=BUF_ADXL[4];
	TxBuf[3]=BUF_ADXL[1];
	TxBuf[4]=BUF_ADXL[0];
	TxBuf[5]=0x99; 
	TxBuf[6]=IMU_j;
	IMU_j=0;
	
//	halRfSendPacket(TxBuf,7);			//这里运行2.76ms，没多发送一个字节，多耗时0.12ms
//	my_delay(1843);

//*********************更新buf_z*******************************************	 下面的这段步伐检测耗费大约0.5ms

	for(IMU_i=0;IMU_i<49;IMU_i++)
	{   buf_z[IMU_i]=buf_z[IMU_i+1];
	}

	buf_z[49]=(BUF_ADXL[1]<<8)+BUF_ADXL[0];
	if(buf_z[49]>=32768)
		{	buf_z[49]=buf_z[49]-65536;			
		}
	buf_z[49]=buf_z[49]*39/10-1024;			 
	for (IMU_i=0;IMU_i<40;IMU_i++)			//每次计算之前将buf_sum清零，matlab的计算和这里不一样
	{		buf_sum[IMU_i]=0;
	}
	for (IMU_i=0;IMU_i<40;IMU_i++)
	{  	for (j=0;j<11;j++) 
			{ buf_sum[IMU_i]=buf_sum[IMU_i]+buf_z[IMU_i+j];
			}	
	}

	accel_y=(BUF_ADXL[5]<<8)+BUF_ADXL[4];
	if(accel_y>=32768)
		{	accel_y=accel_y-65536;			
		}
	accel_y=accel_y*39/10;
	
	accel_x=(BUF_ADXL[5]<<8)+BUF_ADXL[4];
	if(accel_x>=32768)
		{	accel_x=accel_x-65536;			
		}
	accel_x=accel_x*39/10;			

	 samepath=samepath+1;
	 path_d=path_d+1;
	 gate_init=gate_init+1;
	 abnormal=abnormal+1;
	 if (abnormal>250)
	 	abnormal=80;
	 if (gate_init>120)
	 {	u_d_gate=450;
		d_u_gate=-450;
		u_u_gate=9000;
		d_d_gate=-9000;
		path_max_old=0;
		path_min_old=0;
	 }	

	 if (accel_y>max_y)
		{ max_y=accel_y;}
	 if (accel_y<min_y)
	 	{ min_y=accel_y;}
	 if (buf_z[49]>10000 || buf_z[49]<-10000	|| accel_x>1000 || accel_x<-1000 || accel_y>1000 || accel_y<-1300 ) 
	 	{abnormal=0;}


	 for (IMU_i=0;IMU_i<20;IMU_i++)
	 {	if(buf_sum[IMU_i]>=u_d_gate)
	 	{	num_u=num_u+1;	   }
//		if (buf_sum[IMU_i]>=u_u_gate && abnormal>=50)
//		{    abnormal=40;	}
		if (buf_sum[IMU_i]>path_max)
		{	 path_max=buf_sum[IMU_i];}

	 }
	 for (IMU_i=20;IMU_i<40;IMU_i++)
	 {	if(buf_sum[IMU_i]<=d_u_gate)
	 	{	num_d=num_d+1;	   }
//		if (buf_sum[IMU_i]<=d_d_gate && abnormal>=50)
//		{    abnormal=40;	}
		if (buf_sum[IMU_i]<path_min)
		{	 path_min=buf_sum[IMU_i];}

	 }
											   //从读取信息完毕然后发送之后，运行到这里花费5.57ms
	 if (num_u>3 && num_d>2)
	 {		
	 		if (samepath>=5)
	 		{	if (path_max_old-path_min_old<3000)
				{	path_f=20;	 }
				else
				{	path_f=20;	 }

				if (abnormal>=50)
				{	pre_match=1; }

				path_max_old=0;
				path_min_old=0;
				
			}
		 samepath=0;
		 if (path_max>path_max_old)
		 	{path_max_old=path_max;}
		 if (path_min<path_min_old)
		 	{path_min_old=path_min;}

		 if (path_max>u_u_gate || path_min<d_d_gate)
		 	{path_false=1;}

		 gate_init=0;

	 }

	 if (samepath==6)
	 {//	if (max_y>=50 && max_y-min_y>=100)
	 	{
		 	if (pre_match==1 && path_d>path_f)
		 	{
			 	path_num++;		   
				path_sum++;
				pre_match=0;
				maxmin=max_y-min_y;

    if (maxmin<=250)                 
        path_distance=54;
 	else if (maxmin<300)
        path_distance=87;
    else if (maxmin<350)
        path_distance=92;
    else if (maxmin<400)
        path_distance=96;
    else if (maxmin<450)
        path_distance=100;
    else if (maxmin<500)
        path_distance=107;
    else if (maxmin<550)
        path_distance=112;
    else if (maxmin<600)
        path_distance=114;
    else if (maxmin<650)
        path_distance=116;
    else if (maxmin<700)
        path_distance=118;
    else if (maxmin<800)
        path_distance=120;
    else if (maxmin<9000)
        path_distance=122;
    else if (maxmin>900)
        path_distance=125;

		   if (path_distance_sum>=10000)
			  path_distance_sum=0;

		   path_distance_sum=path_distance_sum+path_distance;
		   if (angle==1) 
		   	position_x=position_x-(path_distance-path_false*path_d_old);
		   if (angle==2)
		   	position_y=position_y+(path_distance-path_false*path_d_old);
		   if (angle==3)
		   	position_x=position_x+(path_distance-path_false*path_d_old);
		   if (angle==4)
		   	position_y=position_y-(path_distance-path_false*path_d_old);

		   if (path_false==1)
			  {path_distance_sum=path_distance_sum-path_d_old;
			   path_sum=path_sum-1;
			   path_false=0;
			  }
			path_d_old=path_distance;
			max_y=0;
			min_y=0;

	
//				Dispchar(path_num,5,90);
//			 	Dispchar(path_sum,3,90);
//				Dispchar(path_distance,3,120);
//				Dispchar(path_distance_sum/100,1,120);
//////////////////////////////利用地图与方向信息校准////////////////////////////////////////////////////////////
				if (position_x>400 && position_x<7400 && position_y>400	&& position_y<5300)		 //如果出现在了不允许出现的位置，这时候来消除误差
					{		
						if (angle==1 || angle==3)
							if (position_y<2800)
								position_y=100;
							else
								position_y=5600;
						else 
							if (position_x<3800)
								position_x=100;
							else 
								position_x=7700;
					}
				 if (position_x>7700)
				 	position_x=7700;
				 if (position_x<100)
				 	position_x=100;
				 if (position_y>5600)
				 	position_y=5600;
				 if (position_y<100)
				 	position_y=100;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

						Dispchar2(position_x/100,4,50);
						writedigital(19,4,34);	  // 小数点
						Dispchar2(position_x%100,4,31);
						Dispchar2(position_y/100,2,50);
						writedigital(19,2,34);	  // 小数点
						Dispchar2(position_y%100,2,31);				  

						Dispchar2(path_sum%100,6,54);

						Dispchar2(path_distance/100,6,36);
						writedigital(19,6,20);	  // 小数点
						Dispchar2(path_distance%100,6,17);	
						

			}
		    path_d=0;
	
			d_d_gate=path_min_old*4;
			if (path_min_old/5<-450)
				d_u_gate=path_min_old/5;
			else 
				d_u_gate=-450;
	
			u_u_gate=path_max_old*3;
			if (path_max_old/3>450)
			 	u_d_gate=path_max_old/3;
			else
				u_d_gate=450;
		 }	
//		 else
//		 {test++;Dispchar(test,5,20);}
	 }					

//从上面的窗口赋值完毕后，运行到这里，如果没有进入if，基本不花时间，如果进入了if
//	Dispchar(BUF_ADXL[0],3,90);		 
}
void my_send_data()
{		  				uchar i=0;
						TxBuf[0]=0x00;
						TxBuf[1]=0xFF;
						TxBuf[2]=0x01;
						TxBuf[3]=0xFF;
						TxBuf[4]=0x00;
						TxBuf[5]=biaohao;
						TxBuf[6]=angle;
//						TxBuf[7]=position_x/100;
//						TxBuf[8]=position_x%100;
						TxBuf[7]=path_distance_sum/100;
						TxBuf[8]=path_distance_sum%100;
						TxBuf[9]=position_y/100;
						TxBuf[10]=position_y%100;


						for (i=0;i<1;i++)
						{	if (KEY2==0)				  //用来测惯导性能，到达某点都按下KEY2，惯导暂停工作，同时发送信息给主机，
								{	halRfSendPacket(TxBuf,11); delay_ms(2000);}

//							my_delay(120);		//为了保持采样频率100Hz
							if (KEY2!=0)				  //这样的话如果按着按键，惯导就不进行工作
							path_test();
						}	
						  
}

void main()
{		uchar i_signal=0;
		uchar ss=1;
		uchar leng=10;
		uchar tt=0;

    
	BELL =1;		 //不响铃
	LED1=0;			 //开灯
	LED2=0;
//	UART_init();
//    back_lcd=0; 	  //液晶背光开
    lcd_init();
//	delay_ms(500);
//	delay1();
//    delay1();
//    delay1();
    disdate_a(0x00,0x00);				  
	delay_ms(500);
//	delay1();
//    delay1();
//    delay1();
//  entermenu();              /*-----开机显示  天津大学欢迎您！ -----*/
//	delay1();
//    delay1();
//    delay1();
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


draw_map();
writedigital(22,4,65);
writedigital(23,2,65);
writedigital(16,4,57);
writedigital(16,2,57);
//writedigital(21,3,120);
//writedigital(21,3,112);
//writedigital(16,3,104);
//writedigital(20,7,120);
//writedigital(16,7,113);	 

//draw_position(1,54);



//	Adjust_res_18B20(0x3f);	  //0.25
//	ReadTemperature();

	
	Init_ADXL345();                 //初始化ADXL345
//	devid=Single_Read_ADXL345(0X00);//读出的数据为0XE5,表示正确

	Init_HMC5883();
//	InitITG3205();                   //初始化ITG3205


	TMOD = TMOD | 0x10;
	TH1=0x00;
	TL1=0x00;
	TF1=0;
	TR1=1;	


	TMOD = TMOD | 0x01;
	TH0=0x4c;
	TL0=0x00;
	TF0=0;
	TR0=1;				

	TxBuf[1]=0xFF;
	TxBuf[3]=0xFF;								 
		biaohao++;													   //第一次数据
		TxBuf[5]=biaohao; 			
		LED1=~LED1;
//		SendPacket(TxBuf,8,0);
					

		my_send_data();


		LED1=~LED1;
//		writedigital(biaohao%10,7,92);
//		Dispchar(biaohao%256,7,103);
		tt=0;


	while(1)
	{			  
	  if ((TH1*256+TL1)>=8558 || IMU_j>=1)	  //时间应为10000us-573us，但本单片机晶振为11.0592MHz	//528是IMU耗费时间，2857是步伐检测的时间
	  {		if (KEY2!=0)				  //这样的话如果按着按键，惯导就不进行工作
	  			path_test();
	  }

	   if (KEY1==0)
	   {
	   draw_position(position_x/100,position_y/100);	 //这家伙耗时很多，大于100ms吧
		}


	  if(TF1==1)	{IMU_j++;TH1=0x00;TL1=0x00;TF1=0;}	

		 if(TF0==1)
		 {	tt++;	
		 	if(tt==20)		   //1s
			{		
					biaohao++;
					TxBuf[5]=biaohao; 			
					LED1=~LED1;
					i_signal=i_signal+1;
					if(i_signal==2) 
					{/*	writeone(0,6,60);		  //清空		当
						writeone(0,6,44);			//		  前
						writeone(0,6,28);			  //		位
						writeone(0,6,12);				//		  置
						writeone(2,6,52);		//	  	无
						writeone(3,6,36);		  //	信
						writeone(4,6,20);			//	号		*/
						i_signal=0;	 
					}
					writedigital(angle,6,65);//为了省点时间，就在这显示了														
					my_send_data();
					path_num=0;		//周期内步伐数清零
					LED1=~LED1;
//					Dispchar(biaohao%256,7,103);
					tt=0;					
			}
			TH0=0x4c;
			TL0=0x00;
			TF0=0;			 								 
		 }
			
		if(halRfReceivePacket(RxBuf,&leng))		
		{	if (RxBuf[1]!=0xff)	
			{ 		/*	writeone(0,6,52);		//	  	无	   //清空
						writeone(0,6,36);		  //	信
						writeone(0,6,20);			//	号	
						writeone(5,6,60);		  //		当
						writeone(6,6,44);			//		  前
						writeone(7,6,28);			  //		位
						writeone(8,6,12);				//		  置				   */

//			 Dispchar(ss,3,30);					  //测试，用来观察收到几个返回的数据包
//			 if(RxBuf[5]==biaohao)											 //返回的第一个数据包，节点2345的RSSI  ，测试用
//			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,120);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,120);
//				Dispchar(RxBuf[1],1,30);//附近最近节点。
		//	 	Dispchar(RxBuf[3],7,120);
		//		Dispchar(RxBuf[4],5,120);
		//		Dispchar(RxBuf[6],3,120);
		//	 	Dispchar(RxBuf[7],1,120);
//				writedigital(RxBuf[1],3,90);//附近最近节点。
//				ss++;
//				LED2=~LED2;
//			 }
			 if(RxBuf[5]==biaohao)										//返回的第二个数据包。节点6789的RSSI ，测试用
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
				Dispchar2(RxBuf[5]%100,5,100);
//				draw_position(RxBuf[3],RxBuf[6]);


				

				ss++;
				LED2=~LED2;


					biaohao++;
					TxBuf[5]=biaohao; 			
					LED1=~LED1;						   
				Dispchar2(biaohao%100,7,103);
				my_send_data();

				LED1=~LED1;
				
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
	}