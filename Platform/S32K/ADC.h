/*
 * ADC.h
 *
 *  Created on: Aug 22, 2020
 *      Author: FireSourcery
 */

#ifndef PLATFORM_S32K_ADC_H
#define PLATFORM_S32K_ADC_H

#include <stdint.h>
#include <stdbool.h>

#ifndef ADC_DATA_T_16
typedef uint8_t ADC_ResultData_t;
#else
typedef uint16_t ADC_ResultData_t;
#endif

void ADC0_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger);
ADC_ResultData_t ADC0_GetResult(uint8_t channelIndex);
void ADC0_DisableInterrupt(void);
void ADC0_EnableInterrupt(void);
void ADC0_AbortConversion(void);
bool ADC0_GetConversionActiveFlag(void);
bool ADC0_GetConversionCompleteFlag(void);

#endif
