/*
 * PID.c
 *
 *  Created on: Nov 26, 2019
 *      Author: FireSourcery
 */
#include "PID.h"

#include <stdbool.h>
#include <stdint.h>

// standard PID calculation
// Must be called in intervals of designated sample time
bool Compute(PID_T * pid)
{
	int32_t error = (int32_t)*pid->SetPoint - (int32_t)*pid->Input;
	int32_t output = 0;

	int32_t proportional, intergral, derivative;



//	if 		(pid->IntergralSum > pid->OutMax) pid->IntergralSum = pid->OutMax;
//	else if (pid->IntergralSum < pid->OutMin) pid->IntergralSum = pid->OutMin;

	//	if 		((pid->KiFactor * pid->IntergralSum * pid->SampleTime / pid->TimerFreq / pid->KiDivisor) > pid->OutMax) pid->IntergralSum = pid->OutMax;

	output = (pid->KpFactor * error / pid->KpDivisor) + 50;

//	proportional = (pid->KpFactor * error / pid->KpDivisor);
//	if 		(proportional > pid->OutMax) proportional = pid->OutMax;
//	else if (proportional < pid->OutMin) proportional = pid->OutMin;

	//if (pid->IntergralSum * pid->SampleTime / pid->TimerFreq == 0)
//	pid->IntergralSum += error;
//	output += 	(pid->KiFactor * pid->IntergralSum * pid->SampleTime / pid->TimerFreq / pid->KiDivisor);
	//else
	//pid->IntergralSum += pid->IntergralSum * pid->SampleTime / pid->TimerFreq;
	//output =+ 	(pid->KiFactor * pid->IntergralSum / pid->KiDivisor);

//	if (pid->KdFactor * error > UINT32_MAX/pid->TimerFreq)
//		output -= (pid->KdFactor * error * (pid->TimerFreq / pid->SampleTime) / pid->KdDivisor);
//	else
//		output -= (pid->KdFactor * error * pid->TimerFreq / pid->SampleTime / pid->KdDivisor);

	if 		(output > pid->OutMax) output = pid->OutMax;
	else if (output < pid->OutMin) output = pid->OutMin;

	*pid->Output = output;

	return true;
}

// experimental calculation from arduino library
bool ComputeProportionalOnInput(PID_T * pid)
{
//	int32_t error = *pid->SetPoint - *pid->Input;
//	int32_t dInput = *pid->Input - pid->LastInput;
//	uint32_t output = 0;
//
//	pid->LastInput = *pid->Input;
//
//	pid->IntergralSum += ((pid->Ki * error) - (pid->Kp * dInput));
//
//	if 		(pid->IntergralSum > pid->OutMax)	pid->IntergralSum = pid->OutMax;
//	else if (pid->IntergralSum < pid->OutMin)	pid->IntergralSum = pid->OutMin;
//
//	output = (pid->IntergralSum * pid->SampleTime / pid->TimerFreq) - (pid->Kd * dInput * pid->TimerFreq / pid->SampleTime);
//
//	if 		(output > pid->OutMax)	output = pid->OutMax;
//	else if (output < pid->OutMin)	output = pid->OutMin;
//
//	*pid->Output = output;
//
//	return true;
}

bool PID_ComputeTimerISR(PID_T * pid)
{
    if (pid->ComputeCounter < pid->ComputeInterval - 1)
    {
    	pid->ComputeCounter++;
    	return false;
    }
    else
    {
		pid->ComputeCounter = 0;
		if (pid->ComputeMode == PID_P_ON_E) return Compute(pid);
		else								return ComputeProportionalOnInput(pid);
	}
}

void PID_SetTunings(PID_T * pid, int32_t KpFactor, int32_t KpDivisor, int32_t KiFactor, int32_t KiDivisor, int32_t KdFactor, int32_t KdDivisor)
{
	pid->KpFactor 	= KpFactor;
	pid->KpDivisor 	= KpDivisor;
	pid->KiFactor 	= KiFactor;
	pid->KiDivisor 	= KiDivisor;
	pid->KdFactor 	= KdFactor;
	pid->KdDivisor 	= KdDivisor;

	if (pid->ControllerDirection == PID_DIRECTION_REVERSE)
	{
		pid->KpFactor 	= 0 - KpFactor;
		pid->KiFactor 	= 0 - KiFactor;
		pid->KdFactor 	= 0 - KdFactor;
	}
}

void PID_SetSampleTime(PID_T * pid, uint32_t sampleTime)
{
   if (sampleTime > 0)
   {
      pid->SampleTime = sampleTime;
   }
}

void PID_SetOutputLimits(PID_T * pid, uint32_t Min, uint32_t Max)
{
   if(Min >= Max) return;

   pid->OutMin = Min;
   pid->OutMax = Max;

   if		(*pid->Output > pid->OutMax) 		*pid->Output = pid->OutMax;
   else if	(*pid->Output < pid->OutMin) 		*pid->Output = pid->OutMin;

   if		(pid->IntergralSum > pid->OutMax) 	pid->IntergralSum = pid->OutMax;
   else if	(pid->IntergralSum < pid->OutMin) 	pid->IntergralSum = pid->OutMin;
}

void PID_SetProportionMode(PID_T * pid, PID_COMPUTE_MODE_T mode)
{
    pid->ComputeMode = mode;
}

void PID_SetControllerDirection(PID_T * pid, PID_DIRECTION_T Direction)
{
	if ((Direction != pid->ControllerDirection))
	{
		pid->KpFactor = 0 - pid->KpFactor;
		pid->KiFactor = 0 - pid->KiFactor;
		pid->KdFactor = 0 - pid->KdFactor;
	}
	pid->ControllerDirection = Direction;
}

//int32_t PID_GetKp(PID_T * pid) {return  pid->Kp;}
//int32_t PID_GetKi(PID_T * pid) {return  pid->Ki;}
//int32_t PID_GetKd(PID_T * pid) {return  pid->Kd;}
PID_DIRECTION_T PID_GetDirection(PID_T * pid) 	{return pid->ControllerDirection;}

void PID_Init
(
	PID_T * pid,
	uint32_t * input, uint32_t * output, uint32_t * setPoint,
	int32_t KpFactor, int32_t KpDivisor,
	int32_t KiFactor, int32_t KiDivisor,
	int32_t KdFactor, int32_t KdDivisor,
	uint32_t timerFreq, uint32_t sampleISRPeriodTicks, uint32_t sampleISRCycles,
	uint32_t outMin, uint32_t outMax
)
{
	pid->Output = output;
	pid->Input = input;
	pid->SetPoint = setPoint;
	pid->ControllerDirection = PID_DIRECTION_DIRECT;

	PID_SetTunings(pid, KpFactor, KpDivisor, KiFactor, KiDivisor, KdFactor,	KdDivisor);
	pid->ComputeMode = PID_P_ON_E;

	pid->ComputeInterval = sampleISRCycles;
	pid->SampleTime = sampleISRPeriodTicks * sampleISRCycles;
	pid->TimerFreq = timerFreq;

    PID_SetOutputLimits(pid, outMin, outMax);
	pid->IntergralSum = *pid->Output;
	pid->LastInput = *pid->Input;
}

volatile static uint32_t * Millis;

bool PID_ComputeMillisModePoll(PID_T * pid)
{
	if (*Millis - pid->LastTime > pid->SampleTime)
	{
		pid->LastTime = Millis;
		if (pid->ComputeMode == PID_P_ON_E)		return Compute(pid);
		else									return ComputeProportionalOnInput(pid);
	}

	return false;
}

void PID_InitMillisMode
(
	PID_T * pid,
	uint32_t * input, uint32_t * output, uint32_t * setpoint,
	uint32_t Kp, uint32_t Ki, uint32_t Kd,
	uint32_t *millisCounter, uint32_t sampleTimeMs,
	uint32_t outMin, uint32_t outMax
)
{
	pid->Output = output;
	pid->Input = input;
	pid->SetPoint = setpoint;
	pid->ControllerDirection = PID_DIRECTION_DIRECT;

    //PID_SetTunings(pid, Kp, Ki, Kd);
	pid->ComputeMode = PID_P_ON_E;

	Millis = millisCounter;
	pid->TimerFreq = 1000;
	pid->SampleTime = sampleTimeMs;

    PID_SetOutputLimits(pid, outMin, outMax);
    pid->LastTime = millisCounter - pid->SampleTime;
	pid->LastInput = *pid->Input;
	pid->IntergralSum = *pid->Output;
}
