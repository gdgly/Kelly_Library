/*
 * ADC.h
 *
 *  Created on: Aug 22, 2020
 *      Author: FireSourcery
 */

#ifndef PLATFORM_S32K_ADC0_H
#define PLATFORM_S32K_ADC_H

#include "adc_driver.h"

#include "S32K142/include/S32K142.h"
#include <stdint.h>
#include <stdbool.h>

#ifndef ADC_OPTION_16
typedef uint8_t ADC_ResultData_t;
#else
typedef uint16_t ADC_ResultData_t;
#endif

void ADC0_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger)
{
	static inline void ADC_SetInputChannel(ADC_Type * const baseAddr,
	                                       const uint8_t chanIndex,
	                                       const adc_inputchannel_t inputChan,
	                                       const bool state)

	ADC0->SC2 = ((ADC0->SC2 & ~(ADC_SC2_ADTRG_MASK)) | ADC_SC2_ADTRG((uint32_t)hwTrigger)); // set hw trigger

}

ADC_ResultData_t ADC0_GetResult(uint8_t channelIndex)
{
	return ADC0->R[channelIndex];
}

void ADC0_DisableInterrupt(void)
{

}

void ADC0_EnableInterrupt(void)
{

}

void ADC0_AbortConversion(void)
{

}

bool ADC0_GetConversionActiveFlag(void)
{
    uint32_t tmp = (uint32_t)baseAddr->SC2;
    tmp = (tmp & ADC_SC2_ADACT_MASK) >> ADC_SC2_ADACT_SHIFT;

    return (tmp != 0u) ? true : false;
}

bool ADC0_GetConversionCompleteFlag(void)
{

}

void ADC0_Init(void)
{
	static const adc_converter_config_t ADC_0_ConvConfig0 = {
	  .clockDivide = ADC_CLK_DIVIDE_4,
	  .sampleTime = 255U,
	  .resolution = ADC_RESOLUTION_12BIT,
	  .inputClock = ADC_CLK_ALT_1,
	  .trigger = ADC_TRIGGER_HARDWARE,
	  .pretriggerSel = ADC_PRETRIGGER_SEL_PDB,
	  .triggerSel = ADC_TRIGGER_SEL_PDB,
	  .dmaEnable = false,
	  .voltageRef = ADC_VOLTAGEREF_VREF,
	  .continuousConvEnable = false,
	  .supplyMonitoringEnable = false
	};

	ADC_DRV_ConfigConverter(0, &ADC_0_ConvConfig0);
	ADC_DRV_AutoCalibration(0);
	INT_SYS_EnableIRQ(ADC0_IRQn);
}



#endif /* PLATFORM_S32K_ADC0_H_ */
