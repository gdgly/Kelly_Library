/*
 * Monitor.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */

#ifndef _BLDC_MONITOR_H_
#define _BLDC_MONITOR_H_

#include "VoltageDivider/VoltageDivider.h"

#include <stdint.h>
#include <stdbool.h>

#ifndef ADC_DATA_T_16
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_DATA_T;
#endif

typedef volatile union
{
	uint16_t Flags;
	struct
	{
		uint16_t MotorStall		:1;
		uint16_t BatVoltOver	:1;
		uint16_t BatVoltLow		:1;
//		uint16_t OverVolt		:1;
//		uint16_t LowVolt		:1;
//		uint16_t RegenOverVolt	:1;
//
//		uint16_t TempWarn		:1;
//		uint16_t OverTemp		:1;
//		uint16_t StartupTPS		:1;
//		//uint16_t Reverse		:1;
//		//uint16_t ILOP			:1;
//		//uint16_t Identify1		:1;
//		//uint16_t Identify2		:1;
//		//uint16_t Startup		:1;
//		uint16_t HallSensor1	:1;
//		uint16_t HallSensor2	:1;
		//uint16_t Com			:1;
	}
	Flag;
}
MONITOR_ERROR_T;

typedef struct
{
	volatile MONITOR_ERROR_T ErrorFlags;

	//from adc // can generalize to adcvalue/convertedvalue/threasholds/errorresponse function/

	//ADC live values map
	volatile ADC_DATA_T * BackEMFPhaseA_ADCU;
	volatile ADC_DATA_T * BackEMFPhaseB_ADCU;
	volatile ADC_DATA_T * BackEMFPhaseC_ADCU;
	volatile ADC_DATA_T * I_ADCU;
	volatile ADC_DATA_T * VBat_ADCU;
	volatile ADC_DATA_T * LSTemp_ADCU;

	//converted values, use for com Tx
	uint16_t BackEMFPhaseA;
	uint16_t BackEMFPhaseB;
	uint16_t BackEMFPhaseC;
	uint16_t I;
	uint16_t VBat;
	uint16_t LSTemp;

	//volatile ADC_DATA_T * BackEMFSelect_ADCU;
	uint16_t BackEMFSelectBuffer_ADCU;
	uint16_t BackEMFSelect;

	//uint16_t BackEMF;

	//	ADC_DATA_T BackEMF_ADCU;

	//Thresholds
	ADC_DATA_T IZero_ADCU; // == 125
	ADC_DATA_T IMax_ADCU;
	ADC_DATA_T OverVoltage_ADCU;
	ADC_DATA_T UnderVoltage_ADCU;

	// ADC conversion use VoltageDivider struct or function pointer, or primitive factor?
	//holds ADCU voltage conversion ratios
//	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseA;
//	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseB;
//	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseC;
	VOLTAGE_DIVIDER_T * VDivBackEMF; // Assume all backemf channels use same resistor values
	VOLTAGE_DIVIDER_T * VDivBat;
	VOLTAGE_DIVIDER_T * VDivTemperature;
	uint16_t (*ConvertIADCUToAmp)();

	//uint16_t * p_RPM;



}
MONITOR_T;


#endif
