/*
 * Monitor.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */

#ifndef KELLY_BLDC_MONITOR_H_
#define KELLY_BLDC_MONITOR_H_

#include <stdint.h>


typedef struct
{
	//Back emf is monitored at a particular time
	uint8_t * BackEMFPhaseA_ADCU;
	uint8_t * BackEMFPhaseB_ADCU;
	uint8_t * BackEMFPhaseC_ADCU;
	uint8_t * I_ADCU;

	uint8_t * VBat_ADCU;
	uint8_t * LSTemp_ADCU;

	uint8_t IZero_ADCU; // == 125
	uint8_t IMax_ADCU; // == 125

	uint8_t OverVoltage_ADCU;
	uint8_t UnderVoltage_ADCU;

}
MONITOR_T;

#endif /* KELLY_BLDC_MONITOR_H_ */
