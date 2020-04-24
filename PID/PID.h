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
} PID_COMPUTE_MODE_T;

typedef struct PID
{
	PID_COMPUTE_MODE_T ComputeMode;
	PID_DIRECTION_T ControllerDirection;

	uint32_t * Input;
	uint32_t * Output;
	uint32_t * SetPoint;

	int32_t KpFactor;
	int32_t KpDivisor;
	int32_t KiFactor;
	int32_t KiDivisor;
	int32_t KdFactor;
	int32_t KdDivisor;

	uint32_t SampleTime; 	// unit in timer ticks
	uint32_t TimerFreq;		// convert sample time to standard units

	int32_t IntergralSum;
	uint32_t LastTime;
	uint32_t LastInput;

	int32_t OutMin;
	int32_t OutMax;
	uint32_t ComputeInterval; 	// isr count per compute
	uint32_t ComputeCounter; 	//
}
PID_T;


bool Compute(PID_T * pid);
bool ComputeProportionalOnInput(PID_T * pid);
bool PID_ComputeTimerISR(PID_T * pid);

void PID_SetTunings(PID_T * pid, int32_t KpFactor, int32_t KpDivisor, int32_t KiFactor, int32_t KiDivisor, int32_t KdFactor, int32_t KdDivisor);
void PID_SetSampleTime(PID_T * pid, uint32_t sampleTime);
void PID_SetOutputLimits(PID_T * pid, uint32_t Min, uint32_t Max);
void PID_SetProportionMode(PID_T * pid, PID_COMPUTE_MODE_T mode);
void PID_SetControllerDirection(PID_T * pid, PID_DIRECTION_T Direction);
uint32_t PID_GetKp(PID_T * pid);
uint32_t PID_GetKi(PID_T * pid);
uint32_t PID_GetKd(PID_T * pid);
PID_DIRECTION_T PID_GetDirection(PID_T * pid);


bool PID_ComputeMillisModePoll(PID_T * pid);

#endif /* PID_H_ */
