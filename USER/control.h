#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


extern float current_soc;

void fastRun(void);
void run(void);
void Para_Init(void);
int trans_data(u16 data);

void run18(void);
int32_t trans_16(uint16_t data);
uint32_t trans_18(uint32_t data);

#endif

