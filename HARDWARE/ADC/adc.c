#include "adc.h"
#include "delay.h"
#include "dma.h"
#include "ntc.h"
#include "modbus.h"
//////////////////////////////////////////////////////////////////////////////////
u16 T1R=0,T2R=0,T3R=0,T4R=0,T5R=0,T6R=0;
u16* AverageTempRe=0;
u32 ADC_Value=0;
u16 ADC_ConvertedX[10][6]= {0}; //�������6��ͨ���ĸ�10������ֵ���Ժ�������ƽ��ֵ��
//////////////////////////////////////////////////////////////////////////////////
//0 1 2 3 4 5
//1
//2
//3
//4
//5
//6
//7
//8
//9

void ADC_DMA_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ�� GPIOCʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMAʱ��

    //PC0 ��Ϊģ��ͨ����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ���������� PC0 1 2 3 4 5
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //72M/8=9,ADC���ʱ�䲻�ܳ���14M

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//ģ��ת�������ڵ�ͨ��ģʽ��ɨ��ģʽ��
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת�������ڵ���ת��ģʽ������ת��ģʽ��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת���������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 6;	//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

    DMA_DeInit(DMA1_Channel1);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ��ADC1����DMAͨ��1
    DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA����ADC����ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedX;      //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
    DMA_InitStructure.DMA_BufferSize = 10*6;  //��ֵΪ����һ��DMA����Ĵ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //��������λ����16λ����DMA����ߴ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ���16λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ��һ�ֽ������Զ���ʼ���ִ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��x��ֹ�ڴ浽�ڴ�
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct�в���DMAͨ��
    DMA_Cmd(DMA1_Channel1,ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC������������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);  //ADC������������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);  //ADC������������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);  //ADC������������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_239Cycles5);  //ADC������������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_239Cycles5);  //ADC������������		

//	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);

    ADC_DMACmd(ADC1,ENABLE);               //ʹ��ADC��DMA
    ADC_Cmd(ADC1, ENABLE);               //����ADת����

    ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�
    ADC_StartCalibration(ADC1);		//��ʼָ��ADC1��У׼״̬
    while(ADC_GetCalibrationStatus(ADC1));		//��ȡָ��ADC1��У׼����,����״̬��ȴ�
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1������ת����������
}



void GetTemp(void)
{
    float temp1=0,temp2=0;
    u16 kk=0,ik=0,p=0;

	  T1R=0,T2R=0,T3R=0,T4R=0,T5R=0,T6R=0;
	
    for(ik=0; ik<10; ik++)
    {
			T1R+=ADC_ConvertedX[ik][0];
			T2R+=ADC_ConvertedX[ik][1];
			T3R+=ADC_ConvertedX[ik][2];
			T4R+=ADC_ConvertedX[ik][3];
			T5R+=ADC_ConvertedX[ik][4];
			T6R+=ADC_ConvertedX[ik][5];			
    }

    T1R=T1R/10;
    temp1=(float)T1R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000    
    p=temp2;
    T1R=GetTempreture(p);////////////100K��������
    T1R=(u16)T1R-300;



    T2R=T2R/10;
    temp1=(float)T2R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000
    kk=temp2;
    T2R=GetTempreture(kk);
    T2R=(u16)T2R-300;
		
    T3R=T3R/10;
    temp1=(float)T3R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000
    kk=temp2;
    T3R=GetTempreture(kk);
    T3R=(u16)T3R-300;

    T4R=T4R/10;
    temp1=(float)T4R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000
    kk=temp2;
    T4R=GetTempreture(kk);
    T4R=(u16)T4R-300;
		
    T5R=T5R/10;
    temp1=(float)T5R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000
    kk=temp2;
    T5R=GetTempreture(kk);
    T5R=(u16)T5R-300;

    T6R=T6R/10;
    temp1=(float)T6R*(3.3/4096)*1000;
    temp2 = temp1*1000/(3300-temp1);///////////0.1K       100000/1000
    kk=temp2;
    T6R=GetTempreture(kk);
    T6R=(u16)T6R-300;
		
		sys_data[2]=T1R;
		sys_data[3]=T2R;
		sys_data[4]=T3R;
//		sys_data[5]=T4R;
//		sys_data[6]=T5R;
//		sys_data[7]=T6R;
}


//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)
{
    //����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5 );	//ADC1,ADCͨ��,�������˳��ֵΪ1,����ʱ��Ϊ239.5����
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC������������

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1������ת����������

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

    return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
    u32 temp_val=0;
    u8 t;
    for(t=0; t<times; t++)
    {
        temp_val+=Get_Adc(ch);
        delay_ms(5);
    }
    return temp_val/times;
}





void  Adc_Init_single(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��

//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	//ADC1��λ
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);		//ADC1��λ����

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //72M/8=9,ADC���ʱ�䲻�ܳ���14M
    //PA1 ��Ϊģ��ͨ����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת���������������ⲿ��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC������������
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);  //ADC������������
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);  //ADC������������
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);  //ADC������������

    ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
//	ADC_DMACmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);	//����ָ����ADC1��У׼�Ĵ���

    while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1����У׼�Ĵ�����״̬,����״̬��ȴ�

    ADC_StartCalibration(ADC1);		//��ʼָ��ADC1��У׼״̬

    while(ADC_GetCalibrationStatus(ADC1));		//��ȡָ��ADC1��У׼����,����״̬��ȴ�

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1������ת����������

}





void T_Adc_Init(void) //ADC ͨ����ʼ��
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_ADC1, ENABLE ); //ʹ�� GPIOA,ADC1 ͨ��ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //��Ƶ���� 6 ʱ��Ϊ 72M/6=12MHz
    ADC_DeInit(ADC1); //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC ����ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //ģ��ת��:��ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //��������
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC �����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1; //˳����й���ת���� ADC ͨ������Ŀ
    ADC_Init(ADC1, &ADC_InitStructure); //����ָ���Ĳ�����ʼ�� ADCx
    ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
    ADC_Cmd(ADC1, ENABLE); //ʹ��ָ���� ADC1
    ADC_ResetCalibration(ADC1); //����ָ���� ADC1 �ĸ�λ�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ����������
    ADC_StartCalibration(ADC1); //AD У׼
    while(ADC_GetCalibrationStatus(ADC1)); //�ȴ�У׼���
}






//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������ 100 ��,��λ:��.)
short Get_Temprate(void) //��ȡ�ڲ��¶ȴ������¶�ֵ
{
    u32 adcx;
    float a=0;
    short result;
    double temperate;
    adcx=Get_Adc_Average(ADC_Channel_16,20); //��ȡͨ�� 16,20 ��ȡƽ��
    temperate=(float)adcx*(3.3/4096); //��ѹֵ
    temperate=(1.43-temperate)/0.0043+25; //ת��Ϊ�¶�ֵ
    result=temperate*=100; //���� 100 ��.
    if(result<0)
        result=-result;
    T3R=result;
    return result;
}

