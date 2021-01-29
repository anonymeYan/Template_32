#ifndef __modbus_h
#define __modbus_h
#include "sys.h"
#include "sys.h"
#include "24cxx.h"
#include "delay.h"

#define RS485_TX_EN  PAout(1)

extern u16 sys_data[40];

extern u32 RS485_Baudrate;                       //ͨѶ������
extern u8 RS485_Parity;                              //0��У�飻1��У�飻2żУ��
extern const u8 RS485_Addr;                                //�ӻ���ַ
extern u16 RS485_Frame_Distance;                     //����֡��С�����ms),������ʱ������Ϊ����һ֡
extern u8 RS485_RX_BUFF[512];                         //���ջ�����
extern u16 RS485_RX_CNT;                             //���ռ�����
extern u8 RS485_FrameFlag;                           //֡�������
extern u8 RS485_TX_BUFF[512];                         //���ͻ�����
extern u16 RS485_TX_CNT;                             //���ͼ�����
extern u16 *Modbus_HoldReg[512];                      //���ּĴ���ָ��


void Modbus_RegMap(void);
void RS485_Init(void);
void Timer7_Init(void);
void RS485_SendData(u8 *buff,u8 len);
void USART2_IRQHandler(void);
void TIM7_IRQHandler(void);
void RS485_Service(void);
void Modbus_02_Solve(void);
void Modbus_01_Solve(void);
void Modbus_05_Solve(void);
void Modbus_15_Solve(void);
void Modbus_03_Solve(void);
void Modbus_06_Solve(void);
void Modbus_16_Solve(void);

#endif
