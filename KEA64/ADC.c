#include "KEA64/ADC.h"

//#include "Cpu.h"
//#include "PE_Types.h"

#include "IO_Map.h"
#include "ADC_PDD.h"

#include <stdint.h>
#include <stdbool.h>


/*-----------------------------------------------------------------------------
  ADC hardware functions fed to Measure module
 *----------------------------------------------------------------------------*/
//uint8_t ADC_TriggerMeasurement(uint8_t channel);
//uint8_t ADC_StartMeasurement(uint8_t channel);
//uint8_t ADC_TriggerMeasurementGroup(const uint8_t * const channels, uint8_t channelCount);
//uint8_t ADC_StartMeasurementGroup(const uint8_t * const channels, uint8_t channelCount);
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
	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, ADC_NO_CHANNEL);

	ADC_PDD_SetFIFO_Depth(ADC_BASE_PTR, channelCount);
//	for (index = 0U; index < channelCount; index++)
//	{
//		//if (SampleGroupPtr[Sample] > ADC_CHANNEL_COUNT - 1) return ERR_PARAM_INDEX;
//		StatusControlRegVal[index] = CHANNEL_TO_PIN[channels[index]];
//	}

	if (hwTrigger)	ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_HW_TRIGGER); /* Select HW triggering */
	//else			ADC_PDD_SetConversionTriggerType(ADC_BASE_PTR, ADC_PDD_SW_TRIGGER); /* Select SW triggering */

	for (index = 0U; index < (channelCount - 1U); index++)
		ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, channels[index]); /* Set channel for all samples except last */

	//ADC_PDD_EnableConversionCompleteInterrupt(ADC_BASE_PTR, 0U); // enabled with next step
	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, channels[channelCount - 1U] | ADC_SC1_AIEN_MASK); /* Set last sample of the group with one conversion mode */

	return 0;
}

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
	ADC_PDD_WriteStatusControl1Reg(ADC_BASE_PTR, 0U, ADC_NO_CHANNEL);
}

bool ADC_GetConversionActiveFlag(void)
{
	return ADC_PDD_GetConversionActiveFlag(ADC_BASE_PTR);
}

bool ADC_GetConversionCompleteFlag(void)
{
	return ADC_PDD_GetConversionCompleteFlag(ADC_BASE_PTR, 0U);
}

void ADC_Init(uint32_t regAPCTL)
{
	/* Interrupt vector(s) priority setting */
	// priority level 2 (normal)
	/* NVIC_IPR3: PRI_15=0x80 */
	NVIC_IPR3 = (uint32_t) ((NVIC_IPR3 & (uint32_t) ~(uint32_t) (NVIC_IP_PRI_15(0xFF))) | (uint32_t) (NVIC_IP_PRI_15(0x80)));
	/* NVIC_ISER: SETENA|=0x8000 */
	NVIC_ISER |= NVIC_ISER_SETENA(0x8000);

	SIM_SCGC |= SIM_SCGC_ADC_MASK;

	// Set HW trigger FTM2 counter init
	/* SIM_SOPT: ADHWT=0x02 */
	SIM_SOPT = (uint32_t) ((SIM_SOPT & (uint32_t) ~(uint32_t) (SIM_SOPT_ADHWT(0x03))) | (uint32_t) (SIM_SOPT_ADHWT(0x02)));

	ADC_APCTL1 = ADC_APCTL1_ADPC(regAPCTL);
	/* ADC_SC4: ASCANE=0,ACFSEL=0,AFDEP=0 */
	ADC_SC4 = ADC_SC4_AFDEP(0x00) | 0x0100U;
	/* ADC_SC3: ADIV=2,ADLSMP=0,MODE=0,ADICLK=0 */
	ADC_SC3 = (ADC_SC3_ADIV(0x02) | ADC_SC3_MODE(0x00) | ADC_SC3_ADICLK(0x00));
	/* ADC_SC2: ADACT=0,ADTRG=0,ACFE=0,ACFGT=0,FEMPTY=0,FFULL=0,REFSEL=1 */
	ADC_SC2 = ADC_SC2_REFSEL(0x01);
	/* ADC_SC1: COCO=0,AIEN=1,ADCO=1,ADCH=0x1F */
	ADC_SC1 = (ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0x1F));
}

/** ===================================================================
**     Method      :  AdcLdd1_EnableAutoCompare (component ADC_LDD)
*/
/*!
**     @brief
**         This method enables the automatic compare function of the AD
**         converter. The compare function can be configured to check
**         if the result is less than or greater-than-or-equal-to a
**         single compare value, or if the result falls within or
**         outside a range determined by low and high compare values.
**         If evaluated condition isn't true for new measured value,
**         the measurement complete flag returned by
**         [GetMeasurementCompleteStatus] isn't asserted, the event
**         [OnMeasurementComplete] isn't invoked and the
**         [GetMeasuredValues] returns previous result value.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         Mode            - Determines type of compare function.
**                           Possible modes:
**                           Less than threshold - Compare true if the
**                           result is less than the Low compare value.
**                           Greater than or equal to threshold -
**                           Compare true if the result is greater than
**                           or equal to Low compare value.
**                           Inside range, inclusive - Compare true if
**                           the result is greater than or equal to Low
**                           compare value and the result is less than
**                           or equal to High compare value.
**                           Inside range, not inclusive - Compare true
**                           if the result is greater than Low compare
**                           value and the result is less than High
**                           compare value.
**                           Outside range, inclusive - Compare true if
**                           the result is less than or equal to Low
**                           compare value or the result is greater than
**                           or equal to High compare value.
**                           Outside range, not inclusive - Compare true
**                           if the result is less than Low compare
**                           value or the result is greater than High
**                           compare value.
**     @param
**         LowValue        - The low compare level used for
**                           comparison. Data representation is defined
**                           by the [Result type] property.
**     @param
**         HighValue       - The high compare level used
**                           for comparison. Data representation is
**                           defined by the [Result type] property.
**     @return
**                         - Error code
**                           ERR_OK - OK
**                           ERR_SPEED - The device doesn't work in the
**                           active clock configuration
**                           ERR_DISABLED - Component is disabled
**                           ERR_PARAM_RANGE - Low value is greater than
**                           High value (not for signed/unsigned result
**                           type)
**                           ERR_PARAM_MODE - Wrong or unsupported Mode
**                           parameter value
**                           ERR_BUSY - A conversion is already running
*/
/* ===================================================================*/
//LDD_TError AdcLdd1_EnableAutoCompare(LDD_TDeviceData *DeviceDataPtr, LDD_ADC_TCompareMode Mode, uint32_t LowValue, uint32_t HighValue)
//{
//  /* Device state test - this test can be disabled by setting the "Ignore enable test"
//     property to the "yes" value in the "Configuration inspector" */
//  if (!((AdcLdd1_TDeviceDataPtr)DeviceDataPtr)->EnUser) { /* Is the device disabled by user? */
//    return ERR_DISABLED;               /* If yes then error */
//  }
//  /* Low and high value test - this test can be disabled by setting the "Ignore range checking"
//     property to the "yes" value in the "Configuration inspector" */
//  if (LowValue >= 0x0100U) {           /* Is compare value out of range? */
//    return ERR_PARAM_RANGE;            /* Yes, return ERR_PARAM_RANGE */
//  }
//  if (ADC_PDD_GetConversionTriggerType(ADC_BASE_PTR) != 0U) { /* Is HW loop measurement in progress? */
//    return ERR_BUSY;                   /* Yes, return ERR_FAILED */
//  }
//  if (ADC_PDD_GetConversionActiveFlag(ADC_BASE_PTR) != 0U) { /* Last measurement still pending? */
//    return ERR_BUSY;                   /* Yes, return ERR_BUSY */
//  }
//  switch (Mode) {
//    case LDD_ADC_LESS_THAN:
//      ADC_PDD_SetCompareValue1Raw(ADC_BASE_PTR, LowValue);
//      ADC_PDD_SetCompareFunction(ADC_BASE_PTR, (uint32_t)ADC_PDD_LESS_THAN);
//      break;
//    case LDD_ADC_GREATER_THAN_OR_EQUAL:
//      ADC_PDD_SetCompareValue1Raw(ADC_BASE_PTR, LowValue);
//      ADC_PDD_SetCompareFunction(ADC_BASE_PTR, (uint32_t)ADC_PDD_GREATER_THAN_OR_EQUAL);
//      break;
//    default:
//      return ERR_PARAM_MODE;           /* Wrong Mode parameter value */
//    break;
//  }
//  return ERR_OK;
//}
