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
	if (bldc->State == MOTOR_STATE_RUN) Commutation_Poll(bldc->Commutation, bldc->PWM);
}

inline void BLDC_ProcessRunHallISR(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->State == MOTOR_STATE_RUN) Commutation_ISR(bldc->Commutation, bldc->PWM);
}

//run in main
void BLDC_Process(BLDC_CONTROLLER_T * bldc)
{
	// boundless input
	switch (bldc->State)
	{
		case MOTOR_STATE_STANDBY:
			BLDC_Jog(bldc);
			break;

		case MOTOR_STATE_RUN:
			// if (BLDC_Commutation_Poll(&bldc->Commutation, bldc->PWM)) //commutate in pwm cycle timer
			break;

		//case MOTOR_STATE_RUN:
			//state bounded input
			break;
	}

	//or give each state function pointer and avoid switch statement
	//bldc->State->function();

	//Monitor_Process(bldc->Monitor)
	//if (Monitor_GetError()) Stop();

}

void BLDC_CaptureBEMF(BLDC_CONTROLLER_T * bldc, uint8_t * backEMF_ADCU)
{
	bldc->BackEMF_ADCU = *backEMF_ADCU;
}

void BLDC_SetBEMF(BLDC_CONTROLLER_T * bldc, uint8_t backEMF_ADCU)
{
	bldc->BackEMF_ADCU = backEMF_ADCU;
}


/*-----------------------------------------------------------------------------
  Jog
 *----------------------------------------------------------------------------*/
void BLDC_Jog(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->JogSteps)
	{
		if (Commutation_Poll(bldc->Commutation, bldc->PWM)) bldc->JogSteps--;
	}
}

void BLDC_SetJogSteps(BLDC_CONTROLLER_T * bldc, uint8_t steps)
{
	bldc->JogSteps = steps;
}

/*-----------------------------------------------------------------------------
  Inputs
 *----------------------------------------------------------------------------*/
void BLDC_ApplyPWM(BLDC_CONTROLLER_T * bldc)
{
	Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM);
}

void BLDC_SetPWM(BLDC_CONTROLLER_T * bldc, uint16_t pwm)
{
	bldc->PWM = pwm;
	Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM);
}

void BLDC_SetVoltage(BLDC_CONTROLLER_T * bldc, uint16_t appliedVoltage)
{
	bldc->PWM = appliedVoltage*bldc->PWMMax/VoltageDivider_GetVoltage(bldc->VDivBat, *bldc->VBat_ADCU);
}

void BLDC_SetVoltageADCU(BLDC_CONTROLLER_T * bldc, uint16_t appliedVoltageADCU)
{
	bldc->PWM = appliedVoltageADCU * bldc->PWMMax / (*bldc->VBat_ADCU);
}

void BLDC_Stop(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	bldc->FloatMotor();
}

void BLDC_Start(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_RUN;
	Commutation_ISR(bldc->Commutation, bldc->PWM);
}

void BLDC_Hold(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM); //change to hold strength PWM, match to hall state
}

void BLDC_Coast(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	bldc->FloatMotor();
}

void BLDC_DynamicBrake(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	bldc->ShortMotor(); //set pwm = 0
}

void BLDC_PluggingBrake(BLDC_CONTROLLER_T * bldc)
{
	Commutation_SetDirectionReverse(bldc->Commutation);
}

void BLDC_RegenBrakeOptimal(BLDC_CONTROLLER_T * bldc)
{
	//PID_SetSetPoint(pid, bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2);
	BLDC_SetVoltageADCU(bldc, *bldc->BackEMFSelect_ADCU/2); //todo backemf average
}

void BLDC_RegenBrake(BLDC_CONTROLLER_T * bldc, uint8_t intensity)
{
	//braking softer / max regen -> pwm 75 of back emf;
	//braking medium / max regen -> pwm 50 of back emf;
	//braking harder / max regen -> pwm 25 of back emf;
}


void BLDC_Init
(
	BLDC_CONTROLLER_T * bldc,
	COMMUTATION_T * commutation,
	SPEED_T * speed,
	PID_T * pid,
	//MONITOR_T * monitor,
	VOLTAGE_DIVIDER_T * vDivBat,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseA,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseB,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseC,
	void(*floatMotor)(void),
	void(*shortMotor)(void),
	uint16_t pwmMax
)
{
	bldc->Commutation = commutation;
	bldc->Speed = speed;
	bldc->PID = pid;
	bldc->VDivBackEMFPhaseA = vDivBackEMFPhaseA;
	bldc->VDivBackEMFPhaseB = vDivBackEMFPhaseB;
	bldc->VDivBackEMFPhaseC = vDivBackEMFPhaseC;
	bldc->VDivBat = vDivBat;
	bldc->PWMMax = pwmMax;

	bldc->FloatMotor = floatMotor;
	bldc->ShortMotor = shortMotor;
	bldc->State	= MOTOR_STATE_STANDBY;

	bldc->BackEMFSelect_ADCU = bldc->BackEMFPhaseA_ADCU;
}






