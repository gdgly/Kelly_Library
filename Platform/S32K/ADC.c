/*
 * ADC.c
 *
 *  Created on: Aug 22, 2020
 *      Author: FireSourcery
 */
#include "Platform/S32K/ADC.h"

#include "S32K142/include/S32K142.h"

#include <stdint.h>
#include <stdbool.h>

//uint8_t * const channels always point to array of size 1
void ADC0_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger)
{

	(void) (channelCount); //channelCount always 1

	uint32_t tmp;

    ADC0->SC1[0] = ADC_SC1_ADCH(0x3FU);

	ADC0->SC2 = ((ADC0->SC2 & ~(ADC_SC2_ADTRG_MASK)) | ADC_SC2_ADTRG((uint32_t)hwTrigger)); // set hw trigger

    tmp &= ~(ADC_SC1_ADCH_MASK);
    tmp |= ADC_SC1_ADCH((uint32_t)*channels);
    tmp |= ADC_SC1_AIEN_MASK;

    ADC0->SC1[0] = tmp;
}

ADC_ResultData_t ADC0_GetResult(uint8_t channelIndex)
{
	return ADC0->R[channelIndex];
}

void ADC0_DisableInterrupt(void)
{
    ADC0->SC1[0] = ADC0->SC1[0] & ~ADC_SC1_AIEN_MASK;
}

void ADC0_EnableInterrupt(void)
{
	ADC0->SC1[0] = ADC0->SC1[0] | ADC_SC1_AIEN_MASK;
}

void ADC0_AbortConversion(void)
{
    ADC0->SC1[0] = ADC_SC1_ADCH(0x3FU);
}

bool ADC0_GetConversionActiveFlag(void)
{
    uint32_t tmp = (uint32_t)ADC0->SC2;
    tmp = (tmp & ADC_SC2_ADACT_MASK) >> ADC_SC2_ADACT_SHIFT;
    return (tmp != 0u) ? true : false;
}

bool ADC0_GetConversionCompleteFlag(void)
{

    uint32_t tmp = ADC0->SC1[chanIndex];
    tmp = (tmp & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT;
    return (tmp != 0u) ? true : false;
}
