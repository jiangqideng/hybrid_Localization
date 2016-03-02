#ifndef server_H
#define server_H
#define uchar unsigned char
void UART_init();
void Sendchar(unsigned char cha);
void print_string(unsigned char* p);
void Send_int8(int int8);
uchar UART_ReceiveChar();
void delay_ms(unsigned int n);
void delay_1ms(void);
#endif
