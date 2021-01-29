#include "mcp3421.h"


static void MCP3421_A0_Init(void);
static u16 MCP3421_A0_READ_16Bits(void);
static u32 MCP3421_A0_READ_18Bits(void);

MCP3421_t MCP3421_A0 =
{
    MCP3421_A0_Init,
    MCP3421_A0_READ_16Bits,
    MCP3421_A0_READ_18Bits
};

static void MCP3421_A1_Init(void);
static u16 MCP3421_A1_READ_16Bits(void);
static u32 MCP3421_A1_READ_18Bits(void);

MCP3421_t MCP3421_A1 =
{
    MCP3421_A1_Init,
    MCP3421_A1_READ_16Bits,
    MCP3421_A1_READ_18Bits
};

static void MCP3421_A0_Init(void)
{
    MYIIC_IIC2.MYIIC_Init();
}

static u16 MCP3421_A0_READ_16Bits(void)
{
    u16 buff=0;
    u8 cofig=0;
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd0) ;//写地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Send_Byte(0x88) ;//数据
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Stop();
    delay_ms(1);
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd1) ;//读地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1)<<8;
    buff+= MYIIC_IIC2.MYIIC_Read_Byte(1);//读出的转换数据存入16位变量中
    cofig=MYIIC_IIC2.MYIIC_Read_Byte(0);
    MYIIC_IIC2.MYIIC_Wait_Ack();
    MYIIC_IIC2.MYIIC_Stop();
    return buff;
}



static u32 MCP3421_A0_READ_18Bits(void)
{
    u32 data=0,buff=0,config=0;
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd0) ;//写地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Send_Byte(0x8c) ;//数据
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Stop();
    delay_ms(1);
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd1) ;//读地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    config=MYIIC_IIC2.MYIIC_Read_Byte(0);
    MYIIC_IIC2.MYIIC_Wait_Ack();
    MYIIC_IIC2.MYIIC_Stop();
    data&=0x03ffff;
    return data;
}


static void MCP3421_A1_Init(void)
{
    MYIIC_IIC2.MYIIC_Init();
}

static u16 MCP3421_A1_READ_16Bits(void)
{
    u16 buff=0;
    u8 cofig=0;
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd2) ;//写地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Send_Byte(0x88) ;//数据
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Stop();
    delay_ms(1);
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd3) ;//读地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1)<<8;
    buff+= MYIIC_IIC2.MYIIC_Read_Byte(1);//读出的转换数据存入16位变量中
    cofig=MYIIC_IIC2.MYIIC_Read_Byte(0);
    MYIIC_IIC2.MYIIC_Wait_Ack();
    MYIIC_IIC2.MYIIC_Stop();
    return buff;
}



static u32 MCP3421_A1_READ_18Bits(void)
{
    u32 data=0,buff=0,config=0;
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd2) ;//写地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Send_Byte(0x8c) ;//数据
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    MYIIC_IIC2.MYIIC_Stop();
    delay_ms(1);
    MYIIC_IIC2.MYIIC_Start();
    MYIIC_IIC2.MYIIC_Send_Byte(0xd3) ;//读地址
    MYIIC_IIC2.MYIIC_Wait_Ack() ;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    buff = MYIIC_IIC2.MYIIC_Read_Byte(1);
    data=(data<<8)|buff;
    config=MYIIC_IIC2.MYIIC_Read_Byte(0);
    MYIIC_IIC2.MYIIC_Wait_Ack();
    MYIIC_IIC2.MYIIC_Stop();
    data&=0x03ffff;
    return data;
}














