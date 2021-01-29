#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//ADC ��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
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
