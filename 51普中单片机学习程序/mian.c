#include <reg51.h>
#include <intrins.h>
#include <i2c.h>

#define A P2                                //����һ��I/O�ڣ�����Զദ����ͬһI/O���޸ģ����ڳ�����ֲ��            //#define�궨�壬���ڶ��峣���������޲������������
#define LED P0                             //�˶������I/O�ڶ���
#define GPIO_Key P1                        //���󰴼�I/O�ڶ�

typedef unsigned int uint16;
typedef unsigned long int uint32;
typedef unsigned char uchar8;


sbit FM=P1^5;                             //���������I/O��

sbit LSA=P2^2;                            //���������Ƭѡ�ź�I/O��
sbit LSB=P2^3;                            //AΪ��λ��BΪ��λ
sbit LSC=P2^4;


sbit Led2=P2^2;                          //�������I/O��,���ڶԶ���I/O�ڵı�ʶ 

sbit K2=P3^0; 
sbit K1=P3^1;                            //�����������          �ļ�ȫ�ֱ����Ķ��壬�ñ������ڸ�.c�ļ��б�ȫ������                    
sbit K3=P3^2;
sbit K4=P3^3; 



sbit SER=P3^4;                              //DS-������������
sbit RCLK=P3^5;                            //STCP-�洢�Ĵ���ʱ�ӣ������������źţ�����              
sbit SRCLK=P3^6;                           //SHCP-��λ�Ĵ���ʱ�ӣ������������źţ�����
 

void Delay_s(uint16 n);                    //12M�����ӳ�n��                  //��ʱ�����Ķ���
void Delay_ms(uint16 n);                   //12M�����ӳ�n����                
void Delay_us(uint16 n);                  //12M�����ӳ�n/100����

void Keydown();                           //4��4�������ѭ��ɨ�裬�����ϵ����£�����Ϊ����������е�0~F
uchar8  Keyvalue;

void Hc595SendByte(uchar8 dat);               //595�ȴ��͸�λ���ٴ��͵�λ

void Key_Led(void);                      //��������LED������Ϩ��
void LED_Key();                          //Ӧ��4��4����ʹ����ܶ�����ʾ0~F
void Key_Led(void);                      //��������LED����

void smgj(void);                         //��̬�������ʾ 
void smgd1(void);                        //��̬�����ѭ����ʾ0~F 
void smgd2(void);                        //8���������ˮѭ����ʾ0~7

void FMQ(void);                          //����������

void Keypros();
void DigDisplay();
void datapros();

void Int0Init();                          //�ⲿ�ж�0��ʼ������
                                          //�����ⲿ�ж�0���ư���K3ʵ�ֶ�LED��D2���������
void Time0Init();                          //��ʱ/������T0��ʼ�����򡪡��������������������øĺ���
                                          //���ö�ʱ/������TOʵ�ֶ�LED��D2��ѭ������
void UsartInit();                         //����ͨ�ų�ʼ������                         
                                          //���ö�ʱ��ʵ�ִ���ͨ��

void LED_one(void);                      //����Led�ĵ���                    //ִ�г���Ķ���
void LED_cror(void);                     //����ѭ����λ������LED��ˮ��
void LED_crol1(void);                    //��������ʵ��LED��ˮ��
void LED_crol2(void);                   //��������ˮ��

void HC595_send_data(uchar8 dat);      //LED������ɨ��
void Led_dz1(void);	                   //led����������Ͻǵ�
void LED_dianzhen_gexingzi(void);                                     //����LED�����Զ�����ʾ����
uchar8 code Led_duan[]={0x00,0x02,0x52,0x7e,0x52,0x7e,0x02,0x00};                   //���֡���
//uchar8 code Led_duan[]={0xff,0x91,0xaf,0x8b,0x8b,0xaf,0x91,0xff};                 //���֡��塱
//uchar8 code Led_duan[]={0x3c,0x42,0x00,0x42,0x24,0x18,0x24,0x42};                //��ĸ CX
//uchar8 code Led_duan[]={0x00,0x00,0x3e,0x41,0x41,0x41,0x3e,0x00};                //LED������ʾ�㣬��ѡ�źţ�ѡ��ĳ���У�---���ж�16λֵ--�ı�����飬�ﵽ��ʾ��ͬ�ֵ�Ŀ��
uchar8 code Led_wei[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};                   //LED������ʾ�㣬λѡ�źţ�ѡ��ĳ�У�---���ж�16λֵ---�����鲻��

uchar8 code smgduan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
                       0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};             //�߶�����ܶ�Ӧ0~F��ʾֵ����               ��.c�ļ��ж������飬����ɴ洢��Flash�У�����ʱ�ɰ���code��
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
//			if(num>255)num=0;     //if�����ִ������д��
			if(num>255)           //if���������÷�
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
		switch(i)	 //λѡ��ѡ�����������ܣ�
		{
			case(0):
				LSA=0;LSB=0;LSC=0; break;//��ʾ��0λ
			case(1):
				LSA=1;LSB=0;LSC=0; break;//��ʾ��1λ
			case(2):
				LSA=0;LSB=1;LSC=0; break;//��ʾ��2λ
			case(3):
				LSA=1;LSB=1;LSC=0; break;//��ʾ��3λ	
		}
		P0=disp[3-i];//��������
		Delay_ms(5); //���һ��ʱ��ɨ��	
		P0=0x00;//����
	}		
}
	
void datapros()
{
	disp[0]=smgduan[num/1000];//ǧλ
	disp[1]=smgduan[num%1000/100];//��λ
	disp[2]=smgduan[num%1000%100/10];//��λ
	disp[3]=smgduan[num%1000%100%10];		
}

void UsartInit()                                                   //����ͨ�ų�ʼ������ 
{                                                                  //���ö�ʱ��ʵ�ִ���ͨ��
	TMOD=0x20;                                                     //ȷ��T1�Ĺ�����ʽ0010 0000����ѡ��T1��ʱ/������ ѡ��ʱģʽ ���ù�����ʽΪ��ʽ2  
	TH1=0xf3;                                                    //���ö�ʱ����ֵ ���üĴ�������λ=�������ֵ��������51�����ʳ�ֵ�趨С�������
	TL1=0xf3;                                                   //               ���üĴ�������λ=��ֵ�����λ��ͬ
	PCON=0x80;                                                  //����PCON�Ĵ�������ѡ���Ƿ���в����ʱ���λ��SMOD��1000 0000�����ڷ�ʽ1/2/3�У���������SMOD�йء�����SMOD=1ʱ�����������һ������λʱ��SMOD=0
	TR1=1;                                                     //����T1�����TCON�е�TR1λ�� ����ʱ����TR0=1ʱ����ʱ����ʼ����
	SCON=0x50;                                                 //ȷ�����пڿ��ƣ����SCON�Ĵ�����0101 0000 SM0=0��SM1=1ѡ����Ʒ�ʽΪ��ʽ1 SM2=0�رն��ͨ�� REN=1�������пڽ�������
	ES=1;                                                     //�����п��ж�����λ
	EA=1;                                                     //��CPU�ж�����������λ��
}

void Usart() interrupt 4                                       //����ͨ�ŷ�����              
{
	uchar8 receiveData;                                        //�������ݴ洢����
	receiveData=SBUF;                                          //PC�������ݵ�SBUF����ʱ�䵱���ݽ��ջ��������󣬽��丳ֵ���������д���
	RI=0;                                                     //һ֡���ݴ�����������ֹͣλ��RI�����жϱ�־λ�����ڲ�Ӳ�����Զ�ʹRI=1�����жϷ����ж����롪��Ϊ�����´����ݵĽ��գ��ʴ˴����������
	SBUF=receiveData;                                         //������ֵ����SBUF����ʱ�䵱���ݷ��ͻ�������������PC��ȡSBUF�е����ݣ��ﵽ��PC�������ݵ�Ŀ��
	while(!TI);                                               //����δ������ʱTI=0�������ͣ�ڸô����ȴ����ݷ��͡��������ݷ��ͽ���ʱ�����ַ���ֹͣλ����ʱ�ڲ�Ӳ���Զ�ʹTI=1��������������䣬ͬʱCPU�����ж�����
	TI=0;                                                     //һ֡���ݴ�����������ֹͣλ��TI�����жϱ�־λ�����ڲ�Ӳ�����Զ�ʹTI=1�����жϷ����ж����롪��Ϊ�����´����ݵķ��ͣ��ʴ˴����������
}

//void main����                                                   //�ж�/��ʱ������
//{
//	Time0Init();
//	while(1);                                                    //�����ⲿ�жϻ��߶�ʱ��ʱ�������д�while�ṹ�����
//}


//void main����                                                   //�ж�/��ʱ������д��
//{
//	while(1)                                                      //�����ⲿ�жϻ��߶�ʱ��ʱ�����ܲ��ô�while�ṹ�����
//	{
//	  Time0Init();
//	}                                                    
//}


void Time0Init()                                                     //��ʱ/������T0��ʼ�����������������������������øĺ���
{                                                                   //���ö�ʱ/������TOʵ�ֶ�LED��D2��ѭ������
//  TMOD=0x01;
	TMOD|=0x01;                                                     //�˷��Ƚ������������ӳ����ȶ��� ������ʽ�Ĵ������á���0000 0001����ѡ��T0��ʱ/������ ѡ��ʱģʽ ���ù�����ʽΪ��ʽ1  GATE���ſ�λ��=0ʱ�����ƶ�ʱ�����������ⲿ�ж�Դ�źŵ�Ӱ�죬ֻҪ�����ʹTCON�е�TR0��TR1Ϊ1���ſ���������ʱ/������
	TH0=0xfc;                                                     //���ö�ʱ����ֵ ���üĴ�������λ������ֵ=65536-��ʱʱ��=65535-��ʱʱ��+1                                                   GATE���ſ�λ��=1ʱ��Ҫ�����ʹTR0��TR1Ϊ1��ͬʱ�ⲿ�ж�����INT0/1ҲΪ�ߵ�ƽʱ������������ʱ/����������
	TL0=0x18;                                                                    //���ö�ʱ������λ 
	ET0=1;                                                     //��ʱ/�������ж�����λ
	EA=1;                                                     //CPU�ж�����λ��������λ=�ж��ܿ��أ�
	TR0=1;                                                     //����ʱ����TR0=1ʱ����ʱ����ʼ����
}

void Time0() interrupt 1                                      //��ʱ/�����������������жϷ���ʱ�Զ�ִ�и��Ӻ����������������������
{
	static uint16 i;                                         //�ֲ�(���ڸ��Ӻ����ж���ı���)��̬�������˴����ڳ����ڲ����ã�ֻ���ڳ����״�ִ��ʱ����ʼ��������ִ��һ�κ�ᱣ����ֵ����Ϊ�´γ���ִ�еĳ�ʼֵ
	TH0=0xfc;                                                //���ö�ʱ����ֵ ���üĴ�������λ
	TL0=0x18;                                                              //���ö�ʱ������λ�����˹�����ʽ�£���ʽ1�����޷��Զ���װ��ֵ���������ж�ִ���Ӻ������ֶ���װ��ֵ
	i++;                                                    //���������������������ʱ��ת�����Ӻ�����ִ�е������ʱ�������жϣ�ÿ�θ��Ӻ���ִ��һ�Σ���ʾ�Ѿ�����1ms��iִ��i+1��������
	if(i==1000)                                                    //��i�ۼ���1000����1s��ʱ��ִ�и�if��䣬ʵ��LED��D1��״̬����
	{
		i=0;                                                    //ÿ��LED״̬������轫i���㣬�������¼��������ڼ����´�����ʱ��
		Led2=~Led2;
	}
}


void Int0Init()                                                //�ⲿ�ж�0��ʼ�����򡪡����������������øĺ����������������������
{                                                              //�����ⲿ�ж�0���ư���K3ʵ�ֶ�LED��D1���������
	IT0=1;                                                     //�ⲿ�ж�0������ʽ����λ����IT0=0ʱΪ�͵�ƽ������IT0=1ʱΪ�½��ش���
	EX0=1;                                                     //�ⲿ�ж�0�жϿ��Ʊ�־λ
	EA=1;                                                     //CPU�ж�����������λ=�ж��ܿ��أ�
}

void Int0() interrupt 0                                      // �жϷ����������жϷ���ʱ����Ҫִ�еĶ������������в����ø��Ӻ�����     //void   Int0()         interrupt       0
{                                                                                                                                   //      �жϺ�����                     �жϺ�
	Delay_ms(5);
	if(K3==0)
	{
		Led2=~Led2;
	}
}


void LED_dianzhen_gexingzi(void)                                     //����LED�����Զ�����ʾ����
{
		uchar8 i;
		LED=0x7f;
		for(i=0;i<8;i++)
		{
			LED=Led_wei[i];                 //������ɨ��
			Hc595SendByte(Led_duan[i]);     //������ɨ��
			Delay_us(100);
			Hc595SendByte(0x00);            //�Ե�������
		}
}
											 
void Led_dz1(void) 	                   //led����������Ͻǵ�
{   
    Hc595SendByte(0x80);                      //ѡ����
    LED=0x7f;                                  //ѡ����
}


void Hc595SendByte(uchar8 dat)           //��ѡ��/��ѡ����       //HC595�ȴ��͸�λ���ٴ��͵�λ  ��OE��JOE��Ϊ�͵�ƽʱоƬ����
{
	uchar8 i;
	SRCLK=0;                                  //SHCP-��λ�Ĵ���ʱ�ӣ������������źţ�����
	RCLK=0;                                   //STCP-�洢�Ĵ���ʱ�ӣ������������źţ�����  
	for(i=0;i<8;i++)                            //ִ��8�κ�8λ���ݴ洢����λ���ݴ洢�Ĵ���  
	{
		SER=dat>>7;                             //a=0ʱ���������λ���磺dat=10101110 ,�������ߴκ󣬽�ʣ�����λ��1��
		dat<<=1;                                                         //��λ������λ
		SRCLK=1; 
		_nop_();                               //ִ��һ����ʱһ����������
		_nop_();                
		SRCLK=0;                                //����λ�Ĵ������ͣ���������´�������
	}                                        //ѭ��8�Σ���8λ����ȫ���洢���˴洢���ݼĴ�����
	  RCLK=1;                               //���洢���ݼĴ����е����ݷ��͸�HC595оƬ�����
		_nop_();
		_nop_(); 
	  RCLK=0;
}


void LED_Key()                               //Ӧ��4��4����ʹ����ܶ�����ʾ0~F
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

void Keydown(void)                           //4��4�������ѭ��ɨ�裬�����ϵ����£�����Ϊ����������е�0~F
{
//	static 	uchar8 a=0;                     //�ֲ���̬�������˴����ڳ����ڲ����ã�ֻ���ڳ����״�ִ��ʱ����ʼ��������ִ��һ�κ�ᱣ����ֵ����Ϊ�´γ���ִ�еĳ�ʼֵ
	uchar8 a=0;                               //����ֲ�������ֻ�ڸó����ڲ�����,ÿ�γ���ִ�п�ʼ���ᱻ��ʼ��
	GPIO_Key=0x0f;
	if(GPIO_Key!=0x0f)
	{
		Delay_ms(10);                           //��������                 
		if(GPIO_Key!=0x0f)                      //�ٴ�ȷ�ϰ���ȷʵ����
		{
			GPIO_Key=0x0f;                         //�����˿����¸�ֵ
			switch(GPIO_Key)                      //�м��      //ɨ������һ���а�������
			{
				case(0x07):Keyvalue=0;break;
				case(0x0b):Keyvalue=1;break;
				case(0x0d):Keyvalue=2;break;
				case(0x0e):Keyvalue=3;break;	
			}
			GPIO_Key=0xf0;                        //�м��     //ɨ������һ���а�������
			switch(GPIO_Key)
			{
				case(0x70):Keyvalue=Keyvalue;break;
				case(0xb0):Keyvalue=Keyvalue+4;break;
				case(0xd0):Keyvalue=Keyvalue+8;break;
				case(0xe0):Keyvalue=Keyvalue+12;break;	
			}
			while((a<50)&&(GPIO_Key!=0xf0))        //������ͬʱ��ⰴ����ʱ��񣬷�ֹ��������ռ��CPU
			{
				Delay_ms(10);
				a++;
			}
		}
	}
}	


void Key_Led(void)                          //��������LED������Ϩ��
{	
	   if(K1==0)  
		{	
			Delay_ms(10);	                              //��������
			if(K1==0)
			{	while(!K1);                            //�ȴ������ɿ���
				Led2=~Led2;
			}
		}
}
 void smgj(void)                               //��̬�������ʾ           //74H573��J21��оƬ��LEΪ����ˣ���LE��P10�̽ӣ���ʾ������״̬��I/O�ڿ��ƣ���LE��VCC�̽�ʱ����ʾ���ⲿ��Դ����
	                                                                                             //��LEΪ�ߵ�ƽʱ���������ݵĲɼ���D0~7��ֵ��Q0~7һһ��Ӧ����LEΪ�͵�ƽʱ��������֮ǰ�����ݣ����ֲ���
{
	LSA=0;
	LSB=0;
	LSC=0;
	LED=smgduan[0];                            //���������һ��Ԫ�أ�ʹ�������ʾ��
}



void smgd1(void)                             //��̬�����ѭ����ʾ0~F 
{
	uchar8 i;
	LSA=0;
	LSB=0;
	LSC=0;
	for(i=0;i<17;i++)
	{
	  LED=smgduan[i];
		Delay_s(1);
	}                                          //���������һ��Ԫ�أ�ʹ�������ʾ��
}



void smgd2(void)                             //8���������ˮѭ����ʾ0~7
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



void FMQ(void)                                  //����������
{
	FM=0;
   	Delay_us(10);	
  	FM=~FM;
		Delay_us(10);                              //����I/O���������ռ�ձ�
}



void LED_one(void)                              //����Led�ĵ�����Ϩ��
{
	  Led2=0;
		Delay_ms(100); 
		Led2=~Led2;
		Delay_ms(100); 
}



void LED_cror(void)                             //����ѭ����λ��������LED��ˮ��   ѭ������
{
	while(1)
	{
    uint16 i;
	  A=0xfe;
	  Delay_ms(50);
	  for(i=0;i<7;i++)
	    { 
	      A=_cror_(A,1);                         //ѭ������
		    Delay_ms(20);
	    }
			
	}
}



void LED_crol1(void)                            //��������ʵ��LED��ˮ��   ѭ������
{
	uchar8 table[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};   //����Ķ���
	while(1)
	{
		uint16 n;
		for(n=0;n<8;n++)
		{
			A=table[n];                              //���������е�n����
			Delay_ms(20);
		}
	}
}



void LED_crol2(void)                           //��������ˮ��
{
	uchar8 m=255;                
	while(1)
     {
       A=m;
			 Delay_ms(50);
			 m--;
     }
}


void Delay_s(uint16 n)                         //12M�����ӳ�n��
{
	uint16 i=0;
	uint32 j=0;
	for(i=0;i<n;i++)
	    for(j=0;j<21738;j++);
}



void Delay_ms(uint16 n)                         //12M�����ӳ�n����
{     
	uint16 i=0;
	uint32 j=0;
	for(i=0;i<n;i++)
	    for(j=0;j<123;j++);
}


  
void Delay_us(uint16 n)                         //12M�����ӳ�n/100����
{
	while(n--);
}	