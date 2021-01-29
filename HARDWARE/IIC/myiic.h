#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"
#include "delay.h"


//IO��������
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

#define SDA_IN1()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_OUT1() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}
#define IIC_SCL1    PBout(10) //SCL
#define IIC_SDA1    PBout(11) //SDA	 
#define READ_SDA1   PBin(11)  //����SDA 

typedef struct
{
	void (*MYIIC_Init)(void); //��ʼ��IIC��IO��
	void (*MYIIC_Start)(void);//����IIC��ʼ�ź�
	void (*MYIIC_Stop)(void); //����IICֹͣ�ź�
	void (*MYIIC_Ack)(void);  //IIC����ACK�ź�
	void (*MYIIC_NAck)(void); //IIC������ACK�ź�
	void (*MYIIC_Send_Byte)(uint8_t txd);//IIC����һ���ֽ�
	uint8_t (*MYIIC_Read_Byte)(uint8_t ack); 
	uint8_t (*MYIIC_Wait_Ack)(void);
}MYIIC_t;

extern MYIIC_t  MYIIC_IIC1;
extern MYIIC_t  MYIIC_IIC2;

#endif
















