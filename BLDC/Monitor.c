/*
 * Error.c
 *
 *  Created on: Dec 10, 2019
 *      Author: FireSourcery
 */

#include "Monitor.h"

// call from main or adc isr
void Monitor_Process(MONITOR_T * monitor)
{
	//Check adc values
	//I_Max = 50;
	//
	//if(Stall_Flag1)
	//	I_Max=(uint16_t)(I_Max*7)/10;
}

void Monitor_ZeroCurrentSensor(MONITOR_T * monitor)
{
	monitor->IZero_ADCU = *monitor->I_ADCU;
}

void Monitor_SetThresholds
(
	MONITOR_T * monitor,
	uint32_t x
)
{
}

void Monitor_Init
(
	MONITOR_T * monitor,
	ADC_DATA_T * backEMFPhaseA_ADCU,
	ADC_DATA_T * backEMFPhaseB_ADCU,
	ADC_DATA_T * backEMFPhaseC_ADCU,
	ADC_DATA_T * i_ADCU,
	ADC_DATA_T * vBat_ADCU,
	ADC_DATA_T * temp_ADCU
)
{
	Monitor_ZeroCurrentSensor(monitor);
}


///*-----------------------------------------------------------------------------
//  Monitor
// *----------------------------------------------------------------------------*/
//uint8_t * BLDC_GetPtrBackEMFPhaseA(BLDC_CONTROLLER_T * bldc)
//{
//	return&bldc->BackEMFPhaseA_ADCU;
//}
//
//uint8_t * BLDC_GetPtrBackEMFPhaseB(BLDC_CONTROLLER_T * bldc)
//{
//	return bldc->BackEMFPhaseB_ADCU;
//}
//
//uint8_t * BLDC_GetPtrBackEMFPhaseC(BLDC_CONTROLLER_T * bldc)
//{
//	return bldc->BackEMFPhaseC_ADCU;
//}
//
//uint8_t * BLDC_GetPtrVBat(BLDC_CONTROLLER_T * bldc)
//{
//	return bldc->VBat_ADCU;
//}
//
//uint8_t * BLDC_GetPtrI(BLDC_CONTROLLER_T * bldc)
//{
//	return bldc->I_ADCU;
//}
//
//uint8_t * BLDC_GetPtrLSTemp(BLDC_CONTROLLER_T * bldc)
//{
//	return bldc->LSTemp_ADCU;
//}
//
//
//void BLDC_MapBackEMFPhaseA(BLDC_CONTROLLER_T * bldc, uint8_t * address)
//{
//	bldc->BackEMFPhaseA_ADCU = address;
//}
//
//void BLDC_MapBackEMFPhaseB(BLDC_CONTROLLER_T * bldc, uint8_t * address)
//{
//	bldc->BackEMFPhaseB_ADCU = address;
//}
//
//void BLDC_MapBackEMFPhaseC(BLDC_CONTROLLER_T * bldc, uint8_t * address)
//{
//	bldc->BackEMFPhaseC_ADCU = address;
//}
