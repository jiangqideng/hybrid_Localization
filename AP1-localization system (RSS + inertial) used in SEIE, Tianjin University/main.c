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

//***************���๦�ʲ������ÿ���ϸ�ο�DATACC1100Ӣ���ĵ��е�48-49ҳ�Ĳ�����******************
//uchar PaTabel[8] = {0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04 ,0x04};  //-30dBm   ������С
//uchar PaTabel[8] = {0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60 ,0x60};  //0dBm
code uchar PaTabel[8] = {0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0 ,0xC0};   //10dBm     �������
extern int j;
xdata uchar TxBuf[13]={0};	 // ʵ�������Ļ��ϵͳֻʹ��11���ֽ�
xdata uchar RxBuf[13]={0};
int position_x=100;
int position_y=100;
uchar biaohao=0;			//ÿ����Ϣ�ı��

//****************************************
typedef unsigned short ushort;
sbit    SCL=P2^5;			//IICʱ�����Ŷ���
sbit    SDA=P2^6;			//IIC�������Ŷ���
//****************************************
#define	SlaveAddress_ADXL345   0xA6	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�                             //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A
#define	SlaveAddress_HMC   0x3C	  //����������IIC�����еĴӵ�ַ
//#define	SlaveAddress_ITG   0xD0	  //	ע�͵��ˣ�����ITG

//****************************
typedef unsigned char  BYTE;
typedef unsigned short WORD;

BYTE BUF_ADXL[6];                        //�������ݻ�����  
BYTE BUF_HMC[6];                         //�������ݻ�����     	
//BYTE BUF_ITG[6];                       //�������ݻ�����    

xdata int  HMC_y;						//y�����������
xdata int  HMC_z;  						//z�����������
xdata uchar angle ;						//������ĽǶ�

void Init_ADXL345(void);            	 //��ʼ��ADXL345
void Init_HMC5883(void);           		 //��ʼ��5883
//void InitITG3205();                 	//��ʼ��ITG3205

void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //����д������
uchar Single_Read_ADXL345(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
void  Multiple_Read_ADXL345();                                  //�����Ķ�ȡ�ڲ��Ĵ�������
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
void  Single_Write_HMC5883(uchar REG_Address,uchar REG_data);   //����д������
uchar Single_Read_HMC5883(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
void  Multiple_Read_HMC5883();                                  //�����Ķ�ȡ�ڲ��Ĵ�������					
/*******************************/
void HMC5883_Start();
void HMC5883_Stop();
void HMC5883_SendACK(bit ack);
bit  HMC5883_RecvACK();
void HMC5883_SendByte(BYTE dat);
BYTE HMC5883_RecvByte();
void HMC5883_ReadPage();
void HMC5883_WritePage();	

//void  Single_WriteITG3205(uchar REG_Address,uchar REG_data);   //����д������
//uchar Single_ReadITG3205(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
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
��ʱ5΢��(STC90C52RC@12M)
��ͬ�Ĺ�������,��Ҫ�����˺�����ע��ʱ�ӹ���ʱ��Ҫ�޸�
������1T��MCUʱ,���������ʱ����
**************************************/
void Delay5us()
{
    _nop_();_nop_();_nop_();_nop_();
    _nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}


/**************************************
��ʼ�ź�
**************************************/
void ADXL345_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void ADXL345_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 1;                    //����������
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
bit ADXL345_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ

    return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void ADXL345_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    ADXL345_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
BYTE ADXL345_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}

//******���ֽ�д��*******************************************

void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress_ADXL345);   //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    ADXL345_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    ADXL345_Stop();                   //����ֹͣ�ź�
}

//********���ֽڶ�ȡ*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  uchar REG_data;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress_ADXL345);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress_ADXL345+1);         //�����豸��ַ+���ź�
    REG_data=ADXL345_RecvByte();              //�����Ĵ�������
	ADXL345_SendACK(1);   
	ADXL345_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
//*********************************************************
//
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//
//*********************************************************
void Multiple_Read_ADXL345(void)
{   uchar i;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress_ADXL345);           //�����豸��ַ+д�ź�
    ADXL345_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_SendByte(SlaveAddress_ADXL345+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF_ADXL[i] = ADXL345_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
           ADXL345_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //��ӦACK
       }
   }
    ADXL345_Stop();                          //ֹͣ�ź�
//    delay_ms(5);
}


//*****************************************************************

//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x0C);   //�����趨Ϊ200Hz �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}
//***********************************************************************

/**************************************
��ʼ�ź�
**************************************/
void HMC5883_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}

/**************************************
ֹͣ�ź�
**************************************/
void HMC5883_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 1;                    //����������
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void HMC5883_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
bit HMC5883_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ

    return CY;
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void HMC5883_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    HMC5883_RecvACK();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
BYTE HMC5883_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}

//***************************************************

void Single_Write_HMC5883(uchar REG_Address,uchar REG_data)
{
    HMC5883_Start();                  //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress_HMC);   //�����豸��ַ+д�ź�
    HMC5883_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf 
    HMC5883_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf
    HMC5883_Stop();                   //����ֹͣ�ź�
}

//********���ֽڶ�ȡ�ڲ��Ĵ���*************************
uchar Single_Read_HMC5883(uchar REG_Address)
{  uchar REG_data;
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress_HMC);           //�����豸��ַ+д�ź�
    HMC5883_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress_HMC+1);         //�����豸��ַ+���ź�
    REG_data=HMC5883_RecvByte();              //�����Ĵ�������
	HMC5883_SendACK(1);   
	HMC5883_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
//******************************************************
//
//��������HMC5883�ڲ��Ƕ����ݣ���ַ��Χ0x3~0x5
//
//******************************************************
void Multiple_Read_HMC5883(void)
{   uchar i;
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress_HMC);           //�����豸��ַ+д�ź�
    HMC5883_SendByte(0x03);                   //���ʹ洢��Ԫ��ַ����0x3��ʼ	
    HMC5883_Start();                          //��ʼ�ź�
    HMC5883_SendByte(SlaveAddress_HMC+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF_HMC[i] = HMC5883_RecvByte();          //BUF[0]�洢����
        if (i == 5)
        {
           HMC5883_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          HMC5883_SendACK(0);                //��ӦACK
       }
   }
    HMC5883_Stop();                          //ֹͣ�ź�
}

//��ʼ��HMC5883��������Ҫ��ο�pdf�����޸�****
void Init_HMC5883()
{
     Single_Write_HMC5883(0x02,0x00);  //
}

//**************************************/




//**************************************/
/*void ITG3205_Start()
{
    SDA = 1;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
}

//**************************************
//ֹͣ�ź�
//**************************************
void ITG3205_Stop()
{
    SDA = 0;                    //����������
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA = 1;                    //����������
    Delay5us();                 //��ʱ
}

//**************************************
//����Ӧ���ź�
//��ڲ���:ack (0:ACK 1:NAK)
//*************************************
void ITG3205_SendACK(bit ack)
{
    SDA = ack;                  //дӦ���ź�
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

//**************************************
//����Ӧ���ź�
//**************************************
bit ITG3205_RecvACK()
{
    SCL = 1;                    //����ʱ����
    Delay5us();                 //��ʱ
    CY = SDA;                   //��Ӧ���ź�
    SCL = 0;                    //����ʱ����
    Delay5us();                 //��ʱ

    return CY;
}

//**************************************
//��IIC���߷���һ���ֽ�����
//**************************************
void ITG3205_SendByte(BYTE dat)
{
    BYTE i;

    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ����ݵ����λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    ITG3205_RecvACK();
}

//**************************************
//��IIC���߽���һ���ֽ�����
//**************************************
BYTE ITG3205_RecvByte()
{
    BYTE i;
    BYTE dat = 0;

    SDA = 1;                    //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    return dat;
}

//���ֽ�д��*******************************************

void Single_WriteITG3205(uchar REG_Address,uchar REG_data)
{
    ITG3205_Start();                  //��ʼ�ź�
    ITG3205_SendByte(SlaveAddress_ITG);   //�����豸��ַ+д�ź�
    ITG3205_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    ITG3205_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
    ITG3205_Stop();                   //����ֹͣ�ź�
}

//���ֽڶ�ȡ*****************************************
uchar Single_ReadITG3205(uchar REG_Address)
{  uchar REG_data;
    ITG3205_Start();                          //��ʼ�ź�
    ITG3205_SendByte(SlaveAddress_ITG);           //�����豸��ַ+д�ź�
    ITG3205_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
    ITG3205_Start();                          //��ʼ�ź�
    ITG3205_SendByte(SlaveAddress_ITG+1);         //�����豸��ַ+���ź�
    REG_data=ITG3205_RecvByte();              //�����Ĵ�������
	ITG3205_SendACK(1);   
	ITG3205_Stop();                           //ֹͣ�ź�
    return REG_data; 
}

//��ʼ��ITG3205��������Ҫ��ο�pdf�����޸�************************
void InitITG3205()
{
   Single_WriteITG3205(PWR_M, 0x80);   //
   Single_WriteITG3205(SMPL, 0x07);    //
   Single_WriteITG3205(DLPF, 0x1E);    //��2000��
   Single_WriteITG3205(INT_C, 0x00 );  //
   Single_WriteITG3205(PWR_M, 0x00);   //
}		   */
//***********************************************************************


//*********************************************************

//**************************************
//��1602����ʾ10λ����

//**************************************
//��ʾ�¶�
//**************************************
//void display_temp()
//{ 
//	Temp_h=Single_ReadI2C(TEMP_OUT_H); //��ȡ�¶�
//	Temp_l=Single_ReadI2C(TEMP_OUT_L); //��ȡ�¶�
//	Temperature=Temp_h<<8|Temp_l;     //�ϳ��¶�
//	Temperature = 35+ ((double) (Temperature + 13200)) / 280; // ������¶�
//	lcd_printf(dis,Temperature);     //ת��������ʾ
//	DisplayListChar(11,1,dis,4);     //��ʼ�У��У���ʾ���飬��ʾλ��
//}
	   

//�������ĳ����matalb�Ǳ���ֲ������ȥ�Ǳ߿�ע��//
 uchar IMU_i=0;
 uchar IMU_j=0;
 uchar TH1_now,TL1_now;

int u_u_gate=8000;
int u_d_gate=450;
int d_u_gate=-450;
xdata int d_d_gate=-8000;
xdata int gate_init=0;
xdata int path_sum=0;		  //matlab��ֻ�б���path
xdata uchar path_num=0; 
xdata int samepath=20;
xdata uchar path_f=20;
xdata int path_d=0;
xdata int path_max_old=0;
xdata int path_min_old=0;
xdata uchar pre_match=0;
xdata int abnormal=80;
xdata int buf_z[50]={0};   		//matlab�����������buf
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
	Multiple_Read_ADXL345();						 //����������0.57ms
	TH1_now=TH1;TL1_now=TL1;						//��¼��ǰ��������һ�����ݼ��ʱ���
	TH1=0x00;
	TL1=0x00;
	TF1=0;

/*	  BUF_ITG[0]= Single_ReadITG3205(GX_L);
      BUF_ITG[1]= Single_ReadITG3205(GX_H);	//x
      BUF_ITG[2]= Single_ReadITG3205(GY_L);
      BUF_ITG[3]= Single_ReadITG3205(GY_H);	//y
      BUF_ITG[4]= Single_ReadITG3205(GZ_L);
      BUF_ITG[5]= Single_ReadITG3205(GZ_H);	//z		*/

	  Multiple_Read_HMC5883();      //�����������ݣ��洢��BUF_HMC��	   //����������0.56ms

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
/////////////////////////�жϷ���////////////////////////////////////////////////////
	 if (HMC_z>150 && HMC_y<243 && HMC_y>-243)      //222
	  	angle=2;
		else if (HMC_z<-150 && HMC_y<243 && HMC_y>-243)     //-222
			angle =4;
				else if (HMC_y>0)			//245
					angle=1;
					else 
						angle=3;


/*	if (position_x>=11 && position_x<=67)		 //��ʱ�ķ���ֻ����Ϊ1��3
		if (HMC_y>0)
			angle=1;
		else 
			angle=3;
	if (position_y>=11 && position_y<=46)		 //��ʱ�ķ���ֻ����Ϊ2��4
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
	
//	halRfSendPacket(TxBuf,7);			//��������2.76ms��û�෢��һ���ֽڣ����ʱ0.12ms
//	my_delay(1843);

//*********************����buf_z*******************************************	 �������β������ķѴ�Լ0.5ms

	for(IMU_i=0;IMU_i<49;IMU_i++)
	{   buf_z[IMU_i]=buf_z[IMU_i+1];
	}

	buf_z[49]=(BUF_ADXL[1]<<8)+BUF_ADXL[0];
	if(buf_z[49]>=32768)
		{	buf_z[49]=buf_z[49]-65536;			
		}
	buf_z[49]=buf_z[49]*39/10-1024;			 
	for (IMU_i=0;IMU_i<40;IMU_i++)			//ÿ�μ���֮ǰ��buf_sum���㣬matlab�ļ�������ﲻһ��
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
											   //�Ӷ�ȡ��Ϣ���Ȼ����֮�����е����ﻨ��5.57ms
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
//////////////////////////////���õ�ͼ�뷽����ϢУ׼////////////////////////////////////////////////////////////
				if (position_x>400 && position_x<7400 && position_y>400	&& position_y<5300)		 //����������˲�������ֵ�λ�ã���ʱ�����������
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
						writedigital(19,4,34);	  // С����
						Dispchar2(position_x%100,4,31);
						Dispchar2(position_y/100,2,50);
						writedigital(19,2,34);	  // С����
						Dispchar2(position_y%100,2,31);				  

						Dispchar2(path_sum%100,6,54);

						Dispchar2(path_distance/100,6,36);
						writedigital(19,6,20);	  // С����
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

//������Ĵ��ڸ�ֵ��Ϻ����е�������û�н���if����������ʱ�䣬���������if
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
						{	if (KEY2==0)				  //������ߵ����ܣ�����ĳ�㶼����KEY2���ߵ���ͣ������ͬʱ������Ϣ��������
								{	halRfSendPacket(TxBuf,11); delay_ms(2000);}

//							my_delay(120);		//Ϊ�˱��ֲ���Ƶ��100Hz
							if (KEY2!=0)				  //�����Ļ�������Ű������ߵ��Ͳ����й���
							path_test();
						}	
						  
}

void main()
{		uchar i_signal=0;
		uchar ss=1;
		uchar leng=10;
		uchar tt=0;

    
	BELL =1;		 //������
	LED1=0;			 //����
	LED2=0;
//	UART_init();
//    back_lcd=0; 	  //Һ�����⿪
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
//  entermenu();              /*-----������ʾ  ����ѧ��ӭ���� -----*/
//	delay1();
//    delay1();
//    delay1();
//    disdate_a(0x00,0x00);
   back_lcd=1;	  		//Һ�������

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

	
	Init_ADXL345();                 //��ʼ��ADXL345
//	devid=Single_Read_ADXL345(0X00);//����������Ϊ0XE5,��ʾ��ȷ

	Init_HMC5883();
//	InitITG3205();                   //��ʼ��ITG3205


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
		biaohao++;													   //��һ������
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
	  if ((TH1*256+TL1)>=8558 || IMU_j>=1)	  //ʱ��ӦΪ10000us-573us��������Ƭ������Ϊ11.0592MHz	//528��IMU�ķ�ʱ�䣬2857�ǲ�������ʱ��
	  {		if (KEY2!=0)				  //�����Ļ�������Ű������ߵ��Ͳ����й���
	  			path_test();
	  }

	   if (KEY1==0)
	   {
	   draw_position(position_x/100,position_y/100);	 //��һ��ʱ�ܶ࣬����100ms��
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
					{/*	writeone(0,6,60);		  //���		��
						writeone(0,6,44);			//		  ǰ
						writeone(0,6,28);			  //		λ
						writeone(0,6,12);				//		  ��
						writeone(2,6,52);		//	  	��
						writeone(3,6,36);		  //	��
						writeone(4,6,20);			//	��		*/
						i_signal=0;	 
					}
					writedigital(angle,6,65);//Ϊ��ʡ��ʱ�䣬��������ʾ��														
					my_send_data();
					path_num=0;		//�����ڲ���������
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
			{ 		/*	writeone(0,6,52);		//	  	��	   //���
						writeone(0,6,36);		  //	��
						writeone(0,6,20);			//	��	
						writeone(5,6,60);		  //		��
						writeone(6,6,44);			//		  ǰ
						writeone(7,6,28);			  //		λ
						writeone(8,6,12);				//		  ��				   */

//			 Dispchar(ss,3,30);					  //���ԣ������۲��յ��������ص����ݰ�
//			 if(RxBuf[5]==biaohao)											 //���صĵ�һ�����ݰ����ڵ�2345��RSSI  ��������
//			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,120);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,120);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,120);
//				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
		//	 	Dispchar(RxBuf[3],7,120);
		//		Dispchar(RxBuf[4],5,120);
		//		Dispchar(RxBuf[6],3,120);
		//	 	Dispchar(RxBuf[7],1,120);
//				writedigital(RxBuf[1],3,90);//��������ڵ㡣
//				ss++;
//				LED2=~LED2;
//			 }
			 if(RxBuf[5]==biaohao)										//���صĵڶ������ݰ����ڵ�6789��RSSI ��������
			 {	LED2=~LED2;
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[3]),7,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[4]),5,80);
//				Dispchar(RSSI_to_RSSI_dB(RxBuf[6]),3,80);
//			 	Dispchar(RSSI_to_RSSI_dB(RxBuf[7]),1,80);
//				Dispchar(RxBuf[1],1,30);//��������ڵ㡣
		//	 	Dispchar(RxBuf[3],7,90);
		//		Dispchar(RxBuf[4],5,90);
		//		Dispchar(RxBuf[6],3,90);
		//	 	Dispchar(RxBuf[7],1,90);
				writedigital(RxBuf[1],3,90);//��������ڵ㡣
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
/*			 	if(RxBuf[5]==biaohao+2)							 //���صĵ��������ݰ�����λ�����͵�λ����Ϣ
			 {	LED2=~LED2;
//			 	Dispchar(RxBuf[3],3,40);
//				Dispchar(RxBuf[4],5,40);
//				Dispchar(RxBuf[6],3,40);
//			 	Dispchar(RxBuf[7],1,40);
//				Dispchar(RxBuf[1],1,40);//��������ڵ㡣
		//	 	Dispchar(RxBuf[3],7,60);
		//		Dispchar(RxBuf[4],5,60);
		//		Dispchar(RxBuf[6],3,60);
		//	 	Dispchar(RxBuf[7],1,60);
		//		Dispchar(RxBuf[1],3,120);//��������ڵ㡣
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