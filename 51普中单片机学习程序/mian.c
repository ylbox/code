#include <reg51.h>
#include <intrins.h>
#include <i2c.h>

#define A P2                                //定义一组I/O口，方便对多处调用同一I/O的修改（便于程序移植）            //#define宏定义，用于定义常量（包括无参量与带参量）
#define LED P0                             //八段数码管I/O口定义
#define GPIO_Key P1                        //矩阵按键I/O口定

typedef unsigned int uint16;
typedef unsigned long int uint32;
typedef unsigned char uchar8;


sbit FM=P1^5;                             //定义蜂鸣器I/O口

sbit LSA=P2^2;                            //定义数码管片选信号I/O口
sbit LSB=P2^3;                            //A为低位，B为高位
sbit LSC=P2^4;


sbit Led2=P2^2;                          //定义独立I/O口,便于对独立I/O口的辨识 

sbit K2=P3^0; 
sbit K1=P3^1;                            //定义独立按键          文件全局变量的定义，该变量仅在该.c文件中被全局引用                    
sbit K3=P3^2;
sbit K4=P3^3; 



sbit SER=P3^4;                              //DS-串行数据输入
sbit RCLK=P3^5;                            //STCP-存储寄存器时钟（触发工作的信号）输入              
sbit SRCLK=P3^6;                           //SHCP-移位寄存器时钟（触发工作的信号）输入
 

void Delay_s(uint16 n);                    //12M晶振延迟n秒                  //延时函数的定义
void Delay_ms(uint16 n);                   //12M晶振延迟n毫秒                
void Delay_us(uint16 n);                  //12M晶振延迟n/100毫妙

void Keydown();                           //4×4矩阵键盘循环扫描，从左上到右下，依次为数码管数组中的0~F
uchar8  Keyvalue;

void Hc595SendByte(uchar8 dat);               //595先传送高位，再传送低位

void Key_Led(void);                      //按键控制LED点亮和熄灭
void LED_Key();                          //应用4×4键盘使数码管对用显示0~F
void Key_Led(void);                      //按键控制LED点亮

void smgj(void);                         //静态数码管显示 
void smgd1(void);                        //静态数码管循环显示0~F 
void smgd2(void);                        //8个数码管流水循环显示0~7

void FMQ(void);                          //蜂鸣器程序

void Keypros();
void DigDisplay();
void datapros();

void Int0Init();                          //外部中断0初始化程序
                                          //运用外部中断0控制按键K3实现对LED灯D2的亮灭控制
void Time0Init();                          //定时/计数器T0初始化程序————需在主函数中引用改函数
                                          //运用定时/计数器TO实现对LED灯D2的循环点亮
void UsartInit();                         //串口通信初始化函数                         
                                          //运用定时器实现串口通信

void LED_one(void);                      //独立Led的点亮                    //执行程序的定义
void LED_cror(void);                     //运用循环移位函数点LED流水灯
void LED_crol1(void);                    //运用数组实现LED流水灯
void LED_crol2(void);                   //计数型流水灯

void HC595_send_data(uchar8 dat);      //LED点阵行扫描
void Led_dz1(void);	                   //led点阵点亮左上角灯
void LED_dianzhen_gexingzi(void);                                     //运用LED点阵自定义显示汉字
uchar8 code Led_duan[]={0x00,0x02,0x52,0x7e,0x52,0x7e,0x02,0x00};                   //汉字“丑”
//uchar8 code Led_duan[]={0xff,0x91,0xaf,0x8b,0x8b,0xaf,0x91,0xff};                 //汉字“囧”
//uchar8 code Led_duan[]={0x3c,0x42,0x00,0x42,0x24,0x18,0x24,0x42};                //字母 CX
//uchar8 code Led_duan[]={0x00,0x00,0x3e,0x41,0x41,0x41,0x3e,0x00};                //LED点阵显示零，段选信号（选中某几行）---按列读16位值--改变此数组，达到显示不同字的目地
uchar8 code Led_wei[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};                   //LED点阵显示零，位选信号（选中某列）---按行读16位值---本数组不变

uchar8 code smgduan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                       0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};             //七段数码管对应0~F显示值数组               在.c文件中定义数组，数组可存储在Flash中（定义时可包含code）
uchar8 num=0,disp[4];

					   
void main()
{
	while(1)
	{
	  Keypros();
	  datapros();
	  DigDisplay();
	}
}

void Keypros()
{
	if(K1==0)
	{
		Delay_ms(5);
		if(K1==0)
		{
			At24c02Write(1,num);
		}
	    while(!K1);
	}
	if(K2==0)
	{
		Delay_ms(5);
		if(K2==0)
		{
			num=At24c02Read(1);
		}	
		while(!K2);
	}
	if(K3==0)
	{
		Delay_ms(5);
		if(K3==0)
		{
			num++;
//			if(num>255)num=0;     //if单语句执行命令写法
			if(num>255)           //if语句的完整用法
			{
				num=0;
			}
		}
	    while(!K3);
	}	
	if(K4==0)
	{
		Delay_ms(5);
		if(K4==0)
		{
			num=0;
		}	
		while(!K4);
	}	
}

void DigDisplay()
{
	uchar8 i;
	for(i=0;i<4;i++)
	{
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//显示第0位
			case(1):
				LSA=1;LSB=0;LSC=0; break;//显示第1位
			case(2):
				LSA=0;LSB=1;LSC=0; break;//显示第2位
			case(3):
				LSA=1;LSB=1;LSC=0; break;//显示第3位	
		}
		P0=disp[3-i];//发送数据
		Delay_ms(5); //间隔一段时间扫描	
		P0=0x00;//消隐
	}		
}
	
void datapros()
{
	disp[0]=smgduan[num/1000];//千位
	disp[1]=smgduan[num%1000/100];//百位
	disp[2]=smgduan[num%1000%100/10];//个位
	disp[3]=smgduan[num%1000%100%10];		
}

void UsartInit()                                                   //串口通信初始化函数 
{                                                                  //运用定时器实现串口通信
	TMOD=0x20;                                                     //确定T1的工作方式0010 0000——选择T1定时/计数器 选择定时模式 设置工作方式为方式2  
	TH1=0xf3;                                                    //配置定时器初值 配置寄存器高四位=程序计算值——可用51波特率初值设定小程序计算
	TL1=0xf3;                                                   //               配置寄存器低四位=其值与高四位相同
	PCON=0x80;                                                  //配置PCON寄存器——选择是否进行波特率倍增位（SMOD）1000 0000——在方式1/2/3中，波特率与SMOD有关——当SMOD=1时，波特率提高一倍；复位时，SMOD=0
	TR1=1;                                                     //启动T1（编程TCON中的TR1位） 开定时器，TR0=1时，定时器开始工作
	SCON=0x50;                                                 //确定串行口控制（编程SCON寄存器）0101 0000 SM0=0，SM1=1选择控制方式为方式1 SM2=0关闭多机通信 REN=1启动串行口接收数据
	ES=1;                                                     //开串行口中断允许位
	EA=1;                                                     //开CPU中断允许（总允许位）
}

void Usart() interrupt 4                                       //串口通信服务函数              
{
	uchar8 receiveData;                                        //定义数据存储变量
	receiveData=SBUF;                                          //PC发送数据到SBUF（此时充当数据接收缓存器）后，将其赋值给变量进行储存
	RI=0;                                                     //一帧数据传输结束会出现停止位（RI接收中断标志位），内部硬件会自动使RI=1，向中断发出中断申请——为便于下次数据的接收，故此处需软件回零
	SBUF=receiveData;                                         //变量将值赋给SBUF（此时充当数据发送缓存器），并由PC读取SBUF中的数据，达到向PC发送数据的目的
	while(!TI);                                               //数据未发送完时TI=0；程序会停在该处，等待数据发送——当数据发送结束时，出现发送停止位，此时内部硬件自动使TI=1，程序跳出该语句，同时CPU发出中断申请
	TI=0;                                                     //一帧数据传输结束会出现停止位（TI发送中断标志位），内部硬件会自动使TI=1，向中断发出中断申请——为便于下次数据的发送，故此处需软件回零
}

//void main（）                                                   //中断/定时器正解
//{
//	Time0Init();
//	while(1);                                                    //引用外部中断或者定时器时，必须有此while结构体语句
//}


//void main（）                                                   //中断/定时器错误写法
//{
//	while(1)                                                      //引用外部中断或者定时器时，不能采用此while结构体语句
//	{
//	  Time0Init();
//	}                                                    
//}


void Time0Init()                                                     //定时/计数器T0初始化函数————需在主函数中引用改函数
{                                                                   //运用定时/计数器TO实现对LED灯D2的循环点亮
//  TMOD=0x01;
	TMOD|=0x01;                                                     //此法比较上条语句可增加程序稳定性 工作方式寄存器配置——0000 0001——选择T0定时/计数器 选择定时模式 设置工作方式为方式1  GATE（门空位）=0时，控制定时器的启动受外部中断源信号的影响，只要用软件使TCON中的TR0或TR1为1，才可以启动定时/计数器
	TH0=0xfc;                                                     //配置定时器初值 配置寄存器高四位——初值=65536-定时时间=65535-定时时间+1                                                   GATE（门空位）=1时，要用软件使TR0或TR1为1，同时外部中断引脚INT0/1也为高电平时，才能启动定时/计数器工作
	TL0=0x18;                                                                    //配置定时器低四位 
	ET0=1;                                                     //定时/计数器中断允许位
	EA=1;                                                     //CPU中断允许位（总允许位=中断总开关）
	TR0=1;                                                     //开定时器，TR0=1时，定时器开始工作
}

void Time0() interrupt 1                                      //定时/计数器服务函数——中断发生时自动执行该子函数——主函数中无需调用
{
	static uint16 i;                                         //局部(仅在该子函数中定义的变量)静态变量，此处仅在程序内部引用，只会在程序首次执行时被初始化，程序执行一次后会保留新值，作为下次程序执行的初始值
	TH0=0xfc;                                                //配置定时器初值 配置寄存器高四位
	TL0=0x18;                                                              //配置定时器低四位——此工作方式下（方式1），无法自动重装初值，故需在中断执行子函数中手动重装初值
	i++;                                                    //当计数器计满，发生溢出时跳转至该子函数，执行到此语句时，跳出中断，每次该子函数执行一次，表示已经计数1ms（i执行i+1操作），
	if(i==1000)                                                    //当i累加至1000（即1s）时，执行该if语句，实现LED灯D1的状态跳变
	{
		i=0;                                                    //每次LED状态跳变后，需将i清零，让其重新计数，便于计算下次跳变时间
		Led2=~Led2;
	}
}


void Int0Init()                                                //外部中断0初始化程序——需在主函数中引用改函数——主函数中无需调用
{                                                              //运用外部中断0控制按键K3实现对LED灯D1的亮灭控制
	IT0=1;                                                     //外部中断0触发方式控制位——IT0=0时为低电平触发；IT0=1时为下降沿触发
	EX0=1;                                                     //外部中断0中断控制标志位
	EA=1;                                                     //CPU中断允许（总允许位=中断总开关）
}

void Int0() interrupt 0                                      // 中断服务函数——中断发生时，需要执行的动作（主函数中不调用该子函数）     //void   Int0()         interrupt       0
{                                                                                                                                   //      中断函数名                     中断号
	Delay_ms(5);
	if(K3==0)
	{
		Led2=~Led2;
	}
}


void LED_dianzhen_gexingzi(void)                                     //运用LED点阵自定义显示汉字
{
		uchar8 i;
		LED=0x7f;
		for(i=0;i<8;i++)
		{
			LED=Led_wei[i];                 //进行列扫描
			Hc595SendByte(Led_duan[i]);     //进行行扫描
			Delay_us(100);
			Hc595SendByte(0x00);            //对点阵清屏
		}
}
											 
void Led_dz1(void) 	                   //led点阵点亮左上角灯
{   
    Hc595SendByte(0x80);                      //选择行
    LED=0x7f;                                  //选择列
}


void Hc595SendByte(uchar8 dat)           //行选择/段选函数       //HC595先传送高位，再传送低位  当OE（JOE）为低电平时芯片工作
{
	uchar8 i;
	SRCLK=0;                                  //SHCP-移位寄存器时钟（触发工作的信号）输入
	RCLK=0;                                   //STCP-存储寄存器时钟（触发工作的信号）输入  
	for(i=0;i<8;i++)                            //执行8次后将8位数据存储至移位数据存储寄存器  
	{
		SER=dat>>7;                             //a=0时，发送最高位，如：dat=10101110 ,则右移七次后，仅剩下最高位的1，
		dat<<=1;                                                         //高位——低位
		SRCLK=1; 
		_nop_();                               //执行一次延时一个机器周期
		_nop_();                
		SRCLK=0;                                //将移位寄存器拉低，方便产生下次上升沿
	}                                        //循环8次，将8位数据全部存储到了存储数据寄存器中
	  RCLK=1;                               //将存储数据寄存器中的数据发送给HC595芯片输出口
		_nop_();
		_nop_(); 
	  RCLK=0;
}


void LED_Key()                               //应用4×4键盘使数码管对用显示0~F
{
	LSA=0;
	LSB=0;
	LSC=0;
	while(1)
	{
		Keydown();
		LED=smgduan[Keyvalue];
		Delay_ms(20); 
	}
}

void Keydown(void)                           //4×4矩阵键盘循环扫描，从左上到右下，依次为数码管数组中的0~F
{
//	static 	uchar8 a=0;                     //局部静态变量，此处仅在程序内部引用，只会在程序首次执行时被初始化，程序执行一次后会保留新值，作为下次程序执行的初始值
	uchar8 a=0;                               //定义局部变量，只在该程序内部引用,每次程序执行开始都会被初始化
	GPIO_Key=0x0f;
	if(GPIO_Key!=0x0f)
	{
		Delay_ms(10);                           //按下消抖                 
		if(GPIO_Key!=0x0f)                      //再次确认按键确实按下
		{
			GPIO_Key=0x0f;                         //按键端口重新赋值
			switch(GPIO_Key)                      //列检测      //扫描是哪一列有按键按下
			{
				case(0x07):Keyvalue=0;break;
				case(0x0b):Keyvalue=1;break;
				case(0x0d):Keyvalue=2;break;
				case(0x0e):Keyvalue=3;break;	
			}
			GPIO_Key=0xf0;                        //行检测     //扫描是哪一行有按键按下
			switch(GPIO_Key)
			{
				case(0x70):Keyvalue=Keyvalue;break;
				case(0xb0):Keyvalue=Keyvalue+4;break;
				case(0xd0):Keyvalue=Keyvalue+8;break;
				case(0xe0):Keyvalue=Keyvalue+12;break;	
			}
			while((a<50)&&(GPIO_Key!=0xf0))        //消抖，同时检测按键超时与否，防止持续按键占用CPU
			{
				Delay_ms(10);
				a++;
			}
		}
	}
}	


void Key_Led(void)                          //按键控制LED点亮和熄灭
{	
	   if(K1==0)  
		{	
			Delay_ms(10);	                              //按下消抖
			if(K1==0)
			{	while(!K1);                            //等待按键松开。
				Led2=~Led2;
			}
		}
}
 void smgj(void)                               //静态数码管显示           //74H573（J21）芯片，LE为锁存端，当LE与P10短接，表示锁存器状态由I/O口控制，当LE与VCC短接时，表示由外部电源控制
	                                                                                             //当LE为高电平时，进行数据的采集，D0~7的值与Q0~7一一对应，当LE为低电平时，会锁存之前的数据，保持不变
{
	LSA=0;
	LSB=0;
	LSC=0;
	LED=smgduan[0];                            //调用数组第一个元素，使数码管显示零
}



void smgd1(void)                             //静态数码管循环显示0~F 
{
	uchar8 i;
	LSA=0;
	LSB=0;
	LSC=0;
	for(i=0;i<17;i++)
	{
	  LED=smgduan[i];
		Delay_s(1);
	}                                          //调用数组第一个元素，使数码管显示零
}



void smgd2(void)                             //8个数码管流水循环显示0~7
{
	uchar8 i;
	for(i=0;i<8;i++)
	{
		switch(i)
		{
			case(0):
				      LSA=0;LSB=0;LSC=0;LED=smgduan[i];Delay_s(1);break;
			case(1):
				      LSA=1;LSB=0;LSC=0;LED=smgduan[i];Delay_s(1);break;
			case(2):
				      LSA=0;LSB=1;LSC=0;LED=smgduan[i];Delay_s(1);break;
			case(3):
				      LSA=1;LSB=1;LSC=0;LED=smgduan[i];Delay_s(1);break;
			case(4):
				      LSA=0;LSB=0;LSC=1;LED=smgduan[i];Delay_s(1);break;
			case(5):
				      LSA=1;LSB=0;LSC=1;LED=smgduan[i];Delay_s(1);break;
			case(6):
				      LSA=0;LSB=1;LSC=1;LED=smgduan[i];Delay_s(1);break;
			case(7):
				      LSA=1;LSB=1;LSC=1;LED=smgduan[i];Delay_s(1);break;
		}
	}
}



void FMQ(void)                                  //蜂鸣器程序
{
	FM=0;
   	Delay_us(10);	
  	FM=~FM;
		Delay_us(10);                              //调节I/O口输出波形占空比
}



void LED_one(void)                              //独立Led的点亮与熄灭
{
	  Led2=0;
		Delay_ms(100); 
		Led2=~Led2;
		Delay_ms(100); 
}



void LED_cror(void)                             //运用循环移位函数点亮LED流水灯   循环右移
{
	while(1)
	{
    uint16 i;
	  A=0xfe;
	  Delay_ms(50);
	  for(i=0;i<7;i++)
	    { 
	      A=_cror_(A,1);                         //循环右移
		    Delay_ms(20);
	    }
			
	}
}



void LED_crol1(void)                            //运用数组实现LED流水灯   循环左移
{
	uchar8 table[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};   //数组的定义
	while(1)
	{
		uint16 n;
		for(n=0;n<8;n++)
		{
			A=table[n];                              //调用数组中第n个数
			Delay_ms(20);
		}
	}
}



void LED_crol2(void)                           //计数型流水灯
{
	uchar8 m=255;                
	while(1)
     {
       A=m;
			 Delay_ms(50);
			 m--;
     }
}


void Delay_s(uint16 n)                         //12M晶振延迟n秒
{
	uint16 i=0;
	uint32 j=0;
	for(i=0;i<n;i++)
	    for(j=0;j<21738;j++);
}



void Delay_ms(uint16 n)                         //12M晶振延迟n毫秒
{     
	uint16 i=0;
	uint32 j=0;
	for(i=0;i<n;i++)
	    for(j=0;j<123;j++);
}


  
void Delay_us(uint16 n)                         //12M晶振延迟n/100毫秒
{
	while(n--);
}	