#include "Measure.h"

//ADC Wrapper
//Provide the following:
// Ensures at least 1 conversion is complete, before user can overwrite the initiated measurement
// software trigger will run once, cannot back overwritten until completed
// hw trigger will run least once, cannot back overwritten until first measurement completed
// restores hw trigger trigger, if a software trigger takes place while the hw trigger is active

typedef enum
{
	MEASURE_MODE_COMPLETE = 0,
	MEASURE_MODE_ACTIVE,
//	MEASURE_MODE_SINGLE,
//	MEASURE_MODE_GROUP,
//	MEASURE_MODE_SAMPLE_STOP,
//	MEASURE_MODE_MEASURE,
//	MEASURE_MODE_CONTINUOUS,
//	MEASURE_MODE_CALIBRATING,
} MEASURE_MODE_T;

static volatile MEASURE_MODE_T SampleMode;

typedef enum
{
	MEASURE_TRIGGER_DISABLED = 0,
	MEASURE_TRIGGER_ACTIVE,
	MEASURE_TRIGGER_ON_HOLD,
} MEASURE_TRIGGER_T;

static volatile MEASURE_TRIGGER_T TriggerMode;

static volatile MEASURE_T MeasureSavedTrigger;
static volatile MEASURE_T MeasureActive;

static volatile ADC_DATA_T *	MeasureChannelResultBuffer;		/*!< Measure Result Buffer, measure channel index*/

static const uint8_t * 		ADC_MapChannelToPin;			/*!< Map to translate index channels to ADC pin channels */
static uint8_t 				ADC_ChannelCount;					/*!< Size of Result Buffer, ADC channel count */

static volatile uint8_t * 	ADC_SampleGroupChannelSelect;	/*!< Buffer to store translated ADC channels */
static uint8_t 				ADC_SampleGroupMax;					/*!< Max size of sample group, i.e size of ADC FIFO */

static uint8_t (*ADC_Set)(const uint8_t * const channels, uint8_t channelCount, bool triggerMode);
static ADC_DATA_T (*ADC_GetResult)(uint8_t channel);
static void (*ADC_Disable)(void);
static void (*ADC_DisableIRQ)(void);
static void (*ADC_EnableIRQ)(void);
static bool (*ADC_GetCompleteFlag)(void);
//Queue for ADC without SampleGroup mode?

//extern void RestoreTrigger(void);

void ADC_ISR(void)
{
	uint8_t i;

//	if (SampleMode == MEASURE_MODE_SINGLE)
//		ChannelResultBuffer[ActiveState.SingleChannelSelect] = ADC_GetResult(ActiveState.SingleChannelSelect);
//	else if (SampleMode == MEASURE_MODE_GROUP)

	if (MeasureActive.SampleGroupChannelCount > 1)
		for (i = 0; i < MeasureActive.SampleGroupChannelCount; i++)
			MeasureChannelResultBuffer[MeasureActive.SampleGroupChannelSelect.ChannelGroup[i]] = ADC_GetResult(ADC_SampleGroupChannelSelect[i]);
	else
		MeasureChannelResultBuffer[MeasureActive.SampleGroupChannelSelect.ChannelSingle] = ADC_GetResult(ADC_SampleGroupChannelSelect[i]);

	SampleMode = MEASURE_MODE_COMPLETE;

	if (MeasureActive.ADC_OnEndISR) MeasureActive.ADC_OnEndISR();

	if (TriggerMode == MEASURE_TRIGGER_ON_HOLD)	// restoring a trigger means it has completed once // can check if queue is empty to prevent restore followed by overwrite by onEndISR
		Measure_StartADC(MeasureSavedTrigger.SampleGroupChannelSelect, MeasureSavedTrigger.SampleGroupChannelCount, true, MeasureSavedTrigger.ADC_OnEndISR, false);
}

void ADC_Poll(void)
{
	if (ADC_GetConversionCompleteFlag()) ADC_ISR();
	//if (ADC_GetCompleteFlag()) ADC_ISR();
}


//
//
//void RestoreTrigger(void)
//{
//	uint8_t i;
//
//	for (i = 0; i < MeasureSavedTrigger.SampleGroupChannelCount; i++)
//	{
//		MeasureChannelResultBuffer[MeasureSavedTrigger.SampleGroupChannelSelect[i]] = 0U;
//		ADC_SampleGroupChannelSelect[i] = ADC_MapChannelToPin[MeasureSavedTrigger.SampleGroupChannelSelect[i]];
//	}
//
//	TriggerMode = MEASURE_TRIGGER_ACTIVE;
//	SampleMode = MEASURE_MODE_ACTIVE;
//	MeasureActive.SampleGroupChannelSelect 		= MeasureSavedTrigger.SampleGroupChannelSelect;
//	MeasureActive.SampleGroupChannelCount 		= MeasureSavedTrigger.SampleGroupChannelCount;
//	MeasureActive.ADC_OnEndISR 					= MeasureSavedTrigger.ADC_OnEndISR;
//
//	ADC_Set(ADC_SampleGroupChannelSelect, MeasureSavedTrigger.SampleGroupChannelCount, true);	 /*!< Enables interrupt */
//}

bool Measure_StartADC(MEASURE_CHANNEL_T channels, uint8_t channelCount, bool hwTrigger, void(*onEnd)(void), bool overwrite)
{
	uint8_t i;

	if (channelCount > ADC_SampleGroupMax)					return false;
	if (SampleMode != MEASURE_MODE_COMPLETE && !overwrite)	return false;

	ADC_DisableIRQ();

	if (channelCount > 1)
	{
		for (i = 0; i < channelCount; i++)
		{
			if (channels.ChannelGroup[i] > ADC_ChannelCount - 1) // invalid channel
			{
				if (TriggerMode == MEASURE_TRIGGER_ACTIVE)
					Measure_StartADC(MeasureSavedTrigger.SampleGroupChannelSelect, MeasureSavedTrigger.SampleGroupChannelCount, true, MeasureSavedTrigger.ADC_OnEndISR, false);
					//SampleMode = MEASURE_MODE_COMPLETE; // restoring a trigger means it has completed once
				return false;
			}

			MeasureChannelResultBuffer[channels.ChannelGroup[i]] = 0U;
			ADC_SampleGroupChannelSelect[i] = ADC_MapChannelToPin[channels.ChannelGroup[i]];
		}
	}
	else
	{
		if (channels.ChannelSingle > ADC_ChannelCount - 1)
		{
			if (TriggerMode == MEASURE_TRIGGER_ACTIVE)
				Measure_StartADC(MeasureSavedTrigger.SampleGroupChannelSelect, MeasureSavedTrigger.SampleGroupChannelCount, true, MeasureSavedTrigger.ADC_OnEndISR, false);
				//SampleMode = MEASURE_MODE_COMPLETE; // restoring a trigger means it has completed once
			return false;
		}
		MeasureChannelResultBuffer[channels.ChannelSingle] = 0U;
		ADC_SampleGroupChannelSelect[0] = ADC_MapChannelToPin[channels.ChannelSingle];
	}

//	if (triggerMode && TriggerMode == MEASURE_TRIGGER_ACTIVE) //overwrite trigger?
//	{
//		TriggerMode = MEASURE_TRIGGER_ACTIVE;
//		StateSavedTrigger.SampleGroupChannelCount 	= channelCount;
//		StateSavedTrigger.SampleGroupChannelSelect 	= channels;
//		StateSavedTrigger.ADC_OnEndISR				= onEnd;
//	}
//	else if (triggerMode && TriggerMode == MEASURE_TRIGGER_ON_HOLD) //continue from intterupt
//	{
//		TriggerMode = MEASURE_TRIGGER_ACTIVE;
//		//no change saved state is already set
////		StateSavedTrigger.SampleGroupChannelCount 	= channelCount;
////		StateSavedTrigger.SampleGroupChannelSelect 	= channels;
////		StateSavedTrigger.ADC_OnEndISR				= onEnd;
//	}
//	else if (triggerMode && TriggerMode == MEASURE_TRIGGER_DISABLED)
//	{
//		TriggerMode = MEASURE_TRIGGER_ACTIVE;
//		//saved state dont care?
//		//can set when restore is needed
//	}
//	else if	(!triggerMode && TriggerMode == MEASURE_TRIGGER_ACTIVE)
//	{
//		TriggerMode = MEASURE_TRIGGER_ON_HOLD;
//		StateSavedTrigger = ActiveState;
//	}
//	else if (!triggerMode && TriggerMode == MEASURE_TRIGGER_ON_HOLD) //invalid?
//	{
//		TriggerMode = MEASURE_TRIGGER_ON_HOLD;//no cahnge
//	}
//	else if (!triggerMode && TriggerMode == MEASURE_TRIGGER_DISABLED)
//	{
//		TriggerMode = MEASURE_TRIGGER_DISABLED; //no change
//	}


	if (hwTrigger)
	{
		TriggerMode = MEASURE_TRIGGER_ACTIVE;
	}
	else
	{
		if (TriggerMode == MEASURE_TRIGGER_ACTIVE)
		{
			TriggerMode = MEASURE_TRIGGER_ON_HOLD;
			MeasureSavedTrigger = MeasureActive;				//save active HW trigger state
		}
	}

	SampleMode = MEASURE_MODE_ACTIVE;
	MeasureActive.SampleGroupChannelSelect = channels;
	MeasureActive.SampleGroupChannelCount = channelCount;
	MeasureActive.ADC_OnEndISR = onEnd;

	ADC_Set(ADC_SampleGroupChannelSelect, channelCount, hwTrigger);	 /*!< Enables interrupt */

	return true;
}

//bool Measure_SetMeasureAdaptive(volatile MEASURE_CHANNEL_T channels, uint8_t channelCount, bool triggerMode, void(*onEnd)(void))
//{
//	if (channelCount > 1)	return Measure_SetMeasure(channels.ChannelGroup, channelCount, triggerMode, onEnd);
//	else					return Measure_SetMeasure(&channels.ChannelSingle, channelCount, triggerMode, onEnd);
//}

bool Measure_TriggerMeasureChannel(uint8_t channel, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channel, 1, true, onEnd, true);
}

bool Measure_StartMeasureChannel(uint8_t channel, void(*onEnd)(void)) //need volatile to avoid pass by register
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channel, 1, false, onEnd, true);
}

bool Measure_TriggerMeasureSampleGroup(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channels, channelCount, true, onEnd, true);
}

bool Measure_StartMeasureSampleGroup(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channels, channelCount, false, onEnd, true);
}

bool Measure_TriggerMeasureChannelOverwrite(uint8_t channel, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channel, 1, true, onEnd, true);
}

bool Measure_StartMeasureChannelOverwrite(uint8_t channel, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channel, 1, false, onEnd, true);
}

bool Measure_TriggerMeasureSampleGroupOverwrite(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channels, channelCount, true, onEnd, true);
}

bool Measure_StartMeasureSampleGroupOverwrite(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	return Measure_StartADC((MEASURE_CHANNEL_T)channels, channelCount, false, onEnd, true);
}

bool Measure_StartMeasure(MEASURE_T * Measure, bool overwrite) 	// places measure at head of q, overwrite to overwrite current
{
	return Measure_StartADC(Measure->SampleGroupChannelSelect, Measure->SampleGroupChannelCount, Measure->ADC_HWTrigger, Measure->ADC_OnEndISR, overwrite);
}

bool Measure_QueueMeasure(MEASURE_T * Measure) //place at end of queue
{

}

//void Measure_InitMeasure(MEASURE_T * Measure, MEASURE_CHANNEL_T channels, uint8_t channelCount, bool hwTrigger, void(*onEnd)(void))
//{
//	Measure->SampleGroupChannelSelect = channels;
//	Measure->SampleGroupChannelCount = channelCount;
//	Measure->ADC_HWTrigger = hwTrigger;
//	Measure->ADC_OnEndISR = onEnd;
//}

void Measure_SetTriggerSource(uint8_t source)
{

}

void Measure_WaitForResult()
{
    while (SampleMode != MEASURE_MODE_COMPLETE);
}

void Measure_Disable(void)
{
	SampleMode = MEASURE_MODE_COMPLETE;
	TriggerMode = MEASURE_TRIGGER_DISABLED;
	ADC_Disable();
}

//void KellyADC_SetOnEndISR(void (*fp)(void))
//{
//	ADC_OnEndISR = fp;
//}

// not really needed, since buffer is provided by user
bool Measure_SaveValue(volatile ADC_DATA_T * value, uint8_t channel)
{
	if (channel >= ADC_ChannelCount)	return false;
	*value = MeasureChannelResultBuffer[channel];
	return true;
}

bool Measure_MapAddress(volatile ADC_DATA_T ** address, uint8_t channel)
{
	if (channel >= ADC_ChannelCount)	return false;
	*address = &MeasureChannelResultBuffer[channel];
	return true;
}

volatile ADC_DATA_T Measure_GetValue(uint8_t channel)
{
	if (channel < ADC_ChannelCount)	return MeasureChannelResultBuffer[channel];
	else 							return 0;
}

volatile ADC_DATA_T Measure_GetChannelResult(uint8_t channel)
{
	if (channel < ADC_ChannelCount)	return MeasureChannelResultBuffer[channel];
	else 							return 0;
}

volatile ADC_DATA_T * Measure_GetAddress(uint8_t channel)
{
	if (channel < ADC_ChannelCount)	return &MeasureChannelResultBuffer[channel];
	else 							return 0;
}

void Measure_Init
(
	uint8_t channelCount,
	uint8_t sampleGroupMax,
	const uint8_t * mapChannelToPin,
	ADC_DATA_T * channelResultBuffer,
	uint8_t * adcChannelPinBuffer,
	uint8_t (*setADC)(const uint8_t * const channels, uint8_t channelCount, bool triggerMode),
	ADC_DATA_T (*getADCResult)(uint8_t),
	void (*disableADC)(void),
	void (*disableIRQ)(void)
)
{
	SampleMode 			= MEASURE_MODE_COMPLETE;
	TriggerMode 		= MEASURE_TRIGGER_DISABLED;

	ADC_ChannelCount	= channelCount;
	ADC_SampleGroupMax 	= sampleGroupMax;
	ADC_MapChannelToPin 	= mapChannelToPin;
	MeasureChannelResultBuffer = channelResultBuffer;
	ADC_SampleGroupChannelSelect = adcChannelPinBuffer;

	ADC_GetResult 		= getADCResult;
	ADC_Disable 		= disableADC;
	ADC_DisableIRQ 		= disableIRQ;
	ADC_Set				= setADC;
}
