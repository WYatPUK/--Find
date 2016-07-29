#include <reg51.h>

char Str[30];
unsigned int Strlen;
void UsartConfiguration();
unsigned int SendData;
unsigned int ReceiveData;
unsigned char SendOrder;
unsigned char ReceiveOrder;
void SendCharHex(char x)
{
	unsigned char low;
	unsigned char high;
	low = x&0x0f;
	high = (x>>4)&0x0f;
	if (low<10) low = '0'+low;
	else low = 'A'+low-10;
	if (high<10) high = '0'+high;
	else high = 'A'+high-10;
	SBUF=high;
	while(!TI);
	TI=0;
	SBUF=low;
	while(!TI);
	TI=0;	
}
unsigned char CharToHex(char high, char low)
{
	unsigned char x; 
	if (high >= '0' && high <= '9')
	{
		x = (high - '0')<<4;
	}
	else if (high >= 'a' && high <= 'f')
	{
		x = (high - 'a' + 10)<<4;
	}
	else if (high >= 'A' && high <= 'F')
	{
		x = (high - 'A' + 10)<<4;
	}
	else return 0;
	if (low >= '0' && low <= '9')
	{
		x |= (low - '0');
	}
	else if (low >= 'a' && low <= 'f')
	{
		x |= (low - 'a' + 10);
	}
	else if (low >= 'A' && low <= 'F')
	{
		x |= (low - 'A' + 10);
	}
	else return 0;
	return x;	
}
void Send_String(char* x)
{
	int i;
	for (i=0; x[i]!=0 && i<10; i++)
	{
		SBUF=x[i];
		while (!TI);
		TI=0;
	}
}
void main()
{
	UsartConfiguration();
	Strlen=0;
	while(1);
}
void UsartConfiguration()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XFA;
	//TH1=0XF3;		    //计数器初始值设置，注意波特率是4800的
	//TL1=0XF3;
	TL1=0XFA;			 //调成了9600波特率
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					    //打开计数器
	ET1=0;
}
int nearistp;
unsigned int R;//10位绝对值，6位数量级
unsigned int order;//8位命令
unsigned int x;
#include "Find_Struct_testing.h"
void Do()
{
	unsigned char D_high, D_low;
	D_high = CharToHex(Str[0],Str[1]);
	D_low = CharToHex(Str[2],Str[3]);
	ReceiveData = D_high<<8 | D_low;
	ReceiveOrder = CharToHex(Str[4], Str[5]);
	Send_String("Data is ");
	SendCharHex((ReceiveData>>8)&0xff);
	SendCharHex(ReceiveData&0xff);
	Send_String(" Order is ");
	SendCharHex(ReceiveOrder);
	
	x = ReceiveData;
	nearistp = Find_Nearist(x);

	Send_String("\r\nnear:");
	SendCharHex((nearistp>>8)&0xff);
	SendCharHex((nearistp)&0xff);

	SendData = SListR[nearistp];
	Send_String("\r\nF:Num=");
	SendCharHex((SendData>>(8+6))&0x03);
	SendCharHex((SendData>>6)&0xff);
	Send_String("  Index=");
	SendCharHex(SendData&0x3f);
	SendOrder = 0x21;
	Send_String("\r\nReturn ");
	SendCharHex((SendData>>8)&0xff);
	SendCharHex(SendData&0xff);
	Send_String(" Order is ");
	SendCharHex(SendOrder);
}
void DoUSART (unsigned char x)
{
	switch(x)
	{
		case '*':
			Strlen=0;
			return;
		case '&':
			Send_String("\r\nlength=");
			SendCharHex(Strlen & 0xff);
			Send_String("\r\n");
			if(Strlen==6) Do();
			Strlen=0;
			return;
		default:
			Str[Strlen] = x;
			Strlen++;
			return;
	}
}
void Usart() interrupt 4
{
	unsigned char receiveData;
	receiveData=SBUF; //出去接收到的数据
	RI = 0;           //清除接收中断标志位
	DoUSART(receiveData);
	//SBUF=receiveData; //将接收到的数据放入到发送寄存器
	//while(!TI);		  //等待发送数据完成
	//TI=0;			  //清除发送完成标志位
}