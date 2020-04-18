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

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	MOTOR_STATE_STANDBY,
	MOTOR_STATE_STARTUP,
	MOTOR_STATE_FAULT,
	MOTOR_STATE_RUN,
}  MOTOR_STATE_T;

typedef struct 
{
	MOTOR_STATE_T State;
	COMMUTATION_T * Commutation;
	SPEED_T * Speed;
	PID_T * PID;
	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseA;
	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseB;
	VOLTAGE_DIVIDER_T * VDivBackEMFPhaseC;
	VOLTAGE_DIVIDER_T * VDivBat;
	VOLTAGE_DIVIDER_T * VDivTemperature;
	//BLDC_ERROR_T Error;

	void(*ShortMotor)(void);
	void(*FloatMotor)(void);

	uint16_t PWM;
	uint16_t PWMMax;

//  speed->EventPeriod
//	uint32_t PIDOutputHallPeriod;
//	uint32_t PIDSetPointHallPeriod;
	uint32_t JogSteps;
	uint32_t StepCount;

	uint32_t SetRPM;
	uint32_t RPMMax; //set proportional to max voltage?

	uint16_t SetVoltage; //desired speed

	//uint16_t OptimalRegen;

	//Monitor

	uint8_t IZero_ADCU; // == 125
	uint8_t IMax_ADCU;

	uint8_t OverVoltage_ADCU;
	uint8_t UnderVoltage_ADCU;

	uint8_t * BackEMFSelect_ADCU;

	uint8_t BackEMF_ADCU;

	uint8_t * BackEMFPhaseA_ADCU;
	uint8_t * BackEMFPhaseB_ADCU;
	uint8_t * BackEMFPhaseC_ADCU;
	uint8_t * VBat_ADCU;
	uint8_t * I_ADCU;
	uint8_t * LSTemp_ADCU;
}
BLDC_CONTROLLER_T;




#endif /* BLDC_H_ */
