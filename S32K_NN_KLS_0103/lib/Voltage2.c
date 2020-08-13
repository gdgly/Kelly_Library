/*
 * Voltage2.c
 *
 *  Created on: 2020年8月4日
 *      Author: ted
 */


#include "Voltage2.h"

VOL_STR DC_Bus;

/****************************************/
/*Function: 平均值滤波                */
/*入口参数：        */
/*出口参数：            */
/****************************************/
uint32_t Avg_Filter1(int32_t *Data,uint32_t Buff_Size)
{
	int32_t i=0,Sum=0;

	for(i=0;i<Buff_Size;i++)
		Sum += Data[i];

	return (Sum / Buff_Size);
}

/****************************************/
/*Function: 去除min,max平均滤波                 */
/*入口参数：        */
/*出口参数：            */
/****************************************/
uint32_t Avg_Filter2(int32_t *Data,uint32_t Buff_Size)
{
	int32_t i=0,Sum=0,min=0,max=0;

  min = max = Data[0];

	for(i=0;i<Buff_Size;i++)
	{
		Sum += Data[i];

		if(Data[i] < min)
			min = Data[i];
		if(Data[i] > max)
			max = Data[i];
	}

	return ((Sum - min - max) / (Buff_Size - 2));
}

/****************************************/
/*Function: 配置              */
/*入口参数：        */
/*出口参数：            */
/****************************************/
void Voltage_Params_Config(VOL_STR * pVoltage,uint32_t R1,uint32_t R2,uint32_t LowVoltage,uint32_t OverVoltage)
{
	//AD = ((voltage_10x *R2/(R1+R2)) << ADC_BITS)/ ADC_VREF_10X
	pVoltage->K1 = (R2 << 16) / (R1 + R2) / ADC_VREF_10X;
	//voltage_10x = (AD * ADC_VREF_10X *(R1 + R2)/R2) >> ADC_BITS;
	pVoltage->K2 = ((ADC_VREF_10X * (R1 + R2)) << (16 - ADC_BITS)) / R2;
	pVoltage->LowLimit_10x = LowVoltage;
	pVoltage->HighLimit_10x = OverVoltage;
	pVoltage->Error = NO_ERROR;
}

void Voltage_Buff_Array_Init(VOL_STR * pVoltage,uint32_t ad)//在Voltage_Params_Config之后调用一次
{
	uint32_t i=0;
	pVoltage->Loop = 0;

	pVoltage->Vol_10x = (uint32_t)((uint32_t)(pVoltage->K2 * ad) >> 16);

	for(i=0;i<VOL_BUFF;i++)//第一次采样计算的电压队数组进行填充
		pVoltage->Vol_10x_Array[i] = pVoltage->Vol_10x;
}

/****************************************/
/*Function: 采样值转化为实际电压                 */
/*入口参数：        */
/*出口参数：            */
/****************************************/
void AD_Conversion_to_Voltage(VOL_STR * pVoltage,uint32_t ad)//2ms执行一次，放入2ms任务
{

	pVoltage->AD_Value = ad;
	pVoltage->Vol_10x = (uint32_t)((uint32_t)(pVoltage->K2 * ad) >> 16);
	pVoltage->Vol = (uint32_t)(pVoltage->Vol_10x /10);

	pVoltage->Vol_10x_Array[pVoltage->Loop] = pVoltage->Vol_10x;
	pVoltage->Loop++;
	if(pVoltage->Loop >= VOL_BUFF)
	{
		pVoltage->Loop = 0;
		//均值滤波
		//pVoltage->Vol_10x_Avg = Avg_Filter1((int32_t *)(&(pVoltage->Vol_10x_Array[0])),VOL_BUFF);
		//pVoltage->Vol_Avg = pVoltage->Vol_10x_Avg/10;
	}

	//去除max,min,移动平均滤波
	pVoltage->Vol_10x_Avg = Avg_Filter2((int32_t *)(&(pVoltage->Vol_10x_Array[0])),VOL_BUFF);
	pVoltage->Vol_Avg = pVoltage->Vol_10x_Avg/10;

}

/****************************************/
/*Function: 精度0.1的电压转化为AD值                 */
/*入口参数：        */
/*出口参数：            */
/****************************************/
uint32_t Voltage_10x_Conversion_to_AD(VOL_STR * pVoltage,uint32_t Vol_10x)
{
	return ((uint32_t)(pVoltage->K1 * Vol_10x) >> (16 - ADC_BITS));
}

uint32_t Get_Voltage_Once(VOL_STR * pVoltage)
{
	return pVoltage->Vol;
}

uint32_t Get_Voltage_10x_Once(VOL_STR * pVoltage)
{
	return pVoltage->Vol_10x;
}

uint32_t Get_Voltage_Avg(VOL_STR * pVoltage)
{
	return pVoltage->Vol_Avg;
}

uint32_t Get_Voltage_10x_Avg(VOL_STR * pVoltage)
{
	return pVoltage->Vol_10x_Avg;
}

/****************************************/
/*Function: 故障检测                 */
/*入口参数：        */
/*出口参数：            */
/****************************************/

void VoltageErr_Status_Monitor(VOL_STR * pVoltage,uint32_t LowErr_Times,uint32_t Clear_OV_Times)
{
	static uint32_t Cnt_LowErr=0,Cnt_OverErr=0;
	uint32_t Voltage=0;
	Voltage = Get_Voltage_10x_Avg(pVoltage);
	if(Voltage > pVoltage->HighLimit_10x)
	{
		pVoltage->Error |= OVER_ERROR;
		Cnt_OverErr = Clear_OV_Times;
	}
	else if(Voltage < pVoltage->LowLimit_10x)
	{
		if(Cnt_LowErr == 0)//上电第一次调用即欠压，立即报错
			pVoltage->Error |= LOW_ERROR;

		Cnt_LowErr++;
		if(Cnt_LowErr > LowErr_Times)
			pVoltage->Error |= LOW_ERROR;
	}
	else
	{
			Cnt_LowErr = 1;
		if(Cnt_OverErr > 0)
			Cnt_OverErr--;
		else
			pVoltage->Error &= ~OVER_ERROR;
	}
}


uint32_t Get_VoltageErr_Status(VOL_STR * pVoltage)
{
	return pVoltage->Error;
}

