/*
 * Voltage2.c
 *
 *  Created on: 2020��8��4��
 *      Author: ted
 */


#include "Voltage2.h"

VOL_STR DC_Bus;

/****************************************/
/*Function: ƽ��ֵ�˲�                */
/*��ڲ�����        */
/*���ڲ�����            */
/****************************************/
uint32_t Avg_Filter1(int32_t *Data,uint32_t Buff_Size)
{
	int32_t i=0,Sum=0;

	for(i=0;i<Buff_Size;i++)
		Sum += Data[i];

	return (Sum / Buff_Size);
}

/****************************************/
/*Function: ȥ��min,maxƽ���˲�                 */
/*��ڲ�����        */
/*���ڲ�����            */
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
/*Function: ����              */
/*��ڲ�����        */
/*���ڲ�����            */
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

void Voltage_Buff_Array_Init(VOL_STR * pVoltage,uint32_t ad)//��Voltage_Params_Config֮�����һ��
{
	uint32_t i=0;
	pVoltage->Loop = 0;

	pVoltage->Vol_10x = (uint32_t)((uint32_t)(pVoltage->K2 * ad) >> 16);

	for(i=0;i<VOL_BUFF;i++)//��һ�β�������ĵ�ѹ������������
		pVoltage->Vol_10x_Array[i] = pVoltage->Vol_10x;
}

/****************************************/
/*Function: ����ֵת��Ϊʵ�ʵ�ѹ                 */
/*��ڲ�����        */
/*���ڲ�����            */
/****************************************/
void AD_Conversion_to_Voltage(VOL_STR * pVoltage,uint32_t ad)//2msִ��һ�Σ�����2ms����
{

	pVoltage->AD_Value = ad;
	pVoltage->Vol_10x = (uint32_t)((uint32_t)(pVoltage->K2 * ad) >> 16);
	pVoltage->Vol = (uint32_t)(pVoltage->Vol_10x /10);

	pVoltage->Vol_10x_Array[pVoltage->Loop] = pVoltage->Vol_10x;
	pVoltage->Loop++;
	if(pVoltage->Loop >= VOL_BUFF)
	{
		pVoltage->Loop = 0;
		//��ֵ�˲�
		//pVoltage->Vol_10x_Avg = Avg_Filter1((int32_t *)(&(pVoltage->Vol_10x_Array[0])),VOL_BUFF);
		//pVoltage->Vol_Avg = pVoltage->Vol_10x_Avg/10;
	}

	//ȥ��max,min,�ƶ�ƽ���˲�
	pVoltage->Vol_10x_Avg = Avg_Filter2((int32_t *)(&(pVoltage->Vol_10x_Array[0])),VOL_BUFF);
	pVoltage->Vol_Avg = pVoltage->Vol_10x_Avg/10;

}

/****************************************/
/*Function: ����0.1�ĵ�ѹת��ΪADֵ                 */
/*��ڲ�����        */
/*���ڲ�����            */
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
/*Function: ���ϼ��                 */
/*��ڲ�����        */
/*���ڲ�����            */
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
		if(Cnt_LowErr == 0)//�ϵ��һ�ε��ü�Ƿѹ����������
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

