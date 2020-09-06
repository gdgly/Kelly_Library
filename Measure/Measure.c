#include "Measure.h"

//ADC Wrapper Module

// Module Shared
static volatile ADC_DATA_T *	p_ChannelResultBuffer;	/*!< Measure Result Buffer, measure/virtual channel index*/
static volatile uint32_t *	 	p_ChannelSumBuffer;		/*!< sum if multiple measurements are required*/
static const uint8_t * 			p_ChannelToADCPin;		/*!< Map to translate index channels to ADC pin channels */
static uint8_t					ChannelCount;			/*!< Size of Result Buffer, ADC channel count */
//extern const uint8_t MEASURE_CHANNEL_TO_ADC_PIN[]; // User define, shared for all adcs and samples
//extern volatile ADC_DATA_T 	Measure_ChannelResultBuffer[];		/*!< Measure Result Buffer, measure channel index*/
//extern volatile uint32_t 	Measure_ChannelSumBuffer[];			/*!< sum if multiple measurements are required*/
//#define MEASURE_CHANNEL_COUNT (sizeof(Measure_ChannelResultBuffer)/sizeof(ADC_DATA_T))

static void (*DisableIRQ)(void);	/*!< critical section */
static void (*EnableIRQ)(void);		/*!< critical section */

static inline bool StartADC(MEASURE_T * measure, MEASURE_CHANNEL_T channels, uint8_t channelCount, bool hwTrigger, void(*onEnd)(void), bool overwrite)
{
	uint8_t i;

//	if (channelCount > measure->ADC_SampleChannelCountMax)	return false;
	if (measure->ADC_GetActiveFlag() && !overwrite)			return false;

	// for modifying measure->ADC_SampleChannels of same measure struct. can occur without calling inside interrupts, i.e need to be implemented if global is not
	if (measure->ADC_GetActiveFlag() && overwrite) measure->ADC_DisableIRQ();

	// need to be implemented if calling from inside interrupts. e.g ftm interrupt call occurs during main call
	if (DisableIRQ) DisableIRQ();

	if (channelCount > 1)
	{
		for (i = 0; i < channelCount; i++)
		{
//			if (channels.ChannelGroup[i] > ChannelCount - 1) // invalid channel
//			{
//				EnableIRQ();
//				return false;
//			}
			//ChannelResultBuffer[channels.ChannelGroup[i]] = 0; //todo settings to reset before measure
			measure->ADC_SampleChannels[i] = p_ChannelToADCPin[channels.ChannelGroup[i]];
		}
	}
	else
	{
//		if (channels.ChannelSingle > ChannelCount - 1)
//		{
//			EnableIRQ();
//			return false;
//		}
		//ChannelResultBuffer[channels.ChannelSingle] = 0; //todo settings to reset before measure
		measure->ADC_SampleChannels[0] = p_ChannelToADCPin[channels.ChannelSingle];
	}

	measure->ADC_Set(measure->ADC_SampleChannels, channelCount, hwTrigger);	 /*!< Enables adc complete interrupt */

	if (EnableIRQ) EnableIRQ();

	return true;
}

// run in corresponding ADC ISR
inline void Measure_CompleteISR(MEASURE_T * measure)
{
	uint8_t i;

	if (measure->Sample->ChannelCount > 1)
	{
		//		if(measure->Sample->NRepeat)
		for (i = 0; i < measure->Sample->ChannelCount; i++)
			p_ChannelResultBuffer[measure->Sample->Channels.ChannelGroup[i]] = measure->ADC_GetResult(measure->ADC_SampleChannels[i]);
	}
	else
	{
		p_ChannelResultBuffer[measure->Sample->Channels.ChannelSingle] = measure->ADC_GetResult(measure->ADC_SampleChannels[0]);
	}

	if (measure->Sample->OnEndISR) measure->Sample->OnEndISR();
}

void Measure_CompletePoll(MEASURE_T * measure)
{
	if (measure->ADC_GetCompleteFlag()) Measure_CompleteISR(measure);
}

volatile ADC_DATA_T Measure_GetChannelResult(MEASURE_T * measure, uint8_t channel)
{
	if (channel < ChannelCount)	return p_ChannelResultBuffer[channel];
	else 						return 0;
}

volatile ADC_DATA_T * Measure_GetChannelPtr(MEASURE_T * measure, uint8_t channel)
{
	if (channel < ChannelCount)	return &p_ChannelResultBuffer[channel];
	else 						return 0;
}


void Measure_ResetChannelResult(MEASURE_T * measure, uint8_t channel)
{
	p_ChannelResultBuffer[channel] = 0;
}

void Measure_InitSample
(
	MEASURE_SAMPLE_T * sample,
	MEASURE_CHANNEL_T channels,
	uint8_t channelCount,
	bool hwTrigger,
	void(*onEnd)(void),
	bool overwrite,
	uint8_t repeatCount
)
{
	sample->Channels 		= channels;
	sample->ChannelCount 	= channelCount;
	sample->HWTrigger 		= hwTrigger;
	sample->OnEndISR 		= onEnd;
	sample->Overwrite 		= overwrite;
	sample->RepeatCount		= repeatCount;
	sample->RepeatCounter	= 0;
}

bool Measure_Start(MEASURE_T * measure, MEASURE_SAMPLE_T * sample)
{
	measure->Sample = sample;
	return StartADC(measure, sample->Channels, sample->ChannelCount, sample->HWTrigger, sample->OnEndISR, sample->Overwrite);
}

// queue implementation
// places measure at head of q, overwrite to overwrite current
// place at end of queue
// todo implement with queue:
// Ensures at least 1 conversion is complete, before user can overwrite the initiated measurement
// software trigger will run once, cannot back overwritten until completed
// hw trigger will run least once, cannot back overwritten until first measurement completed
// restores hw trigger trigger, if a software trigger takes place while the hw trigger is active
bool Measure_Queue(MEASURE_T * measure, MEASURE_SAMPLE_T * sample)
{

}

bool Measure_StartADC(MEASURE_T * measure, MEASURE_CHANNEL_T channels, uint8_t channelCount, bool hwTrigger, void(*onEnd)(void), bool overwrite)
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channels, channelCount, hwTrigger, onEnd, overwrite, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

// various start calls using primitive data types
bool Measure_StartChannel(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channel, 1, false, onEnd, false, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_StartChannelOverwrite(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channel, 1, false, onEnd, true, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_TriggerChannel(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channel, 1, true, onEnd, false, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_TriggerChannelOverwrite(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channel, 1, true, onEnd, true, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_StarChannelGroup(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channels, channelCount, false, onEnd, false, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_StartChannelGroupOverwrite(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channels, channelCount, false, onEnd, true, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_TriggerChannelGroup(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channels, channelCount, true, onEnd, false, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

bool Measure_TriggerChannelGroupOverwrite(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void))
{
	Measure_InitSample(&measure->SampleBuffer, (MEASURE_CHANNEL_T)channels, channelCount, true, onEnd, true, 1);
	return Measure_Start(measure, &measure->SampleBuffer);
}

//void Measure_SetTriggerSource(uint8_t source)
//{
//
//}

void Measure_WaitForResult(MEASURE_T * measure)
{
	while(!measure->ADC_GetCompleteFlag());
}

void Measure_Disable(MEASURE_T * measure)
{
	measure->ADC_Disable();
}

//Init per ADC
void Measure_Init
(
	MEASURE_T * measure,
	uint8_t (*setADC)(const uint8_t * const channels, uint8_t channelCount, bool triggerMode),
	uint8_t * adcSampleChannelsBuffer,
	uint8_t	adcSampleChannelCountMax,
	ADC_DATA_T (*getADCResult)(uint8_t),
	void (*disableADCIRQ)(void),		//needed for overwrite function
	bool (*getADCActiveFlag)(void),		//needed for overwrite function
	bool (*getADCCompleteFlag)(void), 	//optional for polling
	void (*disableADC)(void)			//optional for disable
)
{
	measure->ADC_Set					= setADC;
	measure->ADC_SampleChannels			= adcSampleChannelsBuffer;
	measure->ADC_SampleChannelCountMax	= adcSampleChannelCountMax;
	measure->ADC_GetResult				= getADCResult;
	measure->ADC_GetCompleteFlag		= getADCCompleteFlag;
	measure->ADC_GetActiveFlag			= getADCActiveFlag;
	measure->ADC_Disable				= disableADC;
	measure->ADC_DisableIRQ				= disableADCIRQ;
}


void Measure_InitModule
(
	ADC_DATA_T * 	channelResultBuffer,
	uint32_t *	 	channelSumBuffer,
	uint8_t * 		channelToADCPinTable,
	uint8_t			channelCount,
	void (*disableGlobalIRQ)(void), 	//must implement if calling Measure to start ADC inside interrupts
	void (*enableGlobalIRQ)(void) 		//must implement if calling Measure to start ADC inside interrupts
)
{
	p_ChannelResultBuffer = channelResultBuffer;
	p_ChannelSumBuffer = channelSumBuffer;
	p_ChannelToADCPin = channelToADCPinTable;
	ChannelCount = channelCount;
	DisableIRQ = disableGlobalIRQ;
	EnableIRQ =	enableGlobalIRQ;
}
