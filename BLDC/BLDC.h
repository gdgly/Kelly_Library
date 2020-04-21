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
	BLDC_COMMUTATION_T Commutation;
	SPEED_T Speed;
	PID_T PID;
	//BLDC_ERROR_T Error;

	//speed->EventPeriod
	uint32_t PIDOutputHallPeriod;
	uint32_t PIDSetPointHallPeriod;

	uint16_t PWM;
	uint32_t TargetRPM;

	uint32_t JogSteps;
	uint32_t StepCount; //void BLDC_ResetStepCount(void);


	uint8_t IZero_ADCU; // == 125

	uint8_t * BackEMFPhaseA_ADCU;
	uint8_t * BackEMFPhaseB_ADCU;
	uint8_t * BackEMFPhaseC_ADCU;
	uint8_t * VBat_ADCU;
	uint8_t * I_ADCU;
	uint8_t * LSTemp_ADCU;
}
BLDC_CONTROLLER_T;


uint8_t * BLDC_GetPtrBackEMFPhaseA(BLDC_CONTROLLER_T * bldc);
uint8_t * BLDC_GetPtrBackEMFPhaseB(BLDC_CONTROLLER_T * bldc);
uint8_t * BLDC_GetPtrBackEMFPhaseC(BLDC_CONTROLLER_T * bldc);
uint8_t * BLDC_GetPtrVBat(BLDC_CONTROLLER_T * bldc);
uint8_t * BLDC_GetPtrI(BLDC_CONTROLLER_T * bldc);
uint8_t * BLDC_GetPtrLSTemp(BLDC_CONTROLLER_T * bldc);

#endif /* BLDC_H_ */
