#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//ADC 驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////


extern u16  ADC_ConvertedX[10][6];
extern u16  T1R,T2R,T3R,T4R,T5R,T6R;

void GetTemp(void);
void ADC_DMA_Config(void);
void  Adc_Init_single(void);
void Adc_Init(void);
void T_Adc_Init(void);
short Get_Temprate(void);
u16  Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);

#endif
