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
    void (*AT24CXX_Init)(void);//初始化IIC
    uint8_t (*AT24CXX_Check)(void);//检查器件
    uint8_t (*AT24CXX_ReadOneByte)(uint16_t ReadAddr);//指定地址读取一个字节
    void (*AT24CXX_WriteOneByte)(uint16_t WriteAddr,uint8_t DataToWrite);//指定地址写入一个字节
    uint32_t (*AT24CXX_ReadLenByte)(uint16_t ReadAddr,uint8_t Len);
    void (*AT24CXX_WriteLenByte)(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
    void (*AT24CXX_Read)(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);//从指定地址开始读出指定长度的数据
    void (*AT24CXX_Write)(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);//从指定地址开始写入指定长度的数据
} AT24CXX_t;

extern AT24CXX_t FM24Cl04;






#endif
















