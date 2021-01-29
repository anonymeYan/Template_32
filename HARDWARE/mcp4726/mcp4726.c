#include "mcp4726.h"


/*          命令位  VREF1 VREF0  PD1  PD0   G
        1.命令位 //MCP47X6 支持的命令
          C2  C1  C0
          0   0   x  写易失性DAC 寄存器命令
          0   1   0  写易失性存储器命令
          0   1   1  写所有存储器命令
          1   0   0  写易失性配置位命令

        2.VREF1 VREF0  梯形电阻网络电压基准（VRL）选择位
            0     x     VDD       无缓冲
            1     0     VREF引脚  无缓冲
            1     1            VREF引脚  缓冲

				3.PD1  PD0
					 0    0    正常工作
					 0    1    1KΩ电阻接地
					 1    0    125KΩ电阻接地
					 1    1    640KΩ电阻接地

        4.G         增益选择位
          0 =1x （增益为1）
          1 =2x （增益为2）   VDD作VRL不适用

//   C2  C1  C0  VREF1 VREF0  PD1  PD0   G
//    1  0    0    1     1     0    0    0    0x98  写易失性配置位命令*/

static void MCP4726_A0_Init(void);
static void MCP4726_A0_SetDAC(uint16_t Vout);



MCP4726_t MCP4726_A0 =
{
    MCP4726_A0_Init,
    MCP4726_A0_SetDAC
};

static void MCP4726_A0_Init(void)
{
    MYIIC_IIC1.MYIIC_Init();
}

static void MCP4726_A0_SetDAC(u16 Vout)   
{
    u8 temp;
    u16 Dn;

    MYIIC_IIC1.MYIIC_Start();
    MYIIC_IIC1.MYIIC_Send_Byte(0xc0) ;////器件寻址，器件代吗：1100； 地址位A2，A1，A0为 0 ， 0 ， 0；-> 1100 0000
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(0x98) ;//命令
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Stop();

    delay_ms(1);

    Dn = ( 4096 * Vout) / 2500;
    temp = (0x0F00 & Dn) >> 8;  //12位数据

    MYIIC_IIC1.MYIIC_Start();
    MYIIC_IIC1.MYIIC_Send_Byte(0xc0);      //器件寻址，器件代吗：1100； 地址位A2，A1，A0为 0 ， 0 ， 0；-> 1100 0000
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(temp);    //命令
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(Dn);        //将低8位送到DAC寄存器
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Stop();;//产生一个停止条件
}




