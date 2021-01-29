#ifndef __modbus_h
#define __modbus_h
#include "sys.h"
#include "sys.h"
#include "24cxx.h"
#include "delay.h"

#define RS485_TX_EN  PAout(1)

extern u16 sys_data[40];

extern u32 RS485_Baudrate;                       //通讯波特率
extern u8 RS485_Parity;                              //0无校验；1奇校验；2偶校验
extern const u8 RS485_Addr;                                //从机地址
extern u16 RS485_Frame_Distance;                     //数据帧最小间隔（ms),超过此时间则认为是下一帧
extern u8 RS485_RX_BUFF[512];                         //接收缓冲区
extern u16 RS485_RX_CNT;                             //接收计数器
extern u8 RS485_FrameFlag;                           //帧结束标记
extern u8 RS485_TX_BUFF[512];                         //发送缓冲区
extern u16 RS485_TX_CNT;                             //发送计数器
extern u16 *Modbus_HoldReg[512];                      //保持寄存器指针


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
