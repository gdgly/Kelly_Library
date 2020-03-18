/*
 * BLDC.c
 *
 *  Created on: Oct 24, 2019
 *      Author: FireSourcery
 */

#include "BLDC.h"

/*-----------------------------------------------------------------------------
  Jog
 *----------------------------------------------------------------------------*/
void BLDC_SetJogSteps(BLDC_CONTROLLER_T * bldc, uint8_t steps)
{
	bldc->JogSteps = steps;
}

void BLDC_Jog(BLDC_CONTROLLER_T * bldc)
{
	if (bldc->JogSteps)
	{
		if (BLDC_Commutation_Poll(&bldc->Commutation, bldc->PWM)) bldc->JogSteps--;
	}
}

void BLDC_Release(BLDC_CONTROLLER_T * bldc)
{
	bldc->Commutation.CommuntationTable[0].ActivatePhase(0);
}

/*-----------------------------------------------------------------------------
  Misc
 *----------------------------------------------------------------------------*/
void BLDC_SetPWM(BLDC_CONTROLLER_T * bldc, uint16_t pwm)
{
	bldc->PWM = pwm;
}

/*-----------------------------------------------------------------------------
  Regen
 *----------------------------------------------------------------------------*/
//braking softer / max regen -> pwm 75 of back emf;
//braking medium / max regen -> pwm 50 of back emf;
//braking harder / max regen -> pwm 25 of back emf;
//void BLDC_Regen(BLDC_CONTROLLER_T * bldc)
//{
//	if ( bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2 ) bldc->PWM--;
//}
//
//void BLDC_RegenPID(BLDC_CONTROLLER_T * bldc)
//{
//	PID_SetSetPoint(pid, bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2);
//	if ( bldc->PWM / PWM_MAX * VBatADCU > BackEMFADCU/2 ) bldc->PWM--;
//}

/*-----------------------------------------------------------------------------
  Monitor
 *----------------------------------------------------------------------------*/
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


/*-----------------------------------------------------------------------------
  Process
 *----------------------------------------------------------------------------*/
void BLDC_Process(BLDC_CONTROLLER_T * bldc)
{
	//boundless input
	switch (bldc->State)
	{
		case MOTOR_STATE_STANDBY:
			BLDC_Jog(bldc);
			break;

		case MOTOR_STATE_RUN:
//			if (BLDC_Commutation_Poll(&bldc->Commutation, bldc->PWM))
//			{
//
//			}
			break;

		//case MOTOR_STATE_RUN:
			//state bounded input
			break;
	}

	//or give each state function pointer and avoid switch statement
	//bldc->State->function();
}
