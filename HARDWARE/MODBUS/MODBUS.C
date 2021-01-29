#include "modbus.h"
#include "crc16.h"

u32 RS485_Baudrate=9600;                        //ͨѶ������
u8 RS485_Parity=0;                              //0��У�飻1��У�飻2żУ��
const u8 RS485_Addr=1;                          //�ӻ���ַ
u16 RS485_Frame_Distance=4;                     //����֡��С�����ms),������ʱ������Ϊ����һ֡
u8 RS485_RX_BUFF[512]= {0};                     //���ջ�����
u16 RS485_RX_CNT=0;                             //���ռ�����
u8 RS485_FrameFlag=0;                           //֡�������
u8 RS485_TX_BUFF[512]= {0};                     //���ͻ�����
u16 RS485_TX_CNT=0;                             //���ͼ�����
u16 *Modbus_HoldReg[512]= {0};                  //���ּĴ���ָ��

u16 sys_data[40]= {0x0};

//�Ĵ�����ַӳ��
void Modbus_RegMap(void)
{
    Modbus_HoldReg[0]=(u16*) &sys_data[0];//��ѹ-16λ
    Modbus_HoldReg[1]=(u16*) &sys_data[1];//����-16λ
    Modbus_HoldReg[2]=(u16*) &sys_data[2];//��ѹ-18λ
    Modbus_HoldReg[3]=(u16*) &sys_data[3];//����-18λ	
    Modbus_HoldReg[4]=(u16*) &sys_data[4];//��ѹ
    Modbus_HoldReg[5]=(u16*) &sys_data[5];//����	

    Modbus_HoldReg[10]=(u16*) &sys_data[10];//DAC-mv
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��USART2
void RS485_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOA,Dʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //PA4�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);//Ĭ�Ͻ���״̬

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//PA2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//��λ����2
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//ֹͣ��λ

    USART_InitStructure.USART_BaudRate=RS485_Baudrate;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_WordLength=USART_WordLength_8b;
    USART_InitStructure.USART_StopBits=USART_StopBits_1;
    USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
    switch(RS485_Parity)
    {
    case 0:
        USART_InitStructure.USART_Parity=USART_Parity_No;
        break;//��У��
    case 1:
        USART_InitStructure.USART_Parity=USART_Parity_Odd;
        break;//��У��
    case 2:
        USART_InitStructure.USART_Parity=USART_Parity_Even;
        break;//żУ��
    }
    USART_Init(USART2,&USART_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܴ���2�����ж�

    USART_Cmd(USART2,ENABLE);       //ʹ�ܴ���2

    RS485_TX_EN=0;                  //Ĭ��Ϊ����ģʽ
    Timer7_Init();                  //��ʱ��7��ʼ�������ڼ��ӿ���ʱ��
    Modbus_RegMap();                //Modbus�Ĵ���ӳ��

}

//��ʱ��7��ʼ��
void Timer7_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //TIM7ʱ��ʹ��

    //TIM7��ʼ������
    TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Distance*10; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
    TIM_ITConfig( TIM7, TIM_IT_Update, ENABLE );//TIM7 ���������ж�
//    TIM_Cmd(TIM7, ENABLE);
    //TIM7�жϷ�������
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);             //����NVIC�ж����ȼ�����
    NVIC_InitStructure.NVIC_IRQChannel =TIM7_IRQn;              //TIM7�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //�����ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                             //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//////////////////////////////////////////////////////////////////////////////
//����n���ֽ�����
//buff:�������׵�ַ
//len�����͵��ֽ���
void RS485_SendData(u8 *buf,u8 len)
{
    u8 t;
    RS485_TX_EN=1;			//����Ϊ����ģʽ
    delay_ms(5);
    for(t=0; t<len; t++)		//ѭ����������
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        delay_us(5);
        USART_SendData(USART2,buf[t]);
    }

    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    //RS485_RX_CNT=0;
    RS485_TX_EN=0;				//����Ϊ����ģʽ
}

/////////////////////////////////////////////////////////////////////////////////////
void USART2_IRQHandler(void)//����2�жϷ������
{
    u8 res;
    u8 err;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
    {
        if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE))
            err=1;                                      //��⵽������֡�����У�����
        else
            err=0;
        res=USART_ReceiveData(USART2);                  //�����յ����ֽڣ�ͬʱ��ر�־�Զ����

        if((RS485_RX_CNT<2047)&&(err==0))
        {
            RS485_RX_BUFF[RS485_RX_CNT]=res;
            RS485_RX_CNT++;

            TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //�����ʱ������ж�
            TIM_SetCounter(TIM7,0);                     //�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
            TIM_Cmd(TIM7,ENABLE);                       //��ʼ��ʱ
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////
//�ö�ʱ��7�жϽ��տ���ʱ�䣬������ʱ�����ָ��ʱ�䣬��Ϊһ֡����
//��ʱ��7�жϷ������
void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM7,TIM_IT_Update)!=RESET)
    {
        TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //����жϱ�־
        TIM_Cmd(TIM7,DISABLE);                      //ֹͣ��ʱ��
        RS485_TX_EN=1;                              //ֹͣ���գ��л�Ϊ����״̬
        RS485_FrameFlag=1;                          //��λ֡�������
    }
}

/////////////////////////////////////////////////////////////////////////////////////
//RS485����������ڴ������յ�������(������������ѭ������)
u16 startRegAddr;
u16 RegNum;
u16 calCRC;
void RS485_Service(void)
{
    u16 recCRC;
    if(RS485_FrameFlag==1)
    {
        if(RS485_RX_BUFF[0]==RS485_Addr)//��ַ��ȷ
        {
            if((RS485_RX_BUFF[1]==03)||(RS485_RX_BUFF[1]==06))//��������ȷ
            {
                startRegAddr=(((u16)RS485_RX_BUFF[2])<<8)|RS485_RX_BUFF[3];//��ȡ�Ĵ�����ʼ��ַ
                if(startRegAddr<1000)//�Ĵ�����ַ�ڷ�Χ��
                {
                    calCRC=CRC_Compute(RS485_RX_BUFF,RS485_RX_CNT-2);//�������������ݵ�CRC
                    recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((u16)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//���յ���CRC(���ֽ���ǰ�����ֽ��ں�)
                    if(calCRC==recCRC)//CRCУ����ȷ
                    {
                        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        switch(RS485_RX_BUFF[1])//���ݲ�ͬ�Ĺ�������д���
                        {
                        case 03: //������Ĵ���
                        {
                            Modbus_03_Solve();
                            break;
                        }

                        case 06: //д�����Ĵ���
                        {
                            Modbus_06_Solve();
                            break;
                        }
                        default:
                            break;
                        }
                        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    }
                    else//CRCУ�����
                    {
                        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                        RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                        RS485_TX_BUFF[2]=0x04; //�쳣��
                        RS485_SendData(RS485_TX_BUFF,3);
                    }
                }
                else//�Ĵ�����ַ������Χ
                {
                    RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                    RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                    RS485_TX_BUFF[2]=0x02; //�쳣��
                    RS485_SendData(RS485_TX_BUFF,3);
                }
            }
            else//���������
            {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x01; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
            }
        }
        RS485_FrameFlag=0;//��λ֡������־
        RS485_RX_CNT=0;//���ռ���������
        RS485_TX_EN=0;//��������ģʽ
    }
}



//Modbus������03��������
//�����ּĴ���
void Modbus_03_Solve(void)
{
    u8 i;
    RegNum = (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
    if((startRegAddr+RegNum)<1000)//�Ĵ�����ַ+�����ڷ�Χ��
    {
        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
        RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
        RS485_TX_BUFF[2]=RegNum*2;
        for(i=0; i<RegNum; i++)
        {
            RS485_TX_BUFF[4+i*2]=*Modbus_HoldReg[startRegAddr+i]&0xFF;//���͵��ֽ�
            RS485_TX_BUFF[3+i*2]=(*Modbus_HoldReg[startRegAddr+i]>>8)&0xFF;//�ȷ��͸��ֽ�
        }
        calCRC=CRC_Compute(RS485_TX_BUFF,RegNum*2+3);
        RS485_TX_BUFF[RegNum*2+3]=(calCRC>>8)&0xFF;
        RS485_TX_BUFF[RegNum*2+4]=calCRC&0xFF;
        RS485_SendData(RS485_TX_BUFF,RegNum*2+5);

    }
    else//�Ĵ�����ַ+����������Χ
    {
        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
        RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
        RS485_TX_BUFF[2]=0x02; //�쳣��
        RS485_SendData(RS485_TX_BUFF,3);
    }
}


//Modbus������06��������
//д�������ּĴ���
void Modbus_06_Solve(void)
{
    *Modbus_HoldReg[startRegAddr] = ((u16)RS485_RX_BUFF[4])<<8;//���ֽ��ں�
    *Modbus_HoldReg[startRegAddr]|= RS485_RX_BUFF[5];//���ֽ���ǰ

    RS485_TX_BUFF[0]=RS485_Addr;
    RS485_TX_BUFF[1]=0x06;
    RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
    RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
    RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
    RS485_TX_BUFF[5]=RS485_RX_BUFF[5];

    calCRC=CRC_Compute(RS485_TX_BUFF,6);
    RS485_TX_BUFF[7]=calCRC&0xFF;
    RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
    RS485_SendData(RS485_TX_BUFF,8);

    if (startRegAddr>=30&&startRegAddr<70)   //new add �趨д��Χ
    {
//        AT24CXX_Write(startRegAddr,(u8*)Modbus_HoldReg[startRegAddr],2);
        delay_ms(10);
    }
    else
        delay_ms(1);
}
