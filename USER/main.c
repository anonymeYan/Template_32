#include "main.h"

/***********************************
 *MCU��   STM32F103RCT6
 *EEPROM: FM24Cl04
 *IIC1:   FM24Cl04  MCP4726A0
 *IIC2:   MCP3421A0 MCP3421A1
 *USART2: ADM2483
 *TIM3:   50ms_timer
***********************************/

u16 SetI=0;

int main(void)
{
    delay_init();	    	 //��ʱ������ʼ��
    Run_LED.LED_Init();//����ָʾ�Ƴ�ʼ��
    MYIIC_IIC1.MYIIC_Init();
    MYIIC_IIC2.MYIIC_Init();
    RS485_Init();        //485ͨ�ų�ʼ��
    Start_Init();        //�źŵ��ʼ��  LED��  �̵�������
    Para_Init();         //��EEPROM�õ�������趨ֵ
    TIM3_Int_Init(499,7199);         //��ʱ��0.05s
    IWDG_Init(5,625);    //���ʱ��Ϊ2s
//    ADC_DMA_Config();    //�¶Ȳɼ�
//    TIM4_PWM_Init(3600-1,1-1);///20K      ���ת�ٿ���

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



