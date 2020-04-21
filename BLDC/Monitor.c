/*
 * Error.c
 *
 *  Created on: Dec 10, 2019
 *      Author: FireSourcery
 */

#include "Monitor.h"

// call high priority from isr adc/pwm and main
MONITOR_ERROR_T Monitor_Process(MONITOR_T * monitor)
{
	//Check adc values
	//I_Max = 50;
	//
	//if(Stall_Flag1)
	//	I_Max=(uint16_t)(I_Max*7)/10;
}

//call low priority from main. can use thread to limit
MONITOR_ERROR_T Monitor_ProcessLowPri(MONITOR_T * monitor)
{

}

//convert adc values to voltage for output. call 1 per s?
void Monitor_Convert(MONITOR_T * monitor)
{
	monitor->BackEMF 		= VoltageDivider_GetVoltage(monitor->VDivBackEMF, monitor->BackEMFBuffer_ADCU);
	monitor->BackEMFSelect 	= VoltageDivider_GetVoltage(monitor->VDivBackEMF, *monitor->BackEMFSelect_ADCU);
	monitor->BackEMFPhaseA 	= VoltageDivider_GetVoltage(monitor->VDivBackEMF, *monitor->BackEMFPhaseA_ADCU);
	monitor->BackEMFPhaseB 	= VoltageDivider_GetVoltage(monitor->VDivBackEMF, *monitor->BackEMFPhaseB_ADCU);
	monitor->BackEMFPhaseC 	= VoltageDivider_GetVoltage(monitor->VDivBackEMF, *monitor->BackEMFPhaseC_ADCU);
	monitor->VBat 			= VoltageDivider_GetVoltage(monitor->VDivBat, 	*monitor->VBat_ADCU);
	monitor->I 				= monitor->I_ADCU;
	monitor->LSTemp 		= monitor->LSTemp_ADCU;


}

void Monitor_ZeroCurrentSensor(MONITOR_T * monitor)
{
	monitor->IZero_ADCU = *monitor->I_ADCU;
}

//ADC_DATA_T Monitor_GetSelectBEMF(MONITOR_T * monitor) { return *monitor->BackEMFSelect_ADCU; }

// select points to phase active during commutation
void Monitor_SelectBEMFPhaseA(MONITOR_T * monitor) { monitor->BackEMFSelect_ADCU = monitor->BackEMFPhaseA_ADCU; }
void Monitor_SelectBEMFPhaseB(MONITOR_T * monitor) { monitor->BackEMFSelect_ADCU = monitor->BackEMFPhaseB_ADCU; }
void Monitor_SelectBEMFPhaseC(MONITOR_T * monitor) { monitor->BackEMFSelect_ADCU = monitor->BackEMFPhaseC_ADCU; }
void Monitor_SelectBEMFBuffer(MONITOR_T * monitor) { monitor->BackEMFSelect_ADCU = &monitor->BackEMFBuffer_ADCU; }

void Monitor_CaptureBEMFPhaseA(MONITOR_T * monitor) { if (*monitor->BackEMFPhaseA_ADCU > 0) monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseA_ADCU; }
void Monitor_CaptureBEMFPhaseB(MONITOR_T * monitor) { if (*monitor->BackEMFPhaseB_ADCU > 0) monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseB_ADCU; }
void Monitor_CaptureBEMFPhaseC(MONITOR_T * monitor) { if (*monitor->BackEMFPhaseC_ADCU > 0) monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseC_ADCU; }

void Monitor_CaptureBEMF(MONITOR_T * monitor)
{
	if (*monitor->BackEMFPhaseA_ADCU >= *monitor->BackEMFPhaseB_ADCU && *monitor->BackEMFPhaseA_ADCU >= *monitor->BackEMFPhaseC_ADCU)
		monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseA_ADCU;

	else if (*monitor->BackEMFPhaseB_ADCU >= *monitor->BackEMFPhaseA_ADCU && *monitor->BackEMFPhaseB_ADCU >= *monitor->BackEMFPhaseC_ADCU)
		monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseB_ADCU;

	else if (*monitor->BackEMFPhaseC_ADCU >= *monitor->BackEMFPhaseA_ADCU && *monitor->BackEMFPhaseC_ADCU >= *monitor->BackEMFPhaseB_ADCU)
		monitor->BackEMFBuffer_ADCU = *monitor->BackEMFPhaseC_ADCU;

//	return monitor->BackEMFBuffer_ADCU;
}

//void BLDC_CalculateBEMFAvg(BLDC_CONTROLLER_T * bldc, uint8_t backEMF_ADCU)
//{
//	bldc->BackEMF_ADCU = backEMF_ADCU;
//}

void Monitor_SetThresholds
(
	MONITOR_T * monitor,
	uint16_t OverVoltage,
	uint16_t UnderVoltage
)
{
}

void Monitor_SetThresholdsADCU
(
	MONITOR_T * monitor,
	ADC_DATA_T OverVoltage_ADCU,
	ADC_DATA_T UnderVoltage_ADCU
)
{
}

void Monitor_MapADC
(
	MONITOR_T * monitor,
	ADC_DATA_T * vBat_ADCU,
	ADC_DATA_T * backEMFPhaseA_ADCU,
	ADC_DATA_T * backEMFPhaseB_ADCU,
	ADC_DATA_T * backEMFPhaseC_ADCU,
	ADC_DATA_T * i_ADCU,
	ADC_DATA_T * temp_ADCU
)
{
	monitor->BackEMFSelect_ADCU = backEMFPhaseA_ADCU;
	monitor->BackEMFPhaseA_ADCU = backEMFPhaseA_ADCU;
	monitor->BackEMFPhaseB_ADCU = backEMFPhaseB_ADCU;
	monitor->BackEMFPhaseC_ADCU = backEMFPhaseC_ADCU;
	monitor->VBat_ADCU 		= vBat_ADCU;
	monitor->I_ADCU 		= i_ADCU;
	monitor->LSTemp_ADCU 	= temp_ADCU;
}

void Monitor_Init
(
	MONITOR_T * monitor,
	ADC_DATA_T * vBat_ADCU,
	ADC_DATA_T * backEMFPhaseA_ADCU,
	ADC_DATA_T * backEMFPhaseB_ADCU,
	ADC_DATA_T * backEMFPhaseC_ADCU,
	ADC_DATA_T * i_ADCU,
	ADC_DATA_T * temp_ADCU,
	VOLTAGE_DIVIDER_T * vDivBat,
	VOLTAGE_DIVIDER_T * vDivBackEMF,
	VOLTAGE_DIVIDER_T * vDivTemperature
)
{
	Monitor_ZeroCurrentSensor(monitor);

	monitor->BackEMFSelect_ADCU = monitor->BackEMFPhaseA_ADCU;

	Monitor_MapADC
	(
		monitor,
		vBat_ADCU,
		backEMFPhaseA_ADCU,
		backEMFPhaseB_ADCU,
		backEMFPhaseC_ADCU,
		i_ADCU,
		temp_ADCU
	);

	monitor->VDivBat 			= vDivBat;
	monitor->VDivBackEMF 		= vDivBackEMF;
	monitor->VDivTemperature 	= vDivTemperature;
}
