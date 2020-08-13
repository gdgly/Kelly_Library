/*
 * Voltage2.h
 *
 *  Created on: 2020年8月8日
 *      Author: Admin
 */

#ifndef VOLTAGE2_H_
#define VOLTAGE2_H_

#include "typedefine.h"

#define ADC_BITS 						10//8,10,12
#define ADC_VREF_10X				50//33,50; 采样基准电压
#define VOL_BUFF						32

#define NO_ERROR						0
#define LOW_ERROR						0x01
#define OVER_ERROR					0x02



typedef struct
{
	uint32_t K1;//voltage_10 to ad
	uint32_t K2;//ad to voltage_10
	uint32_t LowLimit_10x;
	uint32_t HighLimit_10x;
	uint32_t Error;
	uint32_t Vol_10x;
	uint32_t Vol;
	uint32_t Vol_10x_Array[VOL_BUFF];
	uint32_t Loop;
	uint32_t Vol_10x_Avg;
	uint32_t Vol_Avg;
	uint32_t AD_Value;
}VOL_STR;

extern VOL_STR DC_Bus;

APP_SECTION uint32_t Avg_Filter1(int32_t *Data,uint32_t Buff_Size);
APP_SECTION uint32_t Avg_Filter2(int32_t *Data,uint32_t Buff_Size);
APP_SECTION void Voltage_Params_Config(VOL_STR * pVoltage,uint32_t R1,uint32_t R2,uint32_t LowVoltage,uint32_t OverVoltage);
APP_SECTION void Voltage_Buff_Array_Init(VOL_STR * pVoltage,uint32_t ad);
APP_SECTION void AD_Conversion_to_Voltage(VOL_STR * pVoltage,uint32_t ad);
APP_SECTION uint32_t Voltage_10x_Conversion_to_AD(VOL_STR * pVoltage,uint32_t Vol_10x);
APP_SECTION uint32_t Get_Voltage_Once(VOL_STR * pVoltage);
APP_SECTION uint32_t Get_Voltage_10x_Once(VOL_STR * pVoltage);
APP_SECTION uint32_t Get_Voltage_Avg(VOL_STR * pVoltage);
APP_SECTION uint32_t Get_Voltage_10x_Avg(VOL_STR * pVoltage);
APP_SECTION void VoltageErr_Status_Monitor(VOL_STR * pVoltage,uint32_t LowErr_Times,uint32_t Clear_OV_Times);
APP_SECTION uint32_t Get_VoltageErr_Status(VOL_STR * pVoltage);

#endif /* VOLTAGE2_H_ */
