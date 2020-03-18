#ifndef MEASURE_H_
#define MEASURE_H_

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
	MEASURE_CHANNEL_T SampleGroupChannelSelect;
	uint8_t SampleGroupChannelCount;
	bool ADC_HWTrigger;
	void(*ADC_OnEndISR)(void);
} MEASURE_T;

bool Measure_StartADC(MEASURE_CHANNEL_T channels, uint8_t channelCount, bool triggerMode, void(*onEnd)(void), bool overwrite);
//bool Measure_SetMeasureAdaptive(volatile MEASURE_CHANNEL_T channels, uint8_t channelCount, bool triggerMode, void(*onEnd)(void));

bool Measure_TriggerMeasureChannel(volatile uint8_t channel, void(*onEnd)(void));
bool Measure_StartMeasureChannel(volatile uint8_t channel, void(*onEnd)(void));
bool Measure_TriggerMeasureSampleGroup(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_StartMeasureSampleGroup(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_TriggerMeasureChannelOverwrite(volatile uint8_t channel, void(*onEnd)(void));
bool Measure_StartMeasureChannelOverwrite(volatile uint8_t channel, void(*onEnd)(void));
bool Measure_TriggerMeasureSampleGroupOverwrite(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));
bool Measure_StartMeasureSampleGroupOverwrite(const uint8_t * channels, uint8_t channelCount, void(*onEnd)(void));

void Measure_WaitForResult();

bool Measure_SaveValue(volatile ADC_DATA_T * value, uint8_t channel);
bool Measure_MapAddress(volatile ADC_DATA_T ** address, uint8_t channel);
volatile ADC_DATA_T Measure_GetValue(uint8_t channel);
volatile ADC_DATA_T * Measure_GetAddress(uint8_t channel);
//void Measure_SetOnEndISR(void (*fp)(void));

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
);


#endif
