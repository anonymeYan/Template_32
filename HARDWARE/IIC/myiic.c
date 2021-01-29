#include "myiic.h"

static void IIC1_Init(void); 
static	void IIC1_Start(void);
static	void IIC1_Stop(void); 
static	void IIC1_Ack(void); 
static	void IIC1_NAck(void);
static	void IIC1_Send_Byte(uint8_t txd);
static	uint8_t IIC1_Read_Byte(uint8_t ack); 
static	uint8_t IIC1_Wait_Ack(void);

MYIIC_t  MYIIC_IIC1 =
{
	IIC1_Init,
	IIC1_Start,
  IIC1_Stop,
	IIC1_Ack,
	IIC1_NAck,
	IIC1_Send_Byte,
	IIC1_Read_Byte,
	IIC1_Wait_Ack
};


static  void IIC2_Init(void); 
static	void IIC2_Start(void);
static	void IIC2_Stop(void); 
static	void IIC2_Ack(void); 
static	void IIC2_NAck(void);
static	void IIC2_Send_Byte(uint8_t txd);
static	uint8_t IIC2_Read_Byte(uint8_t ack); 
static	uint8_t IIC2_Wait_Ack(void);

MYIIC_t  MYIIC_IIC2 =
{
	IIC2_Init,
	IIC2_Start,
  IIC2_Stop,
	IIC2_Ack,
	IIC2_NAck,
	IIC2_Send_Byte,
	IIC2_Read_Byte,
	IIC2_Wait_Ack
};

//��ʼ��IIC
static void IIC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	//PB6,PB7 �����
}
//����IIC��ʼ�ź�
static void IIC1_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA=1;
    IIC_SCL=1;
    delay_us(4);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
static void IIC1_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL=1;
    IIC_SDA=1;//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
static u8 IIC1_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA����Ϊ����
    IIC_SDA=1;
    delay_us(1);
    IIC_SCL=1;
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC1_Stop();
            return 1;
        }
    }
    IIC_SCL=0;//ʱ�����0
    return 0;
}
//����ACKӦ��
static void IIC1_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//������ACKӦ��
static void IIC1_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
static void IIC1_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            IIC_SDA=1;
        else
            IIC_SDA=0;
        txd<<=1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL=1;
        delay_us(2);
        IIC_SCL=0;
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
static u8 IIC1_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA����Ϊ����
    for(i=0; i<8; i++ )
    {
        IIC_SCL=0;
        delay_us(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC1_NAck();//����nACK
    else
        IIC1_Ack(); //����ACK
    return receive;
}




void IIC2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��GPIOBʱ��

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB10,PB11 �����
}



void IIC2_Start(void)
{
    SDA_OUT1();     //sda�����
    IIC_SDA1=1;
    IIC_SCL1=1;
    delay_us(4);
    IIC_SDA1=0;//START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL1=0;//ǯסI2C���ߣ�׼�����ͻ��������
}


//����IICֹͣ�ź�
void IIC2_Stop(void)
{
    SDA_OUT1();//sda�����
    IIC_SCL1=0;
    IIC_SDA1=0;//STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL1=1;
    IIC_SDA1=1;//����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC2_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN1();      //SDA����Ϊ����
    IIC_SDA1=1;
    delay_us(1);
    IIC_SCL1=1;
    delay_us(1);
    while(READ_SDA1)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC2_Stop();
            return 1;
        }
    }
    IIC_SCL1=0;//ʱ�����0
    return 0;
}
//����ACKӦ��
void IIC2_Ack(void)
{
    IIC_SCL1=0;
    SDA_OUT1();
    IIC_SDA1=0;
    delay_us(2);
    IIC_SCL1=1;
    delay_us(2);
    IIC_SCL1=0;
}
//������ACKӦ��
void IIC2_NAck(void)
{
    IIC_SCL1=0;
    SDA_OUT1();
    IIC_SDA1=1;
    delay_us(2);
    IIC_SCL1=1;
    delay_us(2);
    IIC_SCL1=0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC2_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT1();
    IIC_SCL1=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0; t<8; t++)
    {
        //IIC_SDA=(txd&0x80)>>7;
        if((txd&0x80)>>7)
            IIC_SDA1=1;
        else
            IIC_SDA1=0;
        txd<<=1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL1=1;
        delay_us(2);
        IIC_SCL1=0;
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC2_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN1();//SDA����Ϊ����
    for(i=0; i<8; i++ )
    {
        IIC_SCL1=0;
        delay_us(2);
        IIC_SCL1=1;
        receive<<=1;
        if(READ_SDA1)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC2_NAck();//����nACK
    else
        IIC2_Ack(); //����ACK
    return receive;
}

