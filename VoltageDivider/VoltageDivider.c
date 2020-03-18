/*
 * VoltageDivider.c
 *
 *  Created on: Nov 20, 2019
 *      Author: FireSourcery
 */
#include "VoltageDivider.h"

//DIV = R2RATIO/(R1RATIO + R2RATIO)
//VDIV = V*DIV
//VDIV_PER_ADC = VREF/ADCMAX
//ADC = VDIV/VDIV_PER_ADC

//V_PER_ADC == V/ADC:
//VDIV = V*DIV = ADC*VDIV_PER_ADC
//V_PER_ADC = VDIV_PER_ADC/DIV = VREF/ADCMAX/(R2RATIO/(R1RAIO + R2RATIO))
//V_PER_ADC = VREF*(R1RAIO + R2RATIO)/(ADCMAX*R2RATIO)

void VoltageDivider_Init(VOLTAGE_DIVIDER_T * div, uint32_t r1Ratio, uint32_t r2Ratio, uint8_t vRef, uint16_t adcMax)
{
	div->VPerADCTop = vRef*(r1Ratio+r2Ratio);
	div->VPerADCBottom = adcMax*r2Ratio;
}

uint16_t VoltageDivider_GetVoltage(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw)
{
	return (adcRaw*div->VPerADCTop)/div->VPerADCBottom; // (adcRaw*VREF*(R1_RATIO+R2_RATIO))/(R2_RATIO*ADC_RES);
}

uint16_t VoltageDivider_GetVoltage10(VOLTAGE_DIVIDER_T * div, uint16_t adcRaw, uint8_t precision)
{
	return (adcRaw*div->VPerADCTop*(10^precision))/div->VPerADCBottom;
}

uint16_t VoltageDivider_GetADCRaw(VOLTAGE_DIVIDER_T * div, uint16_t voltage)
{
	return (voltage*div->VPerADCBottom)/div->VPerADCTop;
}

