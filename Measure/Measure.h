#ifndef _MEASURE_H_
#define _MEASURE_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef ADC_DATA_T_16
typedef uint8_t ADC_DATA_T;
#else
typedef uint16_t ADC_DATA_T;
#endif

typedef union
{
	uint8_t ChannelSingle;
	const uint8_t * ChannelGroup;
} MEASURE_CHANNEL_T;

typedef struct
{
	MEASURE_CHANNEL_T Channels;
	uint8_t ChannelCount;
	bool HWTrigger;
	void(*OnEndISR)(void);
	bool Overwrite;
	uint8_t RepeatCount;
	uint8_t RepeatCounter;
} MEASURE_SAMPLE_T;

typedef struct
{
	uint8_t (*ADC_Set)(const uint8_t * const channels, uint8_t channelCount, bool triggerMode);
	uint8_t * ADC_SampleChannels;			/*!< Buffer to store translated ADC pin channels */
	uint8_t	ADC_SampleChannelCountMax;		/*!< Max size of sample group, i.e size of ADC FIFO */
	ADC_DATA_T (*ADC_GetResult)(uint8_t channel);
	bool (*ADC_GetCompleteFlag)(void);
	bool (*ADC_GetActiveFlag)(void);
	void (*ADC_Disable)(void);
	void (*ADC_DisableIRQ)(void);

	MEASURE_SAMPLE_T * 	Sample;
	MEASURE_SAMPLE_T 	SampleBuffer; //used in functions passing primitive data
} MEASURE_T;

typedef enum
{
	MEASURE_SW_TRIGGER = 0,
	MEASURE_HW_TRIGGER = 1,
} MEASURE_TRIGGER_MODE_T;

typedef enum
{
	MEASURE_ERROR_OK = 0,
	MEASURE_ERROR_ = 1,
} MEASURE_ERROR_CODE_T;

bool Measure_Start(MEASURE_T * measure, MEASURE_SAMPLE_T * sample);
bool Measure_StartADC(MEASURE_T * measure, MEASURE_CHANNEL_T channels, uint8_t channelCount, bool hwTrigger, void(*onEnd)(void), bool overwrite);

bool Measure_StartChannel					(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void));
bool Measure_StartChannelOverwrite			(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void));
bool Measure_TriggerChannel					(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void));
bool Measure_TriggerChannelOverwrite		(MEASURE_T * measure, uint8_t channel, void(*onEnd)(void));
bool Measure_StartChannelGroup				(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_StartChannelGroupOverwrite		(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_TriggerChannelGroup			(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_TriggerChannelGroupOverwrite	(MEASURE_T * measure, const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));


#endif
