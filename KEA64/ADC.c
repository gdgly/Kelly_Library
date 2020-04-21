/*
 * ADCWrapper.c
 *
 *  Created on: Dec 25, 2019
 *      Author: FireSourcery
 */
#include "ADC.h"

//#include "PE_Types.h"
#include "PE_Error.h"
#include "IO_Map.h"
#include "ADC_PDD.h"

//#include <stdbool.h>

//extern const uint8_t CHANNEL_TO_PIN[]; //Define elsewhere

//static uint8_t * MapChannelToPin;
//static uint8_t ChannelCount;

//static uint8_t  SampleCount;                						/* Sample count */
//static uint8_t  StatusControlRegVal[ADC_MAX_HW_SAMPLE_COUNT]; 	/* Status and control register values */

/*-----------------------------------------------------------------------------
  ADC hardware functions fed to Measure module
 *----------------------------------------------------------------------------*/
ADC_DATA_T ADC_GetResult(uint8_t unused)
{
	(void)unused;
	return ADC_PDD_GetResultValueRaw(ADC_BASE_PTR, 0U);
}

void ADC_DisableInterrupt(void)
{
	ADC_PDD_DisableConversionCompleteInterrupt(ADC_BASE_PTR, 0U);
}

void ADC_EnableInterrupt(void)
{
	ADC_PDD_EnableConversionCompleteInterrupt(ADC_BASE_PTR, 0U);
}

void ADC_AbortConversion(void)
{
	//Any conversion in progress is aborted in the following cases:
	//A write to ADC_SC1 occurs.
	//A write to ADC_SC2, ADC_SC3, ADC_SC4, ADC_CV occurs.
	ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_SW_TRIGGER);
	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, 0x1FU);
}

void ADC_GetConversionCompleteFlag(void)
{
	ADC_PDD_GetConversionCompleteFlag(ADC_BASE_PTR, 0U);
}

uint8_t ADC_SetConversion(const uint8_t * const channels, uint8_t channelCount, bool hwTrigger)
{
	uint8_t index;
//	uint8_t channelCount = channelCount;
//	uint8_t * channels = channels;

	//if ((SampleCount > ADC_MAX_HW_SAMPLE_COUNT) || (SampleCount == 0U)) 	return ERR_PARAM_SAMPLE_COUNT;
	//if (ADC_PDD_GetConversionTriggerType(ADC_BASE_PTR) != 0U)			return ERR_BUSY;
	//if (ADC_PDD_GetConversionActiveFlag(ADC_BASE_PTR) != 0U)			return ERR_BUSY;
	//ADC_AbortConversion()
	ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_SW_TRIGGER);
	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, 0x1FU);

	//DeviceDataPrv->GroupPtr = &DeviceDataPrv->CreatedGroup;

	ADC_PDD_SetFIFO_Depth(ADC_BASE_PTR, channelCount);
	//DeviceDataPrv->GroupPtr->SampleCount = SampleCount;
//	for (index = 0U; index < channelCount; index++)
//	{
//		//if (SampleGroupPtr[Sample] > ADC_CHANNEL_COUNT - 1) return ERR_PARAM_INDEX;
//		StatusControlRegVal[index] = CHANNEL_TO_PIN[channels[index]];
//	}

	if (hwTrigger)	ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_HW_TRIGGER); /* Select HW triggering */
	//else			ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_SW_TRIGGER); /* Select SW triggering */

	for (index = 0U; index < (channelCount - 1U); index++)
		ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, channels[index]); /* Set channel for all samples except last */

	//ADC_PDD_EnableConversionCompleteInterrupt(ADC_BASE_PTR, 0U);

	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, channels[channelCount - 1U] | ADC_SC1_AIEN_MASK); /* Set last sample of the group with one conversion mode */

	return ERR_OK;
}

void ADC_Init(void)
{
	SIM_SCGC |= SIM_SCGC_ADC_MASK;
	/* Interrupt vector(s) priority setting */
	/* NVIC_IPR3: PRI_15=0x40 */
	NVIC_IPR3 = (uint32_t) ((NVIC_IPR3 & (uint32_t) ~(uint32_t) (NVIC_IP_PRI_15(0xBF))) | (uint32_t) (NVIC_IP_PRI_15(0x40)));
	/* NVIC_ISER: SETENA|=0x8000 */
	NVIC_ISER |= NVIC_ISER_SETENA(0x8000);
	/* SIM_SOPT: ADHWT=2 */
	SIM_SOPT = (uint32_t) ((SIM_SOPT & (uint32_t) ~(uint32_t) (SIM_SOPT_ADHWT(0x01))) | (uint32_t) (SIM_SOPT_ADHWT(0x02)));
	/* ADC_APCTL1: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,ADPC=0xE037 */
	ADC_APCTL1 = ADC_APCTL1_ADPC(0xE037);
	/* ADC_SC3: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,ADLPC=0,ADIV=2,ADLSMP=0,MODE=0,ADICLK=0 */
	ADC_SC3 = (ADC_SC3_ADIV(0x02) | ADC_SC3_MODE(0x00) | ADC_SC3_ADICLK(0x00));
	/* ADC_SC2: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,ADACT=0,ADTRG=0,ACFE=0,ACFGT=0,FEMPTY=0,FFULL=0,REFSEL=0 */
	ADC_SC2 = ADC_SC2_REFSEL(0x00);
	/* ADC_SC4: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,ASCANE=0,ACFSEL=0,??=0,??=0,AFDEP=0 */
	ADC_SC4 = ADC_SC4_AFDEP(0x00);
}
