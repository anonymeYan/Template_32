#ifndef _TASK_MODBUS_H_
#define _TASK_MODBUS_H_

#include "stdint.h"


// �����������
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// ����ṹ
typedef struct {
    uint8_t task_id;                    // ���� ID
    uint16_t task_interval;             // �������м��ʱ��
    void (*task_entry)(void);           // Ҫ���е�����
    volatile uint16_t task_tick_ms;     // ��ʱ��
} task_info_t;

void timing_task_scheduler(void);


#endif