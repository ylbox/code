#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;

void main()
{
   TMOD=0X20;	//0010 0000 ������ʽ2��8λ�Զ���װ�ض�ʱ��
   TH1=0XF3;	//����������ֵΪ0xf3����ʱSMODΪ1��������Ϊ4800�����Ϊ0.16%���Ͼ�ȷ
   TL1=0XF3;

   PCON=0X80;	 //SMODΪ1
   SCON=0X50;  	//0101 0000 ������ʽ1��10λ�첽�շ�����REN=1				
   EA=1;
   ES=1;
   TR1=1;		//û��ET1=1������䣬ԭ������T1���൱����һ�������ʷ�����������ҪT1�ж�
   while(1);
}

void chuanhangkou() interrupt 4
{
   uchar receieve_Date;
   receieve_Date=SBUF;		//�˴���SBUF�ǽ��ջ�������������һ֡���ݺ�Ӳ����RIΪ1�������������
   RI=0;

   SBUF=receieve_Date;		  //�˴���SBUF�Ƿ��ͻ�����
   while(!TI);
   TI=0;
} 