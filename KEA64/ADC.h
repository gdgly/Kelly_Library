/*
 * ADC.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */
#ifndef _KEA64_ADC_H_
#define _KEA64_ADC_H_

#include <stdint.h>
#include <stdbool.h>

#define ADC_PRPH_BASE_ADDRESS  0x4003B000U

#define ADC_MAX_HW_SAMPLE_COUNT		8U		// Size of FIFO
#define ADC_NO_CHANNEL				0x1FU	// Module disabled

#ifndef ADC_OPTION_16
typedef uint8_t ADC_TResultData;
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_TResultData;
typedef uint16_t ADC_DATA_T;
#endif

uint8_t ADC_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger);
ADC_DATA_T ADC_GetResult(uint8_t);
bool ADC_GetConversionActiveFlag(void);
bool ADC_GetConversionCompleteFlag(void);
void ADC_AbortConversion(void);
void ADC_DisableInterrupt(void);
void ADC_EnableInterrupt(void);

void ADC_Init(uint32_t);
#endif
