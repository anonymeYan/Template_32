#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


typedef struct
{
	void (*TIM_Init)(void);

}TIM_t;

extern u8 time1,timeFast;

void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);

#endif
