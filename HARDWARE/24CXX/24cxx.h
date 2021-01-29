#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"
#include "delay.h"


#define AT24C01		127
#define AT24C02		255
#define AT24Cl04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767

#define EE_TYPE AT24Cl04


typedef struct
{
    void (*AT24CXX_Init)(void);//��ʼ��IIC
    uint8_t (*AT24CXX_Check)(void);//�������
    uint8_t (*AT24CXX_ReadOneByte)(uint16_t ReadAddr);//ָ����ַ��ȡһ���ֽ�
    void (*AT24CXX_WriteOneByte)(uint16_t WriteAddr,uint8_t DataToWrite);//ָ����ַд��һ���ֽ�
    uint32_t (*AT24CXX_ReadLenByte)(uint16_t ReadAddr,uint8_t Len);
    void (*AT24CXX_WriteLenByte)(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
    void (*AT24CXX_Read)(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);//��ָ����ַ��ʼ����ָ�����ȵ�����
    void (*AT24CXX_Write)(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);//��ָ����ַ��ʼд��ָ�����ȵ�����
} AT24CXX_t;

extern AT24CXX_t FM24Cl04;






#endif
















