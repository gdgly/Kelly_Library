/*
 * UserApp.c
 *
 *  Created on: Aug 22, 2020
 *      Author: FireSourcery
 */


MEASURE_T MeasureADC0;

#define MEASURE_ADC_CHANNEL_COUNT			8U

//	Virtual/Measure Channel, index into arrays
#define MEASURE_CHANNEL_BAT_AD		0U
#define MEASURE_CHANNEL_I1_AD		1U
#define MEASURE_CHANNEL_I2_AD		2U
#define MEASURE_CHANNEL_V1A			3U
#define MEASURE_CHANNEL_V1B			4U
#define MEASURE_CHANNEL_V2A			5U
#define MEASURE_CHANNEL_V2B			6U
#define MEASURE_CHANNEL_LSTEMP_AD	7U

uint8_t Measure_ChannelResultBuffer[MEASURE_ADC_CHANNEL_COUNT];
uint8_t Measure_ChannelSumBuffer[0]; //[MEASURE_ADC_CHANNEL_COUNT];

const uint8_t DummyPhaseCBuffer = 0;

// ADC Pins
#define ADC_CHANNEL_PIN_BAT_AD		ADC_PDD_SINGLE_ENDED_DAD0
#define ADC_CHANNEL_PIN_I1_AD		ADC_PDD_SINGLE_ENDED_AD4
#define ADC_CHANNEL_PIN_I2_AD		ADC_PDD_SINGLE_ENDED_AD5
#define ADC_CHANNEL_PIN_V1A_AD 		ADC_PDD_SINGLE_ENDED_DAD1
#define ADC_CHANNEL_PIN_V1B_AD 		ADC_PDD_SINGLE_ENDED_DAD2
#define ADC_CHANNEL_PIN_V2A_AD 		ADC_PDD_SINGLE_ENDED_AD13
#define ADC_CHANNEL_PIN_V2B_AD 		ADC_PDD_SINGLE_ENDED_AD14
#define ADC_CHANNEL_PIN_LSTEMP_AD 	ADC_PDD_SINGLE_ENDED_AD15

const uint8_t MEASURE_CHANNEL_TO_ADC_PIN[MEASURE_ADC_CHANNEL_COUNT] =
{
	ADC_CHANNEL_PIN_BAT_AD,
	ADC_CHANNEL_PIN_I1_AD,
	ADC_CHANNEL_PIN_I2_AD,
	ADC_CHANNEL_PIN_V1A_AD,
	ADC_CHANNEL_PIN_V1B_AD,
	ADC_CHANNEL_PIN_V2A_AD,
	ADC_CHANNEL_PIN_V2B_AD,
	ADC_CHANNEL_PIN_LSTEMP_AD
};

uint8_t ADC_SampleChannelsBuffer[ADC_MAX_HW_SAMPLE_COUNT]; // array of pin channels pushed to adc fifo

// runs after all items in fifo are processed.
// only 1 channel in fifo is processed every hw trigger.
// entire fifo is processed using software trigger.
void ADC_ISR(void)
{
	Measure_CompleteISR(&MeasureADC0);
}
