/*
 * BLDC.c
 *
 *  Created on: Oct 24, 2019
 *      Author: FireSourcery
 */

#include "BLDC.h"

/*-----------------------------------------------------------------------------
  Process
 *----------------------------------------------------------------------------*/
inline void BLDC_ProcessRunPoll(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->MotorMode == MOTOR_MODE_RUN) Commutation_Poll(bldc->Commutation, bldc->Waveform, bldc->PWM);
}

inline void BLDC_ProcessRunHallISR(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->MotorMode == MOTOR_MODE_RUN) Commutation_ISR(bldc->Commutation, bldc->Waveform, bldc->PWM);
}

//run in main
void BLDC_Process(BLDC_CONTROLLER_T * bldc)
{
	// boundless input
	switch (bldc->MotorMode)
	{
		case MOTOR_MODE_STANDBY:
			BLDC_Jog(bldc);
			break;

		case MOTOR_MODE_RUN:
			// if (BLDC_Commutation_Poll(&bldc->Commutation, bldc->PWM)) //commutate in pwm cycle timer
			break;

		//case MOTOR_MODE_RUN:
			//state bounded input
			//break;

		case MOTOR_MODE_STARTUP:
			break;

		case MOTOR_MODE_FAULT:
			break;
	}

	//or give each state function pointer and avoid switch statement
	//bldc->State->function();

	//Monitor_Process(bldc->Monitor)
	//if (Monitor_GetError()) Stop(); MotorMode = MOTOR_MODE_FAULT

}


/*-----------------------------------------------------------------------------
  Jog
 *----------------------------------------------------------------------------*/
void BLDC_Jog(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->JogSteps) //todo negative
	{
		if (Commutation_Poll(bldc->Commutation, bldc->Waveform, bldc->PWM)) bldc->JogSteps--;
	}
}

void BLDC_SetJogSteps(BLDC_CONTROLLER_T * bldc, uint8_t steps)
{
	bldc->JogSteps = steps;
}


/*-----------------------------------------------------------------------------
  PWM
 *----------------------------------------------------------------------------*/
void BLDC_ApplyPWM(BLDC_CONTROLLER_T * bldc)
{
	Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM);
}

void BLDC_SetPWM(BLDC_CONTROLLER_T * bldc, uint16_t pwm)
{
	bldc->PWM = pwm; //need Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM); to actually change speed
}

void BLDC_SetPWMVoltage(BLDC_CONTROLLER_T * bldc, uint16_t voltage)
{
	bldc->PWM = voltage*bldc->PWMMax/VoltageDivider_GetVoltage(bldc->Monitor->VDivBat, *bldc->Monitor->VBat_ADCU);
}

void BLDC_SetPWMVoltageADCU(BLDC_CONTROLLER_T * bldc, uint16_t voltageADCU)
{
	bldc->PWM = voltageADCU * bldc->PWMMax / (*bldc->Monitor->VBat_ADCU);
}

uint8_t BLDC_GetPWMDutyCycle(BLDC_CONTROLLER_T * bldc)
{
	return bldc->PWM / bldc->PWMMax;
}

uint16_t BLDC_GetPWMVoltage(BLDC_CONTROLLER_T * bldc)
{
	return VoltageDivider_GetVoltage(bldc->Monitor->VDivBat, *bldc->Monitor->VBat_ADCU) * bldc->PWM / bldc->PWMMax;
}

/*-----------------------------------------------------------------------------
  Inputs
 *----------------------------------------------------------------------------*/
void BLDC_Start(BLDC_CONTROLLER_T * bldc)
{
	bldc->MotorMode = MOTOR_MODE_RUN;
	Commutation_ISR(bldc->Commutation, bldc->Waveform, bldc->PWM);
	//Speed_ResetTimer(bldc->Speed);
}

void BLDC_Stop(BLDC_CONTROLLER_T * bldc)
{
	bldc->MotorMode = MOTOR_MODE_STANDBY;
	bldc->FloatMotor();
}

void BLDC_Coast(BLDC_CONTROLLER_T * bldc)
{
	bldc->MotorMode = MOTOR_MODE_STANDBY;
	bldc->FloatMotor();
}

void BLDC_DynamicBrake(BLDC_CONTROLLER_T * bldc)
{
	bldc->MotorMode = MOTOR_MODE_STANDBY;
	bldc->ShortMotor(); //set pwm = 0
}

void BLDC_PluggingBrake(BLDC_CONTROLLER_T * bldc)
{
	Commutation_SetDirectionReverse(bldc->Commutation);
}

void BLDC_RegenBrakeOptimal(BLDC_CONTROLLER_T * bldc)
{
	//PID_SetSetPoint(pid, bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2);
	BLDC_SetVoltageADCU(bldc, bldc->Monitor->BackEMFSelectBuffer_ADCU/2); //todo backemf average
}

void BLDC_RegenBrake(BLDC_CONTROLLER_T * bldc, uint8_t intensity)
{
	//braking softer / max regen -> pwm 75 of back emf;
	//braking medium / max regen -> pwm 50 of back emf;
	//braking harder / max regen -> pwm 25 of back emf;
}

void BLDC_Hold(BLDC_CONTROLLER_T * bldc)
{
	bldc->MotorMode = MOTOR_MODE_STANDBY;
	//Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM); //change to hold strength PWM, match to hall state
}

void BLDC_Init
(
	BLDC_CONTROLLER_T * bldc,
	COMMUTATION_T * commutation,
	SPEED_T * speed,
	PID_T * pid,
	MONITOR_T * monitor,

	void(*floatMotor)(void),
	void(*shortMotor)(void),
	uint16_t pwmMax
)
{
	bldc->Commutation = commutation;
	bldc->Speed = speed;
	bldc->PID = pid;
	bldc->Monitor = monitor;

	bldc->PWMMax = pwmMax;

	bldc->FloatMotor = floatMotor;
	bldc->ShortMotor = shortMotor;
	bldc->MotorMode	= MOTOR_MODE_STANDBY;

	bldc->RPM = 0;
}






