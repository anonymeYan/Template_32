#include "ads1110.h"
#include "24cxx.h"
#include "calculate.h"
#include "get_data.h"
#include "delay.h"

#include "ntc.h"
#include "ltc6803-2.h"
#include "sys.h"
#include "wdg.h"
#include "mcp3421.h"


extern u16 I_Positive,I_DPositive,i_in,i_de,ii,vl,reg1;
extern u16 sys_data[40];
extern float current_soc;
extern union number
{
    float f;
    uint8_t c[4];
} my_num;
extern u8 LTC6803_Ini_OK;
extern u8 LTC6803_Ini_OK1;
extern u8 LTC6803Err_Counter;
u8 LTC6803Err_Counter1=0;
extern u8 timeT;
extern u16 CellV[8];
extern u16 Total_CellV;    //总电压
extern u16 CellV_Buff[8]; //电池电压缓存区
extern u16 CellT_Buff[2];          //温度存储区
extern u8 LTC6803Err_Flag;
extern u16 TempT[2];     //温度寄存器组
extern float T2;

u32 trans_data18(u32 data)
{
    if(data & 0x01ffff)   //负数
    {
        u32 buff;
        if(data==0x8000)
            return 0xffff;


        buff=0xffff-data+1;
        //	buff1=(int)(buff + 0x8000);
        return buff;

    }
    else
        return data;

}







void get_data()
{
    u16 buff,buffv;
    u16 ibuff;
    u16 i;//,adcx;
    float i_buff,res,buff1;
    int res1;
    u8 cbuff[4];
    int16_t bue;
    short iy=0;
    unsigned short iyu=0;
    int iyy=0;
    float ifo=0;

    u32 iu32=0;
    u32 i18=0,ito=0;
    float igbu18=0;
    u16 il=0,ih=0,iibuf=0;
    short isg=0;

//	//电流16位
//	buff=READ_I();
//	reg1=buff;
//	res1=(int)buff;
//	res1=trans_data(res1);
//	if(buff&0x8000)
//	{
//		i_buff=(float)res1*0.00915527;     //300/32768
//			if(i_buff<=0)
//				i_buff=0;
//			if(i_buff>=299)
//				i_buff=299;
//	}
//	else
//	{
//		i_buff=(float)res1*0.00915527;
//			if(i_buff<=0)
//				i_buff=0;
//			if(i_buff>=299)
//				i_buff=299;
//	}
//	ibuff=i_buff*10.0;
//	res=(float)i_buff/7200.0;                  //AH

//
//	if(buff&0x8000)
//	{
//		sys_data[1]=ibuff-2*ibuff;
//	  current_soc-=res*0.01*i_de;
//		I_DPositive=ibuff;
//		I_Positive=0;
//	}
//	else
//	{
//		if(ibuff<10 && ibuff>0)
//		{
//			ibuff=0;
//		  res=0;
//		}
//		sys_data[1]=ibuff;
//		current_soc+=res*0.01*i_in;
//		I_Positive=ibuff;
//		I_DPositive=0;
//	}


    //电流18位
    iu32=READ_I_18Bits();
    iu32=trans18Bits(iu32);
    il=(u16)iu32;
    ih=iu32>>16;
    ito=il;
    if(ih&0x01)
        ito+=65536;
    igbu18=(float)ito*0.00076294*3;            //////////ito*(75/2048)*(2048/131072)*(300/75)
    iibuf=igbu18*10;
    if(iu32&0x20000)
    {
        sys_data[1]=iibuf-2*iibuf;
        res=(float)igbu18/7200.0;                  //AH
        current_soc-=res*0.01*i_de;
    }
    else
    {
        sys_data[1]=iibuf;
        res=(float)igbu18/7200.0;                  //AH
        current_soc+=res*0.01*i_in;
    }



////另外一种计算负电流的方法16位
//	iyu=READ_I();
//	iy=(short)iyu;
//	iyy=(int)iy;
//	i_buff=(float)iyy*0.00915527;
//	ifo=i_buff*10;
//  sys_data[1]=(short)ifo;
//	I_DPositive=sys_data[1];
//	res=(float)i_buff/7200.0;                  //AH
//	if(res<0)
//		current_soc+=res*0.01*i_de;
//	else
//		current_soc+=res*0.01*i_in;



    if(current_soc<0)
        current_soc=0;
    if(current_soc>=sys_data[31])
        current_soc=sys_data[31];
    my_num.f=current_soc;
    for(i=0; i<4; i++)
    {
        cbuff[i]=my_num.c[i];
    }

    AT24CXX_Write(16,(u8*)&cbuff,4);
    res1=current_soc/(float)sys_data[31]*1000;     //SOC
    sys_data[2]=(u16)res1;

//	buffv=gather_u();
//	buff1=((float)buffv)*2.048/32768.0;
//	sys_data[0]=buff1*300;
    //6803的数据

    LTC6803Init(0x80,0x49);
    if(!LTC6803_Ini_OK)
    {
        LTC6803Init(0x80,0x49);			              //重新初始化LTC6803
        if(LTC6803_Ini_OK==0)
        {
            LTC6803Err_Counter++;
            if(LTC6803Err_Counter>=5)
                for(i=0; i<8; i++)
                    CellV_Buff[i]=0;
        }
        else
            LTC6803Err_Counter=0;
    }
    else
    {
        ReadCellVol(0x80,0x49);                 //读单体电池电压
        for(i=0; i<8; i++)
            CellV_Buff[i]=CellV[i]/10;                    //计算后的电压值放入缓冲单元
        sys_data[0]=Total_CellV/100;
    }
    delay_us(2);

    /*

    	if(!LTC6803_Ini_OK1)
    	{
    	LTC6803Init(0x81,0x4e);
    		if(LTC6803_Ini_OK1==0)
    		{
    			LTC6803Err_Counter1++;
    			if(LTC6803Err_Counter1>=5)
    				 for(i=0;i<8;i++)
    			CellV_Buff[i+8]=0;
    		}
    		else
    			LTC6803Err_Counter=0;
    	}
    	else
    	{ReadCellVol(0x81,0x4e);                 //读单体电池电压
    		//	sys_data[0]=(sys_data[0]+Total_CellV)/100;         	  //电压
    						for(i=0;i<8;i++)
    				CellV_Buff[i+8]=CellV[i]/10;                    //计算后的电压值放入缓冲单元
    	}
    */
    if(Total_CellV<40000)

        IWDG_Feed();

    delay_ms(2);



}

void Para_Init(void)
{
    u8 x;
    u8 datatemp[50];
    u8 datatemp1[4];
    u16 datax[50];
    u8 da1[2];
    u8 da2[2];
    u8 da3[2];
    u8 da4[2];

    AT24CXX_Read(16,datatemp1,4);
    my_num.c[0]=datatemp1[0];
    my_num.c[1]=datatemp1[1];
    my_num.c[2]=datatemp1[2];
    my_num.c[3]=datatemp1[3];
    current_soc=my_num.f;
    sys_data[2]=current_soc/(float)sys_data[31]*1000;

    AT24CXX_Read(20,datatemp,48);
    for(x=0; x<48; x=x+2)
    {   datax[x]=datatemp[x+1]<<8;
        sys_data[x/2+10]=datax[x]|datatemp[x];
    }

    AT24CXX_Read(110,da1,2);
    ii=(da1[1]<<8)|da1[0];

    AT24CXX_Read(112,da2,2);
    vl=(da2[1]<<8)|da2[0];

    AT24CXX_Read(114,da3,2);
    i_in=(da3[1]<<8)|da3[0];

    AT24CXX_Read(116,da4,2);
    i_de=(da4[1]<<8)|da4[0];

}
























