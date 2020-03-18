/*
 * VoltageDivider.h
 *
 *  Created on: Nov 20, 2019
 *      Author: FireSourcery
 */

#include <stdint.h>

#ifndef VOLTAGEDIVIDER_H_
#define VOLTAGEDIVIDER_H_

typedef struct VoltageDivider
{
	uint32_t VPerADCTop;		/*!< (VREF*(R1_RATIO+R2_RATIO)) */
	uint32_t VPerADCBottom;		/*!< (R2_RATIO*ADC_RES) */
} VOLTAGE_DIVIDER_T;

void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1Ratio, uint32_t r2Ratio, uint8_t vRef, uint16_t adcMax);
uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw);
uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage);

#endif /* VOLTAGEDIVIDER_H_ */
