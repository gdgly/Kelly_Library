/*
 * PID.c
 *
 *  Created on: Nov 26, 2019
 *      Author: FireSourcery
 */
#include "PID.h"

#include <stdbool.h>
#include <stdint.h>

bool PID_Compute(PID_T * pid)
{
	int32_t error = *pid->SetPoint - *pid->Input;
	uint32_t output = 0;

	pid->LastInput = *pid->Input;

	pid->IntergralSum += error;

	if 		(pid->IntergralSum > pid->OutMax)	pid->IntergralSum = pid->OutMax;
	else if (pid->IntergralSum < pid->OutMin)	pid->IntergralSum = pid->OutMin;

	output = (pid->Kp * error) + (pid->Ki * pid->IntergralSum * pid->SampleTime / pid->TimerFreq) - (pid->Kd * error * pid->TimerFreq / pid->SampleTime);

	if 		(output > pid->OutMax)	output = pid->OutMax;
	else if (output < pid->OutMin)	output = pid->OutMin;

	*pid->Output = output;

	return true;
}

bool PID_ComputeProportionalOnInput(PID_T * pid)
{
	int32_t error = *pid->SetPoint - *pid->Input;
	int32_t dInput = *pid->Input - pid->LastInput;
	uint32_t output = 0;

	pid->LastInput = *pid->Input;

	pid->IntergralSum += ((pid->Ki * error) - (pid->Kp * dInput));

	if 		(pid->IntergralSum > pid->OutMax)	pid->IntergralSum = pid->OutMax;
	else if (pid->IntergralSum < pid->OutMin)	pid->IntergralSum = pid->OutMin;

	output = (pid->IntergralSum * pid->SampleTime / pid->TimerFreq) - (pid->Kd * dInput * pid->TimerFreq / pid->SampleTime);

	if 		(output > pid->OutMax)	output = pid->OutMax;
	else if (output < pid->OutMin)	output = pid->OutMin;

	*pid->Output = output;

	return true;
}

bool PID_ComputeTimerISR(PID_T * pid)
{
    if (pid->CyclesPerComputeCounter < pid->CyclesPerCompute - 1)
    {
    	pid->CyclesPerComputeCounter++;
    	return false;
    }
    else
    {
		pid->CyclesPerComputeCounter = 0;
		if (pid->ProprotionalMode == PID_P_ON_E)	return PID_Compute(pid);
		else										return PID_ComputeProportionalOnInput(pid);
    }
}

void PID_SetTunings(PID_T * pid, uint32_t Kp, uint32_t Ki, uint32_t Kd)
{
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;

	if (pid->ControllerDirection == PID_DIRECTION_REVERSE)
	{
		pid->Kp = (0 - pid->Kp);
		pid->Ki = (0 - pid->Ki);
		pid->Kd = (0 - pid->Kd);
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

   if(*pid->Output > pid->OutMax) 			*pid->Output = pid->OutMax;
   else if(*pid->Output < pid->OutMin) 		*pid->Output = pid->OutMin;

   if(pid->IntergralSum > pid->OutMax) 		pid->IntergralSum = pid->OutMax;
   else if(pid->IntergralSum < pid->OutMin) pid->IntergralSum = pid->OutMin;
}

void PID_SetProportionMode(PID_T * pid, PID_PROPROTIONAL_MODE_T mode)
{
    pid->ProprotionalMode = mode;
}

void PID_SetControllerDirection(PID_T * pid, PID_DIRECTION_T Direction)
{
   if((Direction != pid->ControllerDirection))
   {
	   pid->Kp = (0 - pid->Kp);
	   pid->Ki = (0 - pid->Ki);
	   pid->Kd = (0 - pid->Kd);
   }
   pid->ControllerDirection = Direction;
}

uint32_t PID_GetKp(PID_T * pid) {return  pid->Kp;}
uint32_t PID_GetKi(PID_T * pid) {return  pid->Ki;}
uint32_t PID_GetKd(PID_T * pid) {return  pid->Kd;}
PID_DIRECTION_T PID_GetDirection(PID_T * pid) 	{return pid->ControllerDirection;}

void PID_Init
(
	PID_T * pid, uint32_t * input, uint32_t * output, uint32_t * setpoint,
	int32_t Kp, int32_t Ki, int32_t Kd,
	uint32_t timerTicksPerSample, uint32_t timerFreq, //sample time in timer ticks
	uint32_t cyclesPerCompute,
	uint32_t outMin, uint32_t outMax
)
{
	pid->Output = output;
	pid->Input = input;
	pid->SetPoint = setpoint;
	pid->ControllerDirection = PID_DIRECTION_DIRECT;
	pid->ProprotionalMode = PID_P_ON_E;
    PID_SetTunings(pid, Kp, Ki, Kd);
	pid->IntergralSum = *pid->Output;
	pid->LastInput = *pid->Input;
    PID_SetOutputLimits(pid, outMin, outMax);
	pid->CyclesPerCompute = cyclesPerCompute;
	pid->SampleTime = timerTicksPerSample * pid->CyclesPerCompute;
	pid->TimerFreq = timerFreq;
}

static uint32_t (*Millis)(void);

bool PID_ComputeMillisMode(PID_T * pid)
{
	if (Millis() - pid->LastTime > pid->SampleTime)
	{
		pid->LastTime = Millis();
		if (pid->ProprotionalMode == PID_P_ON_E)	return PID_Compute(pid);
		else										return PID_ComputeProportionalOnInput(pid);
	}

	return false;
}

void PID_InitMillisMode
(
	PID_T * pid, uint32_t * input, uint32_t * output, uint32_t * setpoint,
	uint32_t Kp, uint32_t Ki, uint32_t Kd,
	uint32_t sampleTimeMs,
	uint32_t (*millis)(void),
	uint32_t outMin, uint32_t outMax
)
{
	pid->Output = output;
	pid->Input = input;
	pid->SetPoint = setpoint;
	pid->ControllerDirection = PID_DIRECTION_DIRECT;
    PID_SetTunings(pid, Kp, Ki, Kd);
	pid->IntergralSum = *pid->Output;
	pid->LastInput = *pid->Input;
    PID_SetOutputLimits(pid, outMin, outMax);
	pid->SampleTime = sampleTimeMs;
	pid->TimerFreq = 1000;
	Millis = millis;
    pid->LastTime = Millis() - pid->SampleTime;
}
