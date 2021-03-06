#ifndef __MCP4726_H
#define __MCP4726_H
#include "sys.h"
#include "delay.h"
#include "sys.h"
#include "myiic.h"

typedef struct
{
    void (*MCP4726_Init)(void);
    void (*MCP4726_SetDAC)(uint16_t Vout);
} MCP4726_t;


extern MCP4726_t MCP4726_A0;

#endif

