#ifndef __LED_H
#define __LED_H
#include "sys.h"


#define MCU_Run_LED  PCout(10)      // ����ָʾ��

#define KM1 PBout(0)
#define KM2  PBout(1)
#define KM3  PCout(7)
#define TSafe PCin(0)  //����


void Start_Init(void);//��ʼ��

typedef struct
{
    void (*LED_Init)(void);
    void (*LED_ON)(void);
    void (*LED_OFF)(void);
    void (*LED_Flip)(void);
} LED_t;

extern LED_t Run_LED;

#endif