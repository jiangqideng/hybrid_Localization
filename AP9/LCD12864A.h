
//----------------------LCD IO Define----------------------------------
sbit    cs1=P3^6;
sbit    rst=P3^5;
sbit    a0=P3^4;
sbit    sclk=P3^3;
sbit    di=P3^2;

sbit    back_lcd=P1^5;        //---------------Һ������------------------



//unsigned char bdata flag;/*��λѰַ��*/
//unsigned int *p;
#define  nop()      _nop_()   /*�����ָ��*/

void lcd_init(void);
void delay_lcd(void);
void delay1(void);  

void wcom(unsigned char com);
void wdata(unsigned char dat);
void write(unsigned char a);


void disdate_a(unsigned char dat_a,unsigned char dat_b);
void disdate_b(unsigned char *p);
void cwpr(unsigned char cod,unsigned char page,unsigned char seg);
void ccwpr(unsigned char cod,unsigned char page,unsigned char seg);
void writedigital(unsigned char cod,unsigned char page,unsigned char seg);
void writeone(unsigned char cod,unsigned char page,unsigned char seg);
void writeonebig(unsigned char cod,unsigned char page,unsigned char seg);


void clr_blank();
void entermenu();  
void Dispchar(int,unsigned char page,unsigned char seg);