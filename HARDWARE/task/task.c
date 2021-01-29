#include "task.h"


void task_led();

task_info_t taskLed = { 0,100,task_led,0};



void task_led()
{

}

task_info_t timing_task_array[3]=
{
    taskLed,
    taskLed,
    taskLed
};

// 任务计划表，轮询执行任务
void timing_task_scheduler(void)
{
    uint8_t task_index = 0;

    while (1)
    {
        for (task_index = 0 ; task_index < ARRAY_SIZE(timing_task_array); task_index++)
        {
            if (timing_task_array[task_index].task_tick_ms >= timing_task_array[task_index].task_interval)
            {
                timing_task_array[task_index].task_tick_ms = 0;
                timing_task_array[task_index].task_entry();
            }
        }
    }
}


