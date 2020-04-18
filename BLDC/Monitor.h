/*
 * Monitor.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */

#ifndef _BLDC_MONITOR_H_
#define _BLDC_MONITOR_H_

#include <stdint.h>
#include <stdbool.h>

//volatile union
//{
//	uint16_t Reg;
//	struct
//	{
//		uint16_t E0				:1;
//		uint16_t OverVolt		:1;
//		uint16_t LowVolt		:1;
//		uint16_t TempWarn		:1;
//		uint16_t Startup		:1;
//		uint16_t E5				:1;
//		uint16_t OverTemp		:1;
//		uint16_t StartupTPS		:1;
//		uint16_t Reverse		:1;
//		uint16_t ILOP			:1;
//		uint16_t Identify1		:1;
//		uint16_t Identify2		:1;
//		uint16_t RegenOverVolt	:1;
//		uint16_t HallSensor1	:1;
//		uint16_t HallSensor2	:1;
//		uint16_t Com			:1;
//	};
//} ErrorFlags = {0};

#ifndef ADC_DATA_T_16
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_DATA_T;
#endif


typedef struct
{
	//Back emf is monitored at a particular time
	ADC_DATA_T * BackEMFPhaseA_ADCU;
	ADC_DATA_T * BackEMFPhaseB_ADCU;
	ADC_DATA_T * BackEMFPhaseC_ADCU;
	ADC_DATA_T * BackEMFSelect_ADCU;
	ADC_DATA_T BackEMF_ADCU;

	ADC_DATA_T * I_ADCU;
	ADC_DATA_T * VBat_ADCU;
	ADC_DATA_T * LSTemp_ADCU;

	ADC_DATA_T IZero_ADCU; // == 125
	ADC_DATA_T IMax_ADCU;

	ADC_DATA_T OverVoltage_ADCU;
	ADC_DATA_T UnderVoltage_ADCU;

	bool StallFlag;

}
MONITOR_T;

//uint8_t * BLDC_GetPtrBackEMFPhaseA(BLDC_CONTROLLER_T * bldc);
//uint8_t * BLDC_GetPtrBackEMFPhaseB(BLDC_CONTROLLER_T * bldc);
//uint8_t * BLDC_GetPtrBackEMFPhaseC(BLDC_CONTROLLER_T * bldc);
//uint8_t * BLDC_GetPtrVBat(BLDC_CONTROLLER_T * bldc);
//uint8_t * BLDC_GetPtrI(BLDC_CONTROLLER_T * bldc);
//uint8_t * BLDC_GetPtrLSTemp(BLDC_CONTROLLER_T * bldc);

#endif
