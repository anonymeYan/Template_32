#include "24cxx.h"

static void FM24Cl04_Init(void);
static uint8_t FM24Cl04_Check(void);
static uint8_t FM24Cl04_ReadOneByte(uint16_t ReadAddr);
static void FM24Cl04_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
static uint32_t FM24Cl04_ReadLenByte(uint16_t ReadAddr,uint8_t Len);
static void FM24Cl04_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
static void FM24Cl04_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
static void FM24Cl04_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);

AT24CXX_t FM24Cl04 =
{
    FM24Cl04_Init,
    FM24Cl04_Check,
    FM24Cl04_ReadOneByte,
    FM24Cl04_WriteOneByte,
    FM24Cl04_ReadLenByte,
    FM24Cl04_WriteLenByte,
    FM24Cl04_Read,
    FM24Cl04_Write
};


void FM24Cl04_Init(void)
{
    MYIIC_IIC1.MYIIC_Init();
}

//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(511)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 FM24Cl04_Check(void)
{
    u8 temp;
    temp=FM24Cl04_ReadOneByte(511);//����ÿ�ο�����дAT24CXX
    if(temp==0X55)return 0;
    else//�ų���һ�γ�ʼ�������
    {
        FM24Cl04_WriteOneByte(511,0X55);
        temp=FM24Cl04_ReadOneByte(511);
        if(temp==0X55)return 0;
    }
    return 1;
}


//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
u8 FM24Cl04_ReadOneByte(u16 ReadAddr)
{
    u8 temp=0;
    MYIIC_IIC1.MYIIC_Start();
    if(EE_TYPE>AT24C16)
    {
        MYIIC_IIC1.MYIIC_Send_Byte(0XA8);	   //����д����
        MYIIC_IIC1.MYIIC_Wait_Ack();
        MYIIC_IIC1.MYIIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
        MYIIC_IIC1.MYIIC_Wait_Ack();
    } else MYIIC_IIC1.MYIIC_Send_Byte(0XA8+((ReadAddr/256)<<1));   //����������ַ0xA8,д����

    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Start();
    MYIIC_IIC1.MYIIC_Send_Byte(0XA9);           //�������ģʽ
    MYIIC_IIC1.MYIIC_Wait_Ack();
    temp=MYIIC_IIC1.MYIIC_Read_Byte(0);
    MYIIC_IIC1.MYIIC_Stop();//����һ��ֹͣ����
    return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void FM24Cl04_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
    MYIIC_IIC1.MYIIC_Start();
    if(EE_TYPE>AT24C16)
    {
        MYIIC_IIC1.MYIIC_Send_Byte(0XA8);	    //����д����
        MYIIC_IIC1.MYIIC_Wait_Ack();
        MYIIC_IIC1.MYIIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
    } else
    {
        MYIIC_IIC1.MYIIC_Send_Byte(0xA8+((WriteAddr/256)<<1));   //����������ַ0xA8,д����
    }
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(DataToWrite);     //�����ֽ�
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Stop();;//����һ��ֹͣ����
    delay_ms(10);
}


//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 FM24Cl04_ReadLenByte(u16 ReadAddr,u8 Len)
{
    u8 t;
    u32 temp=0;
    for(t=0; t<Len; t++)
    {
        temp<<=8;
        temp+=FM24Cl04_ReadOneByte(ReadAddr+Len-t-1);
    }
    return temp;
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void FM24Cl04_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{
    u8 t;
    for(t=0; t<Len; t++)
    {
        FM24Cl04_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }
}


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void FM24Cl04_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
    while(NumToRead)
    {
        *pBuffer++=FM24Cl04_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void FM24Cl04_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
    while(NumToWrite--)
    {
        FM24Cl04_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}







