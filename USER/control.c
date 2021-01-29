#include "control.h"
#include "sys.h"
#include "mcp3421.h"
#include "mcp4726.h"
#include "24cxx.h"
#include "modbus.h"
#include "adc.h"
#include "timer.h"
#include "led.h"


extern u16 SetI;

void run(void)
{
    u16 buff=0,ibuf16b=0;
    int res=0;
    float i_buff=0;
    u16   vBuff=0;
    int   vBuffInt=0;
    float vBuffFloat=0;

////读取电流值16位/////////i=(float)res*(2048/32768)*(75/2048)*(300/75) 单位A///////////////////////////////
    buff=MCP3421_A0.MCP3421_READ();
    sys_data[5]=buff;
    res=trans_16(buff);
    i_buff=(float)res*0.00305175*3;  //300A分流器 AD620放大倍数G=(49.4K/R)+1；R=1.8K+75Ω; 单位A
    i_buff=i_buff*100+0.5;   //扩大100倍 保留两位小数
    ibuf16b=i_buff;
    if(buff&0x8000)
    {
        sys_data[1]=ibuf16b-2*ibuf16b;
    }
    else
    {
        sys_data[1]=ibuf16b;
    }
//////读取电压值16位////////////////////////////////////////
    vBuff=MCP3421_A1.MCP3421_READ();
    sys_data[4]=vBuff;
    vBuffInt=trans_16(vBuff);
    vBuffFloat=((float)vBuffInt)*2.048/32768.0;
    vBuffFloat=vBuffFloat*3000+0.5;  //保留两位小数
//    vBuffFloat=vBuffFloat*3000*1.0079-0.0119+0.5;  //保留两位小数
    if(vBuff&0x8000)
    {
        sys_data[0]=0;
    }
    else
    {
        sys_data[0]=vBuffFloat;
    }
//////给定////////////////////////////////////////
    SetI=sys_data[10];
    MCP4726_A0.MCP4726_SetDAC(SetI);
}

void run18(void)
{
    u32 buf_18bit=0;
    int res_18bit=0;
    float i_18bit=0;
    u16 ibuf18b=0;
    u32 vbuf_18bit=0;
    int vbufInt_18bit=0;
    float v_18bit=0;
//////读取电流值18位////////////i=(float)res*(2048/2^17)*(75/2048)*(300/75) 单位A////////////////////////////
    buf_18bit=MCP3421_A0.MCP3421_READ_18Bits();
    res_18bit=trans_18(buf_18bit);
    i_18bit=(float)res_18bit*0.00076294*3; //300A分流器 单位A
    ibuf18b=i_18bit*100;//扩大100倍
    if(buf_18bit&0x020000)
    {
        sys_data[3]=ibuf18b-2*ibuf18b;
    }
    else
    {
        sys_data[3]=ibuf18b;
    }
//////读取电压值18位////////////////////////////////////////
    vbuf_18bit=MCP3421_A1.MCP3421_READ_18Bits();
    vbufInt_18bit=trans_18(vbuf_18bit);
    v_18bit=((float)vbufInt_18bit)*2.048/131072;
    v_18bit=v_18bit*3000;  //保留两位小数
    if(vbuf_18bit&0x020000)
    {
        sys_data[2]=0;
    }
    else
    {
        sys_data[2]=v_18bit;
    }
}

void Para_Init(void)
{
    u8 datatemp[50];
    u16 datax[50];
    u8 datatemp1[4];
    u8 x;

    for(x=0; x<10; x=x+2)
    {
        datax[x]=datatemp[x+1]<<8;
        sys_data[x/2+30]=datax[x]|datatemp[x];
    }
}


/***********************************************************
//函数名：  trans_16
//作者：    yan
//功能：    将16位ADC的采样值（负数为补码）转换为绝对值
//输入参数：data  类型（uint16_t)	 需要转换的数据
//返回值：        类型（int32_t)  绝对值
************************************************************/
int32_t trans_16(uint16_t data)
{
    uint16_t buff;
    if(data & 0x8000)
    {
        if(data==0x8000)
            return 0x8000;
        buff=(data-1)^0xffff;
        return buff;
    }
    else
        return data;
}

/***********************************************************
//函数名：  trans_16
//作者：    yan
//功能：    将18位ADC的采样值（负数为补码）转换为绝对值
//输入参数：data  类型（uint32_t)	 需要转换的数据
//返回值：        类型（uint32_t)  绝对值
************************************************************/
uint32_t trans_18(uint32_t data)
{
    uint32_t buff;
    if(data & 0x020000)
    {
        if(data==0x020000)
            return 0x020000;
        buff=(data-1)^0x03ffff;
        return buff;
    }
    else
        return data;
}

