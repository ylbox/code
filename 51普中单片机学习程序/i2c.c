#include<i2c.h>

void Delay10us(void)
{
	unsigned char a,b;
	for(b=1;b>0;b--)
	    for(a=2;a>0;a--);
}


void I2cStart()                             //软件——模拟IIC通信典型起始信号
{
	SDA=1;                                  //必须先将SDA（数据信号）信号拉高
	Delay10us();
	SCL=1;                                  //SCL（时钟信号）持续保证高电平——且必须先将SDA信号拉高后再执行拉高SCL信号
	Delay10us();
	SDA=0;                                  //拉低SDA产生下降沿信号——实现起始信号的软件模拟
	Delay10us();
	SCL=0;                                  //将SCL信号再次拉低防止对后续信号产生干扰——因当SCL=0时，允许SDA的状态可自由变换，此时不会对数据SDA信号产生影响
	Delay10us();
}

void I2cStop()                             //软件——模拟IIC通信典型终止信号——备注参考：IIC通信典型起始信号
{
	SDA=0;
	Delay10us();
	SCL=1;
	Delay10us();
	SDA=1;
	Delay10us();
	SCL=0;                                  //将SCL信号再次拉低防止对后续信号产生干扰——因当SCL=0时，允许SDA的状态可自由变换，此时不会对数据SDA信号产生影响
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




