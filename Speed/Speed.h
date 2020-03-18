#ifndef _SPEED_H_
#define _SPEED_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
	volatile uint32_t * TimerCounterValue;
    uint32_t TimerCounterMax; 					/*!< For overflow cases */
    uint32_t TimerFreq;							/*!< For event period in standard time units */
    volatile uint32_t DeltaPeriod;				/*!< time tick units */
    volatile uint32_t TimerCounterValueSaved;
    //bool (*GetReferenceSignal)();				/*!< bit signal will not have individually addressable memory */
	bool ReferenceSignalSaved;					/*!< Used for polling */
	volatile uint32_t DeltaCount;				/*!< Track number of deltas, used in extensions */
	//need data to determine timer validity? invalid for first time sample. invalid for time > than 1 timer cycle.

	uint32_t MovementPerDelta;
	uint32_t MovementTimerFreq;					/*!< (Movement * TimerFreq) */

	uint16_t DistancePerRevolution;
	uint16_t PulsePerRevolution;
	uint32_t DistanceTimerFreq; 		/*!< (DistancePerRevolution*TimerFreq/PulsePerRevolution), use for GetLinearSpeed */
	uint32_t RevolutionsTimerFreq; 		/*!< (TimerFreq/PulsePerRevolution), use for GetRotarySpeed */
	bool ALeadB;
	bool (*GetPhaseBSignal)(void);

    uint32_t TimePerPWM; 	/*!< Time in time tick units */
	uint32_t RPMTime;		/*!< Time in time tick units */
}
SPEED_T;

// Call each hall cycle / electric rotation, e.g. hall rising edge interrupt
// Interval cannot be greater than TimerCounterMax [ticks] i.e. TimerFreq * TimerCounterMax [seconds]
inline void Speed_CaptureDeltaISR(SPEED_T * speed)
{
	if (*speed->TimerCounterValue < speed->TimerCounterValueSaved) // TimerCounter overflow
		speed->DeltaPeriod = speed->TimerCounterMax - speed->TimerCounterValueSaved + *speed->TimerCounterValue;
	else
		speed->DeltaPeriod = *speed->TimerCounterValue - speed->TimerCounterValueSaved;

	speed->TimerCounterValueSaved = *speed->TimerCounterValue;

	speed->DeltaCount++;
}

void Speed_CaptureDeltaPoll(SPEED_T * speed, bool reference);
uint32_t * Speed_GetPtrDeltaPeriod(SPEED_T * speed);
uint32_t Speed_GetDeltaPeriodRaw(SPEED_T * speed);
uint32_t Speed_GetDeltaPeriodMillis(SPEED_T * speed);
uint32_t Speed_GetDeltaPeriodMicros(SPEED_T * speed);
uint32_t Speed_GetDeltaFreq(SPEED_T * speed);
//uint32_t Speed_ResetTimer(ENCODER_T * speed);
void Speed_Init(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz);


/*!
 * @addtogroup  BLDCSpeed
 * @brief 		BLDC functions declarations
 */
/*! @{ */
void Speed_InitBLDC(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint8_t polePairs,	uint32_t pwmFreq);
uint32_t Speed_GetRPM(SPEED_T * speed);
uint32_t Speed_GetERPM(SPEED_T * speed);
uint32_t Speed_GetPWMPeriodCount(SPEED_T * speed);
uint32_t Speed_ConvertRPMToDeltaPeriod(SPEED_T * speed, uint32_t rpm);
uint32_t Speed_ConvertDeltaPeroidToRPM(SPEED_T * speed, uint32_t peroid);
//void Speed_SetCalcPWMPeriodCount(SPEED_T * speed, uint32_t pwmFreqHz);
//void Speed_SetCalcRPM(SPEED_T * speed, uint8_t polePairs);
/*! @} */

#endif
