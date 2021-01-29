#ifndef __MCP3421_H
#define __MCP3421_H
#include "delay.h"
#include "sys.h"
#include "myiic.h"

typedef struct
{
    void (*MCP3421_Init)(void);
    u16 (*MCP3421_READ)(void);
    u32 (*MCP3421_READ_18Bits)(void);
} MCP3421_t;


extern MCP3421_t MCP3421_A0;
extern MCP3421_t MCP3421_A1;

#endif


