#include<i2c.h>

void Delay10us(void)
{
	unsigned char a,b;
	for(b=1;b>0;b--)
	    for(a=2;a>0;a--);
}


void I2cStart()                             //�������ģ��IICͨ�ŵ�����ʼ�ź�
{
	SDA=1;                                  //�����Ƚ�SDA�������źţ��ź�����
	Delay10us();
	SCL=1;                                  //SCL��ʱ���źţ�������֤�ߵ�ƽ�����ұ����Ƚ�SDA�ź����ߺ���ִ������SCL�ź�
	Delay10us();
	SDA=0;                                  //����SDA�����½����źš���ʵ����ʼ�źŵ����ģ��
	Delay10us();
	SCL=0;                                  //��SCL�ź��ٴ����ͷ�ֹ�Ժ����źŲ������š�����SCL=0ʱ������SDA��״̬�����ɱ任����ʱ���������SDA�źŲ���Ӱ��
	Delay10us();
}

void I2cStop()                             //�������ģ��IICͨ�ŵ�����ֹ�źš�����ע�ο���IICͨ�ŵ�����ʼ�ź�
{
	SDA=0;
	Delay10us();
	SCL=1;
	Delay10us();
	SDA=1;
	Delay10us();
	SCL=0;                                  //��SCL�ź��ٴ����ͷ�ֹ�Ժ����źŲ������š�����SCL=0ʱ������SDA��״̬�����ɱ任����ʱ���������SDA�źŲ���Ӱ��
	Delay10us();
}

unsigned char I2cSendByte(unsigned char dat)
{
	unsigned char a=0,b;
	for(a=0;a<8;a++)
	{
		SDA=dat>>7;
		dat=dat<<1;
		Delay10us();
		SCL=1;
		Delay10us();
		SCL=0;
		Delay10us();
	}
	SDA=1;
	Delay10us();
	SCL=1;
	while(SDA)
	{
		b++;
		if(b>200)
		{
			SCL=0;
			Delay10us();
			return 0;
		}
	}
	SCL=0;
	Delay10us();
	return 1;
}


unsigned char I2cReadByte()
{
	unsigned char a=0,dat=0;
	for(a=0;a<8;a++)
	{
		SCL=1;
		Delay10us();
		dat<<=1;
		dat|=SDA;
		Delay10us();
		SCL=0;
		Delay10us();
	}
	return dat;
}


void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa0);
	I2cSendByte(addr);
	I2cSendByte(dat);
	I2cStop();
}

unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2cStart();
	I2cSendByte(0xa0);
	I2cSendByte(addr);
	I2cStart();
	I2cSendByte(0xa1);
    num=I2cReadByte();
  	I2cStop();
	return num;
}




