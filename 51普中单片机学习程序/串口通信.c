#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;

void main()
{
   TMOD=0X20;	//0010 0000 工作方式2，8位自动重装载定时器
   TH1=0XF3;	//用软件算出的值为0xf3，此时SMOD为1，波特率为4800，误差为0.16%，较精确
   TL1=0XF3;

   PCON=0X80;	 //SMOD为1
   SCON=0X50;  	//0101 0000 工作方式1，10位异步收发器，REN=1				
   EA=1;
   ES=1;
   TR1=1;		//没有ET1=1这条语句，原因现在T1就相当于是一个波特率发生器，不需要T1中断
   while(1);
}

void chuanhangkou() interrupt 4
{
   uchar receieve_Date;
   receieve_Date=SBUF;		//此处的SBUF是接收缓冲器，接收完一帧数据后，硬件置RI为1，需用软件清零
   RI=0;

   SBUF=receieve_Date;		  //此处的SBUF是发送缓冲器
   while(!TI);
   TI=0;
} 