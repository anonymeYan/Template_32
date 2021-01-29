#include "main.h"

/***********************************
 *MCU：   STM32F103RCT6
 *EEPROM: FM24Cl04
 *IIC1:   FM24Cl04  MCP4726A0
 *IIC2:   MCP3421A0 MCP3421A1
 *USART2: ADM2483
 *TIM3:   50ms_timer
***********************************/

u16 SetI=0;

int main(void)
{
    delay_init();	    	 //延时函数初始化
    Run_LED.LED_Init();//运行指示灯初始化
    MYIIC_IIC1.MYIIC_Init();
    MYIIC_IIC2.MYIIC_Init();
    RS485_Init();        //485通信初始化
    Start_Init();        //信号点初始化  LED灯  继电器控制
    Para_Init();         //从EEPROM得到保存的设定值
    TIM3_Int_Init(499,7199);         //定时器0.05s
    IWDG_Init(5,625);    //溢出时间为2s
//    ADC_DMA_Config();    //温度采集
//    TIM4_PWM_Init(3600-1,1-1);///20K      风机转速控制

    while(1)
    {
        RS485_Service();
        if(time1>=10)
        {
            time1=0;
            IWDG_Feed();
            Run_LED.LED_Flip();
            run();
//            run18();
        }
    }

}



