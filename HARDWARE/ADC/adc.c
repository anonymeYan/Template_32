#include "adc.h"
#include "delay.h"
#include "dma.h"
#include "ntc.h"
#include "modbus.h"
//////////////////////////////////////////////////////////////////////////////////
u16 T1R=0,T2R=0,T3R=0,T4R=0,T5R=0,T6R=0;
u16* AverageTempRe=0;
u32 ADC_Value=0;
u16 ADC_ConvertedX[10][6]= {0}; //用来存放6个通道的各10个采样值，以后用来求平均值，
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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟 GPIOC时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA时钟

    //PC0 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚 PC0 1 2 3 4 5
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //72M/8=9,ADC最大时间不能超过14M

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在单通道模式（扫描模式）
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在单次转换模式（连续转换模式）
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 6;	//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

    DMA_DeInit(DMA1_Channel1);   //将DMA的通道1寄存器重设为缺省值：ADC1连接DMA通道1
    DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA外设ADC基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedX;      //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //内存作为数据传输的目的地
    DMA_InitStructure.DMA_BufferSize = 10*6;  //此值为完整一轮DMA传输的次数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据位宽度16位，即DMA传输尺寸
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环缓存模式，一轮结束后自动开始下轮传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x禁止内存到内存
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据DMA_InitStruct中参数DMA通道
    DMA_Cmd(DMA1_Channel1,ENABLE);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_239Cycles5);  //ADC规则序列配置		

//	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);

    ADC_DMACmd(ADC1,ENABLE);               //使能ADC的DMA
    ADC_Cmd(ADC1, ENABLE);               //开启AD转换器

    ADC_ResetCalibration(ADC1);	//重置指定的ADC1的校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待
    ADC_StartCalibration(ADC1);		//开始指定ADC1的校准状态
    while(ADC_GetCalibrationStatus(ADC1));		//获取指定ADC1的校准程序,设置状态则等待
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
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
    T1R=GetTempreture(p);////////////100K采样电阻
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


//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
    //设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
//	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_55Cycles5 );	//ADC1,ADC通道,规则采样顺序值为1,采样时间为239.5周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC规则序列配置

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

    return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟

//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	//ADC1复位
//	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);		//ADC1复位借宿

    RCC_ADCCLKConfig(RCC_PCLK2_Div8);   //72M/8=9,ADC最大时间不能超过14M
    //PA1 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
    ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);  //ADC规则序列配置
// 	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5);  //ADC规则序列配置

    ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
//	ADC_DMACmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);	//重置指定的ADC1的校准寄存器

    while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1重置校准寄存器的状态,设置状态则等待

    ADC_StartCalibration(ADC1);		//开始指定ADC1的校准状态

    while(ADC_GetCalibrationStatus(ADC1));		//获取指定ADC1的校准程序,设置状态则等待

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}





void T_Adc_Init(void) //ADC 通道初始化
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                           RCC_APB2Periph_ADC1, ENABLE ); //使能 GPIOA,ADC1 通道时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //分频因子 6 时钟为 72M/6=12MHz
    ADC_DeInit(ADC1); //将外设 ADC1 的全部寄存器重设为缺省值
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE; //模数转换:单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; //顺序进行规则转换的 ADC 通道的数目
    ADC_Init(ADC1, &ADC_InitStructure); //根据指定的参数初始化 ADCx
    ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
    ADC_Cmd(ADC1, ENABLE); //使能指定的 ADC1
    ADC_ResetCalibration(ADC1); //重置指定的 ADC1 的复位寄存器
    while(ADC_GetResetCalibrationStatus(ADC1)); //等待教主年完成
    ADC_StartCalibration(ADC1); //AD 校准
    while(ADC_GetCalibrationStatus(ADC1)); //等待校准完成
}






//得到温度值
//返回值:温度值(扩大了 100 倍,单位:℃.)
short Get_Temprate(void) //获取内部温度传感器温度值
{
    u32 adcx;
    float a=0;
    short result;
    double temperate;
    adcx=Get_Adc_Average(ADC_Channel_16,20); //读取通道 16,20 次取平均
    temperate=(float)adcx*(3.3/4096); //电压值
    temperate=(1.43-temperate)/0.0043+25; //转换为温度值
    result=temperate*=100; //扩大 100 倍.
    if(result<0)
        result=-result;
    T3R=result;
    return result;
}


