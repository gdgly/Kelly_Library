/*
 * VoltageDivider.h
 *
 *  Created on: Nov 20, 2019
 *      Author: FireSourcery
 */

#include <stdint.h>

#ifndef _VOLTAGE_DIVIDER_H_
#define _VOLTAGE_DIVIDER_H_

typedef struct VoltageDivider
{
	uint32_t VPerADCFactor;			/*!< (VREF*(R1_RATIO+R2_RATIO)) */
	uint32_t VPerADCDivisor;		/*!< (R2_RATIO*ADC_RES) */
#ifdef VOLTAGE_DIVIDER_CONFIG_FAST
	uint32_t ADCBits;
#endif
} VOLTAGE_DIVIDER_T;

void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1, uint32_t r2, uint8_t vRef, uint8_t adcBits);
uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw);
uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage);

#endif /* _VOLTAGE_DIVIDER_H_ */
