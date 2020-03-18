/*
 * Monitor.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */

#ifndef KELLY_BLDC_MONITOR_H_
#define KELLY_BLDC_MONITOR_H_

#include <stdint.h>

#ifndef ADC_DATA_T_16
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_DATA_T;
#endif

typedef struct
{
	//Back emf is monitored at a particular time
//	ADC_DATA_T * BackEMFPhaseA_ADCU;
//	ADC_DATA_T * BackEMFPhaseB_ADCU;
//	ADC_DATA_T * BackEMFPhaseC_ADCU;
	ADC_DATA_T * I_ADCU;

	ADC_DATA_T * VBat_ADCU;
	ADC_DATA_T * LSTemp_ADCU;

	ADC_DATA_T IZero_ADCU; // == 125
	ADC_DATA_T IMax_ADCU; // == 125

	ADC_DATA_T OverVoltage_ADCU;
	ADC_DATA_T UnderVoltage_ADCU;

}
MONITOR_T;

#endif /* KELLY_BLDC_MONITOR_H_ */
