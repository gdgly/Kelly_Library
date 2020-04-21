/*
 * ADCWrapper.h
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */

#ifndef KEA64_ADCWRAPPER_H_
#define KEA64_ADCWRAPPER_H_

#include "PE_Types.h"

#define ADC_PRPH_BASE_ADDRESS  0x4003B000U

#define ADC_MAX_HW_SAMPLE_COUNT		8U		//Size of FIFO
#define ADC_NO_CHANNEL				0x1FU

#ifndef ADC_OPTION_16
typedef uint8_t ADC_TResultData;
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_TResultData;
typedef uint16_t ADC_DATA_T;
#endif

ADC_DATA_T ADC_GetResult(uint8_t);
void ADC_AbortConversion(void);
void ADC_DisableInterrupt(void);
void ADC_EnableInterrupt(void);
uint8_t ADC_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger);

//uint8_t ADC_TriggerMeasurement(uint8_t channel);
//uint8_t ADC_StartMeasurement(uint8_t channel);
//uint8_t ADC_TriggerMeasurementGroup(const uint8_t * const channels, uint8_t channelCount);
//uint8_t ADC_StartMeasurementGroup(const uint8_t * const channels, uint8_t channelCount);

#endif
