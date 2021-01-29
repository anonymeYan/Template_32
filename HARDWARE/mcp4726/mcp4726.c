#include "mcp4726.h"


/*          ����λ  VREF1 VREF0  PD1  PD0   G
        1.����λ //MCP47X6 ֧�ֵ�����
          C2  C1  C0
          0   0   x  д��ʧ��DAC �Ĵ�������
          0   1   0  д��ʧ�Դ洢������
          0   1   1  д���д洢������
          1   0   0  д��ʧ������λ����

        2.VREF1 VREF0  ���ε��������ѹ��׼��VRL��ѡ��λ
            0     x     VDD       �޻���
            1     0     VREF����  �޻���
            1     1            VREF����  ����

				3.PD1  PD0
					 0    0    ��������
					 0    1    1K������ӵ�
					 1    0    125K������ӵ�
					 1    1    640K������ӵ�

        4.G         ����ѡ��λ
          0 =1x ������Ϊ1��
          1 =2x ������Ϊ2��   VDD��VRL������

//   C2  C1  C0  VREF1 VREF0  PD1  PD0   G
//    1  0    0    1     1     0    0    0    0x98  д��ʧ������λ����*/

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
    MYIIC_IIC1.MYIIC_Send_Byte(0xc0) ;////����Ѱַ����������1100�� ��ַλA2��A1��A0Ϊ 0 �� 0 �� 0��-> 1100 0000
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(0x98) ;//����
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Stop();

    delay_ms(1);

    Dn = ( 4096 * Vout) / 2500;
    temp = (0x0F00 & Dn) >> 8;  //12λ����

    MYIIC_IIC1.MYIIC_Start();
    MYIIC_IIC1.MYIIC_Send_Byte(0xc0);      //����Ѱַ����������1100�� ��ַλA2��A1��A0Ϊ 0 �� 0 �� 0��-> 1100 0000
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(temp);    //����
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Send_Byte(Dn);        //����8λ�͵�DAC�Ĵ���
    MYIIC_IIC1.MYIIC_Wait_Ack();
    MYIIC_IIC1.MYIIC_Stop();;//����һ��ֹͣ����
}




