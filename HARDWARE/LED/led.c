#include "led.h"

static void Run_LED_Init(void);
static void Run_LED_ON(void);
static void Run_LED_OFF(void);
static void Run_LED_Flip(void);

LED_t Run_LED =
{
    Run_LED_Init,
    Run_LED_ON,
    Run_LED_OFF,
    Run_LED_Flip
};

static void Run_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO
    GPIO_SetBits(GPIOC,GPIO_Pin_10);						 // �����led��
}

static void Run_LED_ON(void){MCU_Run_LED = 1;}
static void Run_LED_OFF(void){MCU_Run_LED = 0;}
static void Run_LED_Flip(void){MCU_Run_LED = ~MCU_Run_LED;}


void Start_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_10;				 //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;		//������������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOC,GPIO_Pin_0);		           //PC0  ����
    GPIO_SetBits(GPIOC,GPIO_Pin_10);						 //PC10 �����led��
    GPIO_ResetBits(GPIOC,GPIO_Pin_7); 						 //FAN  KM3


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	    		 //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz

    GPIO_ResetBits(GPIOB,GPIO_Pin_0); 						 //CHR   KM1
    GPIO_ResetBits(GPIOB,GPIO_Pin_1);             //DCHR   KM2
}








