#ifndef _SPEED_H_
#define _SPEED_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
	volatile uint32_t * TimerCounterValue;		/*!< Initialize with a timer counter register */
    volatile uint32_t TimerCounterValueSaved;	/*!< 2nd time sample used to calculate delta */
    volatile uint32_t Delta;					/*!< Timer ticks between 2 events. Timer tick units */
    uint32_t TimerCounterMax; 					/*!< For overflow cases */
    uint32_t TimerFreq;							/*!< For delta period in standard time units */
    //bool (*GetReferenceSignal)();				/*!< bit signal will not have individually addressable memory */
	bool ReferenceSignalSaved;					/*!< Saved reference signal, used in case of polling time samples */
	volatile uint32_t DeltaCount;				/*!< Track number of deltas, used in extensions */
	//need data to determine timer validity? invalid for first time sample. invalid for time > than 1 timer cycle.

	uint32_t DistancePerDeltaCycle;			/*!< User defined */
	uint32_t DistanceTimerFreq;			/*!< (DistancePerDelta * TimerFreq) */

	uint16_t PulsePerRevolution;		/*!< User defined */
	uint16_t DistancePerRevolution;		/*!< User defined */
	//uint32_t DistancePerDelta;		/*!< DistancePerRevolution/PulsePerRevolution, used for Encoder GetDistancePerPulse */
	//uint32_t DistanceTimerFreq; 		/*!< TimerFreq*DistancePerRevolution/PulsePerRevolution, used for GetLinearSpeed */
	uint32_t RevolutionsTimerFreq; 		/*!< TimerFreq/PulsePerRevolution, used for GetRotarySpeed */

	bool ALeadB;
	bool (*GetPhaseBSignal)(void);

	//uint32_t RevolutionsTimerFreq; 		/*!< TimerFreq/PolePairs */
    uint32_t TimePerPWM; 					/*!< TimerFreq/PWMFreq. Time in time tick units */
}
SPEED_T;


extern void Speed_CaptureDeltaISR(SPEED_T * speed);

void Speed_CaptureDeltaPoll(SPEED_T * speed, bool reference);
uint32_t * Speed_GetPtrDelta(SPEED_T * speed);
uint32_t Speed_GetDeltaRaw(SPEED_T * speed);
uint32_t Speed_GetDeltaMillis(SPEED_T * speed);
uint32_t Speed_GetDeltaMicros(SPEED_T * speed);
uint32_t Speed_GetDeltaFreq(SPEED_T * speed);
//uint32_t Speed_ResetTimer(ENCODER_T * speed);
void Speed_Init(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz);


/*!
 * @addtogroup  BLDCSpeed
 * @brief 		BLDC functions declarations
 */
/*! @{ */
void Speed_InitHallEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint8_t polePairs,	uint32_t pwmFreq);
uint32_t Speed_GetRPM(SPEED_T * speed);
uint32_t Speed_GetERPM(SPEED_T * speed);
uint32_t Speed_GetPWMPeriodCount(SPEED_T * speed);
uint32_t Speed_ConvertRPMToHallPeriod(SPEED_T * speed, uint32_t rpm);
uint32_t Speed_ConvertHallPeroidToRPM(SPEED_T * speed, uint32_t peroid);
//void Speed_SetCalcPWMPeriodCount(SPEED_T * speed, uint32_t pwmFreqHz);
//void Speed_SetCalcRPM(SPEED_T * speed, uint8_t polePairs);
/*! @} */

#endif
