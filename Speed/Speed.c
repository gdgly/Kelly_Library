#include "Speed.h"

/******************************************************************************/
/*!
 * @name  	Delta
 * @brief 	Find delta time between 2 time samples. Used for all speed calculations
 */
/******************************************************************************/
/*! @{ */

void Speed_InitDelta(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz)
{
	speed->TimerCounterValue = timerCounterValue;
	speed->TimerCounterMax = timerCounterMax;
	speed->TimerFreq = timerFreqHz;
}

/******************************************************************************/
/*!
 * @brief	Call from event interrupt to record time sample
 *
 * e.g. call each hall cycle / electric rotation inside hall edge interrupt
 * Interval cannot be greater than TimerCounterMax [ticks] i.e. (TimerFreq * TimerCounterMax) [seconds]
 */
/******************************************************************************/
inline void Speed_CaptureDeltaISR(SPEED_T * speed)
{
	if (*speed->TimerCounterValue < speed->TimerCounterValueSaved) // TimerCounter overflow
		speed->Delta = speed->TimerCounterMax - speed->TimerCounterValueSaved + *speed->TimerCounterValue;
	else
		speed->Delta = *speed->TimerCounterValue - speed->TimerCounterValueSaved;

	speed->TimerCounterValueSaved = *speed->TimerCounterValue;

	speed->DeltaCount++;
}

/******************************************************************************/
/*!
 * @brief	Poll from main loop if event interrupt is not available.
 *
 * e.g. give 1 hall edge for reference
 */
/******************************************************************************/
void Speed_CaptureDeltaPoll(SPEED_T * speed, bool reference)
{
	//if (reference - speed->ReferenceSignalSaved) // rising edge detect
	if (reference && !speed->ReferenceSignalSaved) // rising edge detect
		Speed_CaptureDeltaISR(speed);

	speed->ReferenceSignalSaved = reference;
}

//void Speed_LongDeltaPoll(SPEED_T * speed)
//{
//	if (speed->Delta == speed->DeltaSaved)
//	{
//		//return delta stoped, return 0 speed
//	}
//}

volatile uint32_t * Speed_GetPtrDelta(SPEED_T * speed)
{
	return &speed->Delta;
}

uint32_t Speed_GetDeltaTicks(SPEED_T * speed) // unit in timer ticks
{
	return speed->Delta;
}

uint32_t Speed_GetDeltaMillis(SPEED_T * speed) // unit in milliseconds
{
	return speed->Delta * 1000 / speed->TimerFreq;
}

uint32_t Speed_GetDeltaMicros(SPEED_T * speed) // unit in microseconds
{
	if (speed->Delta > (UINT32_MAX/1000000)) // overflows if DeltaPeriod > 4294
		return speed->Delta * (1000000 / speed->TimerFreq);
	else
		return speed->Delta * 1000000 / speed->TimerFreq;
}

uint32_t Speed_GetDeltaFreq(SPEED_T * speed) // unit in Hz
{
	if (speed->Delta == 0)	return 0;
	else					return speed->TimerFreq / speed->Delta;
}

uint32_t Speed_GetDeltaFreqCPM(SPEED_T * speed) // unit in cycles per minute
{
	if (speed->Delta == 0)	return 0;
	else					return speed->TimerFreq * 60 / speed->Delta;
}

//uint32_t Speed_ResetTimer(SPEED_T * speed)
//{
//	speed->TimerCounterValueSaved = *speed->TimerCounterValue;
//}

uint32_t Speed_GetDeltaCount(SPEED_T * speed)
{
	return speed->DeltaCount;
}

void Speed_ZeroDeltaCount(SPEED_T * speed)
{
	speed->DeltaCount = 0;
}
/*! @} */ // End of Delta submodule


/******************************************************************************/
/*!
 * @name  	Speed
 * @brief 	Encoder with known distance per tick
 */
/******************************************************************************/
/*! @{ */
void Speed_InitSpeed(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint32_t distancePerDeltaCycle)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);
	speed->DistancePerDeltaCycle = distancePerDeltaCycle;
	speed->DistanceTimerFreq = distancePerDeltaCycle * timerFreqHz; // possible 32bit overflow, max distancePerDelta ~14,000, for 300,000 Hz timer
}

/*!
 * Delta[s] 			= Delta[TimerTicks]/TimerFreq[Hz]
 * DistancePerDelta		= Distance/1[Delta]
 * Distance/UnitTime[s] = DistancePerDelta/Delta[s]
 * Distance/UnitTime[s] = DistancePerDelta*TimerFreq[Hz]/Delta[TimerTicks]
 */
uint32_t Speed_GetSpeed(SPEED_T * speed)
{
//	if (0xFFFFFFFF / speed->DisplacementPerDelta > speed->TimerFreq) // check for overflow
//	{
//		if(speed->TimerFreq > speed->MovementPerDelta) // pick more accurate
//			return speed->MovementPerDelta * (speed->TimerFreq / speed->DeltaPeriod);
//		else
//			return speed->TimerFreq * (speed->MovementPerDelta / speed->DeltaPeriod);
//	}
//	else
	return speed->DistanceTimerFreq / speed->Delta; // distancePerDeltaCycle * timerFreqHz / Delta
}

uint32_t Speed_GetDistance(SPEED_T * speed)
{
	return speed->DistancePerDeltaCycle * speed->DeltaCount;
}

uint32_t Speed_ConvertSpeedToDelta(SPEED_T * speed, uint32_t distancePerSecond)// Delta is in timer tick units
{
	if (distancePerSecond == 0)	return 0;
	return speed->DistanceTimerFreq / distancePerSecond; // distancePerDeltaCycle * timerFreqHz / distancePerSecond
}

uint32_t Speed_ConvertDeltaToSpeed(SPEED_T * speed, uint32_t delta)
{
	if (delta == 0)	return 0;
	return speed->DistanceTimerFreq / delta;
}
/*! @} */ // End of Speed submodule


/******************************************************************************/
/*!
 * @name  	Encoder
 * @brief 	Encoder defined in Pulse per Revolution and Distance Per Revolution
 */
/******************************************************************************/
/*! @{ */
void Speed_InitEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint32_t pulsePerRevolution, uint32_t distancePerRevolution)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);

	speed->PulsePerRevolution = pulsePerRevolution;
	speed->DistancePerRevolution = distancePerRevolution;
	speed->DistancePerDeltaCycle = distancePerRevolution/pulsePerRevolution; // only used for GetDistancePerPulse, use components for precisions in other cases

	if (distancePerRevolution > 0xFFFFFFFF / speed->TimerFreq) // if speed->TimerFreq * distancePerRevolution overflow
		speed->DistanceTimerFreq = distancePerRevolution * (speed->TimerFreq / pulsePerRevolution);
	else
		speed->DistanceTimerFreq = speed->TimerFreq * distancePerRevolution / pulsePerRevolution; // distancePerRevolution max input ~100,000

	speed->RevolutionsTimerFreq = speed->TimerFreq / pulsePerRevolution;
}

uint32_t Speed_GetDistancePerPulse(SPEED_T * speed)
{
	return speed->DistancePerDeltaCycle;
}

/*!
 * Delta[s] 		= Delta[TimerTicks]/TimerFreq[Hz]
 * Distance			= DistancePerRevolution[Distance/1]/PulsePerRevolution[EncoderTicks/1] = Distance/1[EncoderTick]
 * Distance/Time[s] = Distance/Delta[s]
 * Distance/Time[s] = Distance*TimerFreq[Hz]/Delta[TimerTicks]
 */
uint32_t Speed_GetLinearSpeed(SPEED_T * speed) // unit in distance per second
{
	if (speed->Delta == 0)	return 0;
	return speed->DistanceTimerFreq / speed->Delta; // (distancePerRevolution/pulsePerRevolution * speed->TimerFreq) / speed->DeltaPeriod;
}

/*!
 * Delta[s] 			= Delta[TimerTicks]/TimerFreq[Hz]
 * Revolutions			= 1[EncoderTicks]/PulsePerRevolution[EncoderTicks/1]
 * Revolutions/Time[s] 	= Revolutions/Delta[s]
 * Revolutions/Time[s] 	= TimerFreq[Hz]/PulsePerRevolution[count]/Delta[TimerTicks]
 */
uint32_t Speed_GetRotarySpeed(SPEED_T * speed) // Revolution per Second
{
	if (speed->Delta == 0)	return 0;
	return speed->RevolutionsTimerFreq / speed->Delta; // (speed->TimerFreq/pulsePerRevolution) / speed->DeltaPeriod;
}

uint32_t Speed_GetRotarySpeedRPM(SPEED_T * speed)
{
	if (speed->Delta == 0)	return 0;
	return speed->RevolutionsTimerFreq * 60 / speed->Delta;
}

uint32_t Speed_GetRotarySpeedRadiansPerSecond(SPEED_T * speed)
{
	if (speed->Delta == 0)	return 0;
	return speed->RevolutionsTimerFreq * (2*314) / speed->Delta / 100;
}

uint32_t Speed_GetRotarySpeedRadiansPerSecond10(SPEED_T * speed)
{

}

uint32_t Speed_GetRotarySpeedDegreesPerSecond(SPEED_T * speed)
{
	if (speed->Delta == 0)	return 0;
	return speed->RevolutionsTimerFreq * 360 / speed->Delta;
}

uint32_t Speed_GetRotarySpeedDegreesPerSecond10(SPEED_T * speed)
{

}

uint32_t Speed_GetLinearDistance(SPEED_T * speed)
{
	if (0xFFFFFFFF/speed->DeltaCount > speed->DistancePerRevolution)
		return speed->DistancePerRevolution*(speed->DeltaCount/speed->PulsePerRevolution);
	else
		return speed->DistancePerRevolution*speed->DeltaCount/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRevolutions(SPEED_T * speed)
{
	return speed->DeltaCount/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRevolutions10(SPEED_T * speed, uint8_t precision)
{
	if (0xFFFFFFFF / speed->DeltaCount > 10^precision)
		return (10^precision)*(speed->DeltaCount/speed->PulsePerRevolution);
	else
		return speed->DeltaCount*(10^precision)/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationRadians(SPEED_T * speed)
{
	if (0xFFFFFFFF/speed->DeltaCount > (2*314))
		return (2*314)*(speed->DeltaCount/speed->PulsePerRevolution)/100;
	else
		return speed->DeltaCount*(2*314)/speed->PulsePerRevolution/100;
}

uint32_t Speed_GetRotationRadians10(SPEED_T * speed, uint8_t precision)
{

}

uint32_t Speed_GetRotationDegrees(SPEED_T * speed)
{
	if (0xFFFFFFFF/speed->DeltaCount > 360)
		return 360*(speed->DeltaCount/speed->PulsePerRevolution);
	else
		return speed->DeltaCount*360/speed->PulsePerRevolution;
}

uint32_t Speed_GetRotationDegrees10(SPEED_T * speed)
{

}

uint32_t Speed_ConvertRPMToDelta(SPEED_T * speed, uint32_t rpm) // HallPeriod unit in timer ticks
{
	if (rpm == 0)	return 0;
	return speed->RevolutionsTimerFreq *60 / rpm; //timerFreqHz * 60 / nPolePairs / rpm;
}

uint32_t Speed_ConvertDeltaToRPM(SPEED_T * speed, uint32_t peroid) // HallPeriod unit in timer ticks
{
	if (peroid == 0)	return 0;
	return speed->RevolutionsTimerFreq *60 / peroid;
}

/*! @} */

/******************************************************************************/
/*!
 * @name  	QuadratureEncoder
 * @brief 	Quadrature Encoder
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

int32_t Speed_GetLinearDisplacement(SPEED_T * speed) // treat displacement as signed scalar quantity
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
 * @name  	HallEncoder
 * @brief 	BLDC speed functions using 1 hall sensor as encoder.
 *			Delta = 1 hall cycle = 1 electronic revolution.
 *			PulsePerRevolution = PolePairs
 *
 * MECH_R = ELECTRIC_R / N_POLE_PAIRS
 * ELECTRIC_R = 6 COMMUTATION_STEPS
 * 1 POLE_PAIRS -> 6 COMMUTATION_STEPS = ELECTRIC_R = MECH_R
 * 2 POLE_PAIRS -> 12 COMMUTATION_STEPS = 2 ELECTRIC_R = MECH_R
 */
/******************************************************************************/
/*! @{ */

//void Speed_InitHallEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint8_t nPolePairs, uint16_t distancePerRevolution, uint32_t pwmFreqHz)
//{
//
//}

/*!
 * Delta[s] 	= Delta[TimerTicks]/TimerFreq[Hz]
 * ERPM			= 1[Delta]*60[s]/Delta[s]
 * RPM 			= ERPM/PolePairs
 * RPM 			= 1[Delta]*60[s]*TimerFreq[Hz]/PolePairs/Delta[TimerTicks]
 */
void Speed_InitHallEncoder(SPEED_T * speed, uint32_t * timerCounterValue, uint32_t timerCounterMax, uint32_t timerFreqHz, uint8_t nPolePairs, uint32_t pwmFreqHz)
{
	Speed_InitDelta(speed, timerCounterValue, timerCounterMax, timerFreqHz);

	speed->RevolutionsTimerFreq = timerFreqHz / nPolePairs;
	speed->TimePerPWM = timerFreqHz / pwmFreqHz;
}

uint32_t Speed_GetRPS(SPEED_T * speed)
{
	return Speed_GetRotarySpeed(speed); // timerFreqHz / nPolePairs / speed->Delta;
}

uint32_t Speed_GetRPM(SPEED_T * speed)
{
	return Speed_GetRotarySpeedRPM(speed); // timerFreqHz * 60 / nPolePairs / speed->Delta;
}

uint32_t Speed_GetERPM(SPEED_T * speed)
{
	return Speed_GetDeltaFreqCPM(speed); //speed->TimerFreq * 60 / speed->Delta;
}

uint32_t Speed_GetHallPeroid(SPEED_T * speed)
{
	return Speed_GetDelta(speed);
}

uint32_t Speed_GetPWMCyclesPerHallCycle(SPEED_T * speed)
{
	return speed->Delta / speed->TimePerPWM; // speed->Delta / (timerFreqHz / pwmFreqHz);
}

uint32_t Speed_GetPWMCyclesPerCommutationStep(SPEED_T * speed)
{
	return speed->Delta / speed->TimePerPWM / 6;

}

uint32_t Speed_ConvertRPMToHallPeriod(SPEED_T * speed, uint32_t rpm) // HallPeriod unit in timer ticks
{
	if (rpm == 0) return 0;
	return speed->RevolutionsTimerFreq * 60 / rpm ; //timerFreqHz * 60 / nPolePairs / rpm;
}

uint32_t Speed_ConvertHallPeroidToRPM(SPEED_T * speed, uint32_t ticks) // HallPeriod unit in timer ticks
{
	if (ticks == 0) return 0;
	return speed->RevolutionsTimerFreq * 60 / ticks;
}

/*! @} */


uint32_t Speed_GetRoadSpeed(SPEED_T * speed, uint32_t wheelRatio, uint32_t motorRatio)
{

}

