#ifndef _TASK_H_
#define _TASK_H_

#include "stdint.h"


// 计算任务个数
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// 任务结构
typedef struct {
    uint8_t task_id;                    // 任务 ID
    uint16_t task_interval;             // 任务运行间隔时间
    void (*task_entry)(void);           // 要运行的任务
    volatile uint16_t task_tick_ms;     // 计时器
} task_info_t;

void timing_task_scheduler(void);

extern task_info_t taskLed;
extern task_info_t taskModbus;

#endif
