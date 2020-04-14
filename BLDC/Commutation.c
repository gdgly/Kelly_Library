/*
 * Commutation.c
 *
 *  Created on: Dec 1, 2019
 *      Author: FireSourcery
 */

#include "Commutation.h"

inline void BLDC_Commutation_ISR(BLDC_COMMUTATION_T * commutation, uint16_t pwm)
{
	//	if (commutation->Direction == DIRECTION_CCW)
	//		~(commutation->GetHallSensors())&0x07
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase(pwm);
}


bool BLDC_Commutation_Poll(BLDC_COMMUTATION_T * commutation, uint16_t pwm)
{
	if (commutation->SavedHallState != commutation->GetHallState())
	{
		commutation->SavedHallState = commutation->GetHallState();
		BLDC_Commutation_ISR(commutation, pwm);
		return true;
	}

	return false;
}

void BLDC_Commutation_SetPhasePWM(BLDC_COMMUTATION_T * commutation, uint16_t pwm)
{
	commutation->CommuntationTable[commutation->GetHallState()].SetPhasePWM(pwm);
}

//void (*BLDC_Commutation_GetFunctionActivatePhase(BLDC_COMMUTATION_T * commutation))(uint16_t)
//{
//	return commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase;
//}
//
//void (*BLDC_Commutation_GetFunctionSetPhasePWM(BLDC_COMMUTATION_T * commutation))(uint16_t)
//{
//	return commutation->CommuntationTable[commutation->GetHallState()].SetPhasePWM;
//}

void BLDC_Commutation_SetDirection(BLDC_COMMUTATION_T * commutation, BLDC_DIRECTION_T dir) 
{
	// alternatively during commutation
	//	if (commutation->Direction == DIRECTION_CCW)
	//		~(commutation->GetHallSensors())&0x07
	
	commutation->Direction = dir;
	if (dir == DIRECTION_CW)		commutation->CommuntationTable = commutation->CommuntationTableCW;
	else /*(dir == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCCW;
}

void BLDC_Commutation_SetDirectionReverse(BLDC_COMMUTATION_T * commutation) 
{
	commutation->Direction = ~commutation->Direction;
	if (commutation->Direction == DIRECTION_CW)			commutation->CommuntationTable = commutation->CommuntationTableCCW;
	else /*(commutation->Direction == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCW;
}

BLDC_DIRECTION_T BLDC_Commutation_GetDirection(BLDC_COMMUTATION_T * commutation)
{
	return commutation->Direction;
}

void BLDC_Commutation_MapCommuntationTableRunCalibration
(	
	BLDC_COMMUTATION_T * commutation,
	uint8_t * returnIndexAB,
	uint8_t * returnIndexAC,
	uint8_t * returnIndexBC,
	uint8_t * returnIndexBA,
	uint8_t * returnIndexCA,
	uint8_t * returnIndexCB,
	void (*setPWMPhaseAB)(uint16_t),
	void (*setPWMPhaseAC)(uint16_t),
	void (*setPWMPhaseBC)(uint16_t),
	void (*setPWMPhaseBA)(uint16_t),
	void (*setPWMPhaseCA)(uint16_t),
	void (*setPWMPhaseCB)(uint16_t),
	uint16_t pwm,
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t),
	//pwm
	void (*enablePhaseABC)(void),
	void (*delay)(uint32_t),
	uint32_t delayTime
) 
{
	//enablePhaseABC();
	//phaseA top on
	//phaseB bottom on
	//phaseC bottom on
	commutatePhaseAB(pwm);
	commutatePhaseAC(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM		= setPWMPhaseBC;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBC;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCB;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCB;
	if (returnIndexBC) *returnIndexBC = commutation->GetHallState();
	
	commutatePhaseAC(pwm);
	commutatePhaseBC(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBA;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBA;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAB;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAB;
	if (returnIndexBA) *returnIndexBA = commutation->GetHallState();
	
	commutatePhaseBC(pwm);
	commutatePhaseBA(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCA;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCA;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAC;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAC;
	if (returnIndexCA) *returnIndexCA = commutation->GetHallState();
	
	commutatePhaseBA(pwm);
	commutatePhaseCA(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCB;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCB;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBC;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBC;
	if (returnIndexCB) *returnIndexCB = commutation->GetHallState();
	
	commutatePhaseCA(pwm);
	commutatePhaseCB(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAB;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAB;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBA;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBA;
	if (returnIndexAB) *returnIndexAB = commutation->GetHallState();
	
	commutatePhaseCB(pwm);
	commutatePhaseAB(pwm);
	if(enablePhaseABC) enablePhaseABC();
	delay(delayTime);
	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAC;
	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAC;
	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCA;
	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCA;
	if (returnIndexAC) *returnIndexAC = commutation->GetHallState();
}

void BLDC_Commutation_MapCommuntationTable
(	
	BLDC_COMMUTATION_T * commutation,				
	void (*setPWMPhaseAB)(uint16_t),
	void (*setPWMPhaseAC)(uint16_t),
	void (*setPWMPhaseBC)(uint16_t),
	void (*setPWMPhaseBA)(uint16_t),
	void (*setPWMPhaseCA)(uint16_t),
	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB
) 
{
	commutation->CommuntationTableCW[indexPhaseAB].SetPhasePWM = setPWMPhaseAB;
	commutation->CommuntationTableCW[indexPhaseAC].SetPhasePWM = setPWMPhaseAC;
	commutation->CommuntationTableCW[indexPhaseBC].SetPhasePWM = setPWMPhaseBC;
	commutation->CommuntationTableCW[indexPhaseBA].SetPhasePWM = setPWMPhaseBA;
	commutation->CommuntationTableCW[indexPhaseCA].SetPhasePWM = setPWMPhaseCA;
	commutation->CommuntationTableCW[indexPhaseCB].SetPhasePWM = setPWMPhaseCB;

	commutation->CommuntationTableCW[indexPhaseAB].ActivatePhase = commutatePhaseAB;
	commutation->CommuntationTableCW[indexPhaseAC].ActivatePhase = commutatePhaseAC;
	commutation->CommuntationTableCW[indexPhaseBC].ActivatePhase = commutatePhaseBC;
	commutation->CommuntationTableCW[indexPhaseBA].ActivatePhase = commutatePhaseBA;
	commutation->CommuntationTableCW[indexPhaseCA].ActivatePhase = commutatePhaseCA;
	commutation->CommuntationTableCW[indexPhaseCB].ActivatePhase = commutatePhaseCB;

	commutation->CommuntationTableCCW[indexPhaseAB].SetPhasePWM = setPWMPhaseBA;
	commutation->CommuntationTableCCW[indexPhaseAC].SetPhasePWM = setPWMPhaseCA;
	commutation->CommuntationTableCCW[indexPhaseBC].SetPhasePWM = setPWMPhaseCB;
	commutation->CommuntationTableCCW[indexPhaseBA].SetPhasePWM = setPWMPhaseAB;
	commutation->CommuntationTableCCW[indexPhaseCA].SetPhasePWM = setPWMPhaseAC;
	commutation->CommuntationTableCCW[indexPhaseCB].SetPhasePWM = setPWMPhaseBC;

	commutation->CommuntationTableCCW[indexPhaseAB].ActivatePhase = commutatePhaseBA;
	commutation->CommuntationTableCCW[indexPhaseAC].ActivatePhase = commutatePhaseCA;
	commutation->CommuntationTableCCW[indexPhaseBC].ActivatePhase = commutatePhaseCB;
	commutation->CommuntationTableCCW[indexPhaseBA].ActivatePhase = commutatePhaseAB;
	commutation->CommuntationTableCCW[indexPhaseCA].ActivatePhase = commutatePhaseAC;
	commutation->CommuntationTableCCW[indexPhaseCB].ActivatePhase = commutatePhaseBC;
}

void BLDC_Commutation_MapCommuntationTableHallDefault
(	
	BLDC_COMMUTATION_T * commutation,				
	void (*setPWMPhaseAB)(uint16_t),
	void (*setPWMPhaseAC)(uint16_t),
	void (*setPWMPhaseBC)(uint16_t),
	void (*setPWMPhaseBA)(uint16_t),
	void (*setPWMPhaseCA)(uint16_t),
	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t)
) 
{
	BLDC_Commutation_MapCommuntationTable
	(
		 commutation,
		 setPWMPhaseAB, setPWMPhaseAC, setPWMPhaseBC, setPWMPhaseBA, setPWMPhaseCA, setPWMPhaseCB,
		 commutatePhaseAB,	commutatePhaseAC, commutatePhaseBC, commutatePhaseBA, commutatePhaseCA, commutatePhaseCB,
		 6, 2, 3, 1, 5, 4
	);
}

void BLDC_Commutation_MapCommuntationTableFaultStates
( 
	BLDC_COMMUTATION_T * commutation,
	void (*fault000)(void),
	void (*fault111)(void)
) 
{
	commutation->CommuntationTableCW[0].SetPhasePWM = fault000;
	commutation->CommuntationTableCW[7].SetPhasePWM = fault111;
	commutation->CommuntationTableCCW[0].ActivatePhase = fault000;
	commutation->CommuntationTableCCW[7].ActivatePhase = fault111;
}

void BLDC_Commutation_Init
(
	BLDC_COMMUTATION_T * commutation,
	BLDC_DIRECTION_T dir,
	uint8_t (*getHallSensors)(void),
	void (*setPWMPhaseAB)(uint16_t),
	void (*setPWMPhaseAC)(uint16_t),
	void (*setPWMPhaseBC)(uint16_t),
	void (*setPWMPhaseBA)(uint16_t),
	void (*setPWMPhaseCA)(uint16_t),
	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB,
	void (*releasePWM)(void)
)
{
	commutation->GetHallState = getHallSensors; //change to get rotor position include sensorless
	
	if (dir == DIRECTION_CW)		commutation->CommuntationTable = commutation->CommuntationTableCW;
	else /*(dir == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCCW;
	
	BLDC_Commutation_MapCommuntationTableFaultStates(commutation, releasePWM, releasePWM);

	BLDC_Commutation_MapCommuntationTable
	(
		 commutation,
		 setPWMPhaseAB, setPWMPhaseAC, setPWMPhaseBC, setPWMPhaseBA, setPWMPhaseCA, setPWMPhaseCB,
		 commutatePhaseAB,	commutatePhaseAC, commutatePhaseBC, commutatePhaseBA, commutatePhaseCA, commutatePhaseCB,
		 indexPhaseAB, indexPhaseAC, indexPhaseBC, indexPhaseBA, indexPhaseCA, indexPhaseCB
	);
}

