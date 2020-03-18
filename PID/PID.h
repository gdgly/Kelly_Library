/*
 * PID.h
 *
 *  Created on: Dec 7, 2019
 *      Author: FireSourcery
 */

#ifndef PID_H_
#define PID_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	PID_DIRECTION_DIRECT, PID_DIRECTION_REVERSE,
} PID_DIRECTION_T;

typedef enum
{
	PID_P_ON_M, PID_P_ON_E,
} PID_PROPROTIONAL_MODE_T;

typedef struct PID
{
	PID_PROPROTIONAL_MODE_T ProprotionalMode;
	PID_DIRECTION_T ControllerDirection;
	uint32_t * Input;
	uint32_t * Output;
	uint32_t * SetPoint;
	int32_t Kp;
	int32_t Ki;
	int32_t Kd;
	uint32_t SampleTime; //unit in timer counts
	uint32_t TimerFreq;
	int32_t IntergralSum;
	uint32_t OutMin;
	uint32_t OutMax;
	uint32_t CyclesPerCompute;
	uint32_t CyclesPerComputeCounter;
	uint32_t LastInput;
	uint32_t LastTime;
}
PID_T;



bool PID_Compute(PID_T * pid);
bool PID_ComputeProportionalOnInput(PID_T * pid);



void PID_SetTunings(PID_T * pid, uint32_t Kp, uint32_t Ki, uint32_t Kd);
void PID_SetSampleTime(PID_T * pid, uint32_t sampleTime);
void PID_SetOutputLimits(PID_T * pid, uint32_t Min, uint32_t Max);
void PID_SetProportionMode(PID_T * pid, PID_PROPROTIONAL_MODE_T mode);
void PID_SetControllerDirection(PID_T * pid, PID_DIRECTION_T Direction);
uint32_t PID_GetKp(PID_T * pid);
uint32_t PID_GetKi(PID_T * pid);
uint32_t PID_GetKd(PID_T * pid);
PID_DIRECTION_T PID_GetDirection(PID_T * pid);
void PID_Init
(
	PID_T * pid, uint32_t * input, uint32_t * output, uint32_t * setpoint,
	int32_t Kp, int32_t Ki, int32_t Kd,
	uint32_t samplePeriod, uint32_t timerFreq, //sample time in timer ticks
	uint32_t cyclesPerCompute,
	uint32_t outMin, uint32_t outMax
);

bool PID_ComputeMillisMode(PID_T * pid);
void PID_InitMillisMode
(
	PID_T * pid, uint32_t * input, uint32_t * output, uint32_t * setpoint,
	uint32_t Kp, uint32_t Ki, uint32_t Kd,
	uint32_t sampleTime,
	uint32_t (*millis)(void),
	uint32_t outMin, uint32_t outMax
);
#endif /* PID_H_ */
