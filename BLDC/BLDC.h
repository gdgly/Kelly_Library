/*
 * BLDC.h
 *
 *  Created on: Oct 24, 2019
 *      Author: FireSourcery
 */

#ifndef BLDC_H_
#define BLDC_H_

#include "Monitor.h"
#include "Speed.h"
#include "Commutation.h"
#include "PID.h"
#include "VoltageDivider.h"
#include "Waveform.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	MOTOR_MODE_STANDBY,
	MOTOR_MODE_STARTUP,
	MOTOR_MODE_FAULT,
	MOTOR_MODE_RUN,
}  MOTOR_MODE_T;

typedef struct 
{
	MOTOR_MODE_T 	MotorMode;
	COMMUTATION_T 	* Commutation;
	SPEED_T 		* Speed;
	PID_T 			* PID;
	MONITOR_T 		* Monitor;
	WAVEFORM_T 		* Waveform;

	void(*ShortMotor)(void);
	void(*FloatMotor)(void);
	//uint16_t OptimalRegen;

	uint32_t PWM;
	uint16_t PWMMax; // PWM 100% duty cycle value, use monitor for physical speed limit

	uint32_t JogSteps;
	uint32_t StepCount;

	uint32_t RPM;
	uint32_t SetPointRPM;
	uint32_t RPMMax; //set proportional to max voltage?

	uint32_t SetPointVoltage; //desired speed
	uint16_t PWMVoltage;
}
BLDC_CONTROLLER_T;

void BLDC_ProcessRunPoll(BLDC_CONTROLLER_T * bldc);
void BLDC_ProcessRunHallISR(BLDC_CONTROLLER_T * bldc);


#endif /* BLDC_H_ */
