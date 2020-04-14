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
	if (bldc->State == MOTOR_STATE_RUN) BLDC_Commutation_Poll(bldc->Commutation, bldc->PWM);
}

inline void BLDC_ProcessRunHallISR(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->State == MOTOR_STATE_RUN) BLDC_Commutation_ISR(bldc->Commutation, bldc->PWM);
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
}

void BLDC_CaptureBEMF(BLDC_CONTROLLER_T * bldc, uint8_t * backEMF_ADCU)
{
	bldc->BackEMFSelect_ADCU = backEMF_ADCU;
}

/*-----------------------------------------------------------------------------
  Jog
 *----------------------------------------------------------------------------*/
void BLDC_Jog(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->JogSteps)
	{
		if (BLDC_Commutation_Poll(bldc->Commutation, bldc->PWM)) bldc->JogSteps--;
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
	BLDC_Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM);
}

void BLDC_SetPWM(BLDC_CONTROLLER_T * bldc, uint16_t pwm)
{
	bldc->PWM = pwm;
	BLDC_Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM);
}

void BLDC_ApplyVoltage(BLDC_CONTROLLER_T * bldc, uint16_t appliedVoltage)
{
	bldc->PWM = appliedVoltage*bldc->PWMMax/VoltageDivider_GetVoltage(bldc->VDivBat, *bldc->VBat_ADCU);
}

void BLDC_ApplyVoltageADCU(BLDC_CONTROLLER_T * bldc, uint16_t appliedVoltageADCU)
{
	bldc->PWM = appliedVoltageADCU * bldc->PWMMax / (*bldc->VBat_ADCU);
}

void BLDC_Stop(BLDC_CONTROLLER_T * bldc)
{
	bldc->PWM = 0;
	bldc->State = MOTOR_STATE_STANDBY;
}

void BLDC_Start(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_RUN;
	BLDC_Commutation_ISR(bldc->Commutation, bldc->PWM);
}

void BLDC_Hold(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	BLDC_Commutation_SetPhasePWM(bldc->Commutation, bldc->PWM); //change to hold strength PWM, match to hall state
}

void BLDC_Release(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	bldc->DisablePWM();
	//bldc->Commutation->CommuntationTable[0].ActivatePhase(0);
}

void BLDC_Coast(BLDC_CONTROLLER_T * bldc)
{
	bldc->State = MOTOR_STATE_STANDBY;
	bldc->DisablePWM();
	//bldc->Commutation->CommuntationTable[0].ActivatePhase(0);
}



/*-----------------------------------------------------------------------------
  Regen
 *----------------------------------------------------------------------------*/
//braking softer / max regen -> pwm 75 of back emf;
//braking medium / max regen -> pwm 50 of back emf;
//braking harder / max regen -> pwm 25 of back emf;
void BLDC_ApplyBrakeOptimalRegen(BLDC_CONTROLLER_T * bldc)
{
	//PID_SetSetPoint(pid, bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2);
	BLDC_ApplyVoltageADCU(bldc, *bldc->BackEMFSelect_ADCU/2); //todo backemf average
}

void BLDC_ApplyBrake(BLDC_CONTROLLER_T * bldc, uint8_t intensity)
{

}


void BLDC_Init
(
	BLDC_CONTROLLER_T * bldc,
	BLDC_COMMUTATION_T * commutation,
	SPEED_T * speed,
	PID_T * pid,
	VOLTAGE_DIVIDER_T * vDivBat,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseA,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseB,
	VOLTAGE_DIVIDER_T * vDivBackEMFPhaseC,
	void(*disablePWM)(void),
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

	bldc->DisablePWM = disablePWM;
	bldc->State	= MOTOR_STATE_STANDBY;

	bldc->BackEMFSelect_ADCU = bldc->BackEMFPhaseA_ADCU;
}



/*-----------------------------------------------------------------------------
  Monitor
 *----------------------------------------------------------------------------*/



//		//BLDC Fault checking
//
//		if
//		(
//			ErrorFlags.HallSensor1 ||
//			ErrorFlags.Identify1 ||
//			ErrorFlags.HallSensor2 ||
//			ErrorFlags.Identify2 ||
//			ErrorFlags.OverVolt /*|| Motor_Temp_Err*/ ||
//			ErrorFlags.LowVolt ||
//			ErrorFlags.StartupTPS||
//			ErrorFlags.Reverse ||
//			ErrorFlags.RegenOverVolt ||
//			ErrorFlags.Com /*||Hall_Sensor_Err||Identify_Err*/  ||
//			ErrorFlags.OverTemp
//		)
//		{
//			SE_1_ALL_PIN_OFF();
//			EN_1_ALL_PIN_OFF();
//			SE_2_ALL_PIN_OFF();
//			EN_2_ALL_PIN_OFF();
//			//M_En=0;
//			//Global_En=0;
//			//Target_TPSx64 = 0;
//		}
//		else
//		{
//			//Global_En = 1;
//		}


uint8_t * BLDC_GetPtrBackEMFPhaseA(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->BackEMFPhaseA_ADCU;
}

uint8_t * BLDC_GetPtrBackEMFPhaseB(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->BackEMFPhaseB_ADCU;
}

uint8_t * BLDC_GetPtrBackEMFPhaseC(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->BackEMFPhaseC_ADCU;
}

uint8_t * BLDC_GetPtrVBat(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->VBat_ADCU;
}

uint8_t * BLDC_GetPtrI(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->I_ADCU;
}

uint8_t * BLDC_GetPtrLSTemp(BLDC_CONTROLLER_T * bldc)
{
	return &bldc->LSTemp_ADCU;
}


void BLDC_MapBackEMFPhaseA(BLDC_CONTROLLER_T * bldc, uint8_t * address)
{
	bldc->BackEMFPhaseA_ADCU = address;
}

void BLDC_MapBackEMFPhaseB(BLDC_CONTROLLER_T * bldc, uint8_t * address)
{
	bldc->BackEMFPhaseB_ADCU = address;
}

void BLDC_MapBackEMFPhaseC(BLDC_CONTROLLER_T * bldc, uint8_t * address)
{
	bldc->BackEMFPhaseC_ADCU = address;
}


