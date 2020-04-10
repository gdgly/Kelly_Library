#include "Speed.h"

void Speed_InitDelta(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz)
{
	speed->TimerCounterValue = timerCounterValue;
	speed->TimerCounterMax = timerCounterMax;
	speed->TimerFreq = timerFreqHz;
}

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


void Speed_CaptureDeltaPoll(SPEED_T * speed, bool reference) //e.g. give 1 hall edge for reference
{
	//if (reference - speed->ReferenceSignalSaved) // rising edge detect
	if (reference && !speed->ReferenceSignalSaved) // rising edge detect
		Speed_CaptureDeltaISR(speed);

	speed->ReferenceSignalSaved = reference;
}

uint32_t * Speed_GetPtrDeltaPeriod(SPEED_T * speed)
{
	return &speed->DeltaPeriod;
}

uint32_t Speed_GetDeltaPeriodRaw(SPEED_T * speed) // unit in timer ticks
{
	return speed->DeltaPeriod;
}

uint32_t Speed_GetDeltaPeriodMillis(SPEED_T * speed) // unit in milliseconds
{
	return speed->DeltaPeriod * 1000 / speed->TimerFreq;
}

uint32_t Speed_GetDeltaPeriodMicros(SPEED_T * speed) // unit in microseconds
{
	if (speed->DeltaPeriod > (UINT32_MAX/1000000)) //overflows if DeltaPeriod > 4294
		return speed->DeltaPeriod * (1000000 / speed->TimerFreq);
	else
		return speed->DeltaPeriod * 1000000 / speed->TimerFreq;
}

uint32_t Speed_GetDeltaFreq(SPEED_T * speed) // unit in Hz
{
	if (speed->DeltaPeriod == 0)	return 0;
	else							return speed->TimerFreq / speed->DeltaPeriod;
}

//uint32_t Speed_ResetTimer(ENCODER_T * speed)
//{
//	speed->TimerCounterValueSaved = *speed->TimerCounterValue;
//	speed->TimerCounterValueSaved
//}

uint32_t Speed_GetDeltaCount(SPEED_T * speed)
{
	return speed->DeltaCount;
}

uint32_t Speed_GetElaspedTime(SPEED_T * speed) // unit in seconds
{
	return speed->DeltaCount * speed->DeltaPeriod;
}

void Speed_ZeroDeltaCount(SPEED_T * speed)
{
	speed->DeltaCount = 0;
}

void Speed_InitSpeed(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint32_t movementPerDelta)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);
	speed->MovementPerDelta = movementPerDelta;
	speed->MovementTimerFreq = movementPerDelta * timerFreqHz;
}

uint32_t Speed_GetSpeed(SPEED_T * speed)
{
//	if (0xFFFFFFFF / speed->MovementPerDelta > speed->TimerFreq) //check for overflow
//	{
//		if(speed->TimerFreq > speed->MovementPerDelta) // pick more accurate
//			return speed->MovementPerDelta * (speed->TimerFreq / speed->DeltaPeriod);
//		else
//			return speed->TimerFreq * (speed->MovementPerDelta / speed->DeltaPeriod);
//	}
//	else
	return speed->MovementTimerFreq / speed->DeltaPeriod;
}

uint32_t Speed_GetMovement(SPEED_T * speed)
{
	return speed->MovementPerDelta * speed->DeltaCount;
}


/******************************************************************************/
/*!
 * @name  	EncoderSpeed
 * @brief 	Encoder speed
 */
/******************************************************************************/
/*! @{ */
void Speed_InitEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint32_t pulsePerRevolution, uint32_t distancePerRevolution)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);

	speed->PulsePerRevolution = pulsePerRevolution;
	speed->DistancePerRevolution = distancePerRevolution;
	speed->MovementPerDelta = distancePerRevolution/pulsePerRevolution;
	speed->RevolutionsTimerFreq = speed->TimerFreq / pulsePerRevolution;
	if (distancePerRevolution > 0xFFFFFFFF / speed->TimerFreq) //if speed->TimerFreq * distancePerRevolution overflow
		speed->DistanceTimerFreq = (distancePerRevolution * (speed->TimerFreq / pulsePerRevolution));
	else
		speed->DistanceTimerFreq = speed->TimerFreq * distancePerRevolution / pulsePerRevolution; // distancePerRevolution max input ~100,000
}

uint32_t Speed_GetDistancePerPulse(SPEED_T * speed)
{
	return speed->MovementPerDelta;
}

/*!
 * DeltaPeroid[s] 	= DeltaPeroid[TimerTicks]/TimerFreq[Hz]
 * Distance			= DistancePerRevolution[Distance/1]/PulsePerRevolution[EncoderTicks/1] = Distance/1[EncoderTick]
 * Distance/Time[s] = Distance/DeltaPeroid[s]
 * Distance/Time[s] = Distance*TimerFreq[Hz]/DeltaPeroid[TimerTicks]
 */
uint32_t Speed_GetLinearSpeed(SPEED_T * speed) // unit in distance per second
{
	return speed->DistanceTimerFreq / speed->DeltaPeriod; // (distancePerRevolution/pulsePerRevolution * speed->TimerFreq) / speed->DeltaPeriod;
}

/*!
 * DeltaPeroid[s] 		= DeltaPeroid[TimerTicks]/TimerFreq[Hz]
 * Revolutions			= 1[EncoderTicks]/PulsePerRevolution[EncoderTicks/1]
 * Revolutions/Time[s] 	= Revolutions/DeltaPeroid[s]
 * Revolutions/Time[s] 	= TimerFreq[Hz]/PulsePerRevolution[count]/DeltaPeroid[TimerTicks]
 */
uint32_t Speed_GetRotarySpeed(SPEED_T * speed) //*RPS
{
	return speed->RevolutionsTimerFreq / speed->DeltaPeriod; // (speed->TimerFreq/pulsePerRevolution) / speed->DeltaPeriod;
}

uint32_t Speed_GetRotarySpeedRPM(SPEED_T * speed)
{
	return speed->RevolutionsTimerFreq * 60 / speed->DeltaPeriod;
}

uint32_t Speed_GetRotarySpeedRadPerSecond(SPEED_T * speed) //*RPS
{
	return speed->RevolutionsTimerFreq * (2*314) / speed->DeltaPeriod / 100;
}

uint32_t Speed_GetRotarySpeedDegreesPerSecond(SPEED_T * speed) //*RPS
{
	return speed->RevolutionsTimerFreq * 360 / speed->DeltaPeriod;
}

uint32_t Speed_GetDistance(SPEED_T * speed)
{
//	if (0xFFFFFFFF/speed->DeltaCount > speed->DistancePerRevolution)
//		return speed->DistancePerRevolution*(speed->DeltaCount/speed->PulsePerRevolution);
//	else
	return speed->DistancePerRevolution*speed->DeltaCount/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRevolutions(SPEED_T * speed)
{
	return speed->DeltaCount/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRevolutions10(SPEED_T * speed, uint8_t precision)
{
//	if (0xFFFFFFFF/speed->DeltaCount > (10^precision))
//		return (10^precision)*(speed->DeltaCount/speed->PulsePerRevolution);
//	else
	return speed->DeltaCount*(10^precision)/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRadians(SPEED_T * speed)
{
//	if (0xFFFFFFFF/speed->DeltaCount > (2*314))
//		return (2*314)*(speed->DeltaCount/speed->PulsePerRevolution)/100;
//	else
	return speed->DeltaCount*(2*314)/speed->PulsePerRevolution/100;
}

uint32_t Speed_GetRotationDegrees(SPEED_T * speed)
{
//	if (0xFFFFFFFF/speed->DeltaCount > 360)
//		return 360*(speed->DeltaCount/speed->PulsePerRevolution);
//	else
	return speed->DeltaCount*360/speed->PulsePerRevolution;
}
/*! @} */

/******************************************************************************/
/*!
 * @name  	QuadratureEncoderSpeed
 * @brief 	Quadrature Encoder speed
 */
/******************************************************************************/
/*! @{ */
void Speed_InitQuadratureEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint32_t pulsePerRevolution, uint32_t distancePerRevolution, bool isPositiveALeadB)
{
	Speed_InitEncoder(speed, timerCounterValue, timerCounterMax, timerFreqHz, pulsePerRevolution, distancePerRevolution);
	speed->ALeadB = isPositiveALeadB;
}

inline bool Speed_GetDirection(SPEED_T * speed)
{
	if(speed->ALeadB ^ speed->GetPhaseBSignal())	return false;
	else											return true;
}

int32_t Speed_GetLinearDisplacement(SPEED_T * speed)
{

}

int32_t Speed_GetRotaryDisplacement(SPEED_T * speed)
{

}

int32_t Speed_GetLinearVelocity(SPEED_T * speed)
{
	if(Speed_GetDirection(speed))	return (int32_t)Speed_GetLinearSpeed(speed);
	else							return (int32_t)0 - (int32_t)Speed_GetLinearSpeed(speed);
}

int32_t Speed_GetRotaryVelocity(SPEED_T * speed)
{
	if(Speed_GetDirection(speed))	return (int32_t)Speed_GetRotarySpeed(speed);
	else							return (int32_t)0 - (int32_t)Speed_GetRotarySpeed(speed);
}
/*! @} */


/******************************************************************************/
/*!
 * @name  	BLDCSpeed
 * @brief 	BLDC Speed Functions using hall sensor as encoder
 *
 * ERPM = RPM * N_POLE_PAIRS
 * MECH_R = ELECTRIC_R / N_POLE_PAIRS
 * ELECTRIC_R = 6 COMMUTATION_STEPS
 * 1 POLE_PAIR -> 6 COMMUTATION_STEPS = ELECTRIC_R
 */
/******************************************************************************/
/*! @{ */
void Speed_InitBLDC(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint8_t polePairs, uint32_t pwmFreqHz)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);
	speed->RPMTime = timerFreqHz * 60 / polePairs; // Calibrated for 1 hall cycle = 1 electronic rotation
	speed->TimePerPWM = timerFreqHz / pwmFreqHz;
}

uint32_t Speed_GetRPM(SPEED_T * speed)
{
	if (speed->DeltaPeriod == 0)	return 0;
	else							return speed->RPMTime / speed->DeltaPeriod; // timerFreqHz * 60 / polePairs / speed->DeltaPeriod;
}

uint32_t Speed_GetERPM(SPEED_T * speed)
{
	if (speed->DeltaPeriod == 0)	return 0;
	else							return speed->TimerFreq * 60 / speed->DeltaPeriod;
}

uint32_t Speed_ConvertRPMToDeltaPeriod(SPEED_T * speed, uint32_t rpm) // unit in timer ticks
{
	return speed->RPMTime / rpm;
}

uint32_t Speed_ConvertDeltaPeroidToRPM(SPEED_T * speed, uint32_t peroid) // unit in timer ticks
{
	if (peroid == 0)	return 0;
	else				return speed->RPMTime / peroid;
}

/*! @brief PWM cycles in 1 hall cycle */
uint32_t Speed_GetPWMPeriodCount(SPEED_T * speed)
{
	return speed->DeltaPeriod / speed->TimePerPWM; // speed->DeltaPeriod / (timerFreqHz / pwmFreqHz);
}

//void Speed_SetCalcRPM(SPEED_T * speed, uint8_t polePairs)
//{
//	speed->RPMTime = speed->TimerFreq * 60 / polePairs;
//}
//
//void Speed_SetCalcPWMPeriodCount(SPEED_T * speed, uint32_t pwmFreqHz)
//{
//	speed->TimePerPWM = speed->TimerFreq / pwmFreqHz;
//}
/*! @} */
