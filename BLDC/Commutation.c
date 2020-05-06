/*
 * Commutation.c
 *
 *  Created on: Dec 1, 2019
 *      Author: FireSourcery
 */

#include "Commutation.h"

inline void Commutation_ISR(COMMUTATION_T * commutation, uint16_t pwm)
{
	if (commutation->Direction == DIRECTION_CCW)
		commutation->CommuntationTable[(~commutation->GetHallState())&0x07].ActivatePhase(pwm);
	else
		commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase(pwm);
}


bool Commutation_Poll(COMMUTATION_T * commutation, uint16_t pwm)
{
	if (commutation->SavedHallState != commutation->GetHallState())
	{
		commutation->SavedHallState = commutation->GetHallState();
		Commutation_ISR(commutation, pwm);
		return true;
	}

	return false;
}

void Commutation_SetDirection(COMMUTATION_T * commutation, COMMUTATION_DIRECTION_T dir) 
{
	commutation->Direction = dir;
//	if (dir == DIRECTION_CW)		commutation->CommuntationTable = commutation->CommuntationTableCW;
//	else /*(dir == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCCW;
}

void Commutation_SetDirectionReverse(COMMUTATION_T * commutation) 
{
	commutation->Direction = ~commutation->Direction;
//	if (commutation->Direction == DIRECTION_CW)			commutation->CommuntationTable = commutation->CommuntationTableCCW;
//	else /*(commutation->Direction == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCW;
}

COMMUTATION_DIRECTION_T Commutation_GetDirection(COMMUTATION_T * commutation)
{
	return commutation->Direction;
}


void Commutation_MapCommuntationTable
(
	COMMUTATION_T * commutation,
//	void (*setPWMPhaseAB)(uint16_t),
//	void (*setPWMPhaseAC)(uint16_t),
//	void (*setPWMPhaseBC)(uint16_t),
//	void (*setPWMPhaseBA)(uint16_t),
//	void (*setPWMPhaseCA)(uint16_t),
//	void (*setPWMPhaseCB)(uint16_t),
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
	commutation->CommuntationTable[indexPhaseAB].ActivatePhase = commutatePhaseAB;
	commutation->CommuntationTable[indexPhaseAC].ActivatePhase = commutatePhaseAC;
	commutation->CommuntationTable[indexPhaseBC].ActivatePhase = commutatePhaseBC;
	commutation->CommuntationTable[indexPhaseBA].ActivatePhase = commutatePhaseBA;
	commutation->CommuntationTable[indexPhaseCA].ActivatePhase = commutatePhaseCA;
	commutation->CommuntationTable[indexPhaseCB].ActivatePhase = commutatePhaseCB;

//	commutation->CommuntationTableCW[indexPhaseAB].SetPhasePWM = setPWMPhaseAB;
//	commutation->CommuntationTableCW[indexPhaseAC].SetPhasePWM = setPWMPhaseAC;
//	commutation->CommuntationTableCW[indexPhaseBC].SetPhasePWM = setPWMPhaseBC;
//	commutation->CommuntationTableCW[indexPhaseBA].SetPhasePWM = setPWMPhaseBA;
//	commutation->CommuntationTableCW[indexPhaseCA].SetPhasePWM = setPWMPhaseCA;
//	commutation->CommuntationTableCW[indexPhaseCB].SetPhasePWM = setPWMPhaseCB;
//
//	commutation->CommuntationTableCW[indexPhaseAB].ActivatePhase = commutatePhaseAB;
//	commutation->CommuntationTableCW[indexPhaseAC].ActivatePhase = commutatePhaseAC;
//	commutation->CommuntationTableCW[indexPhaseBC].ActivatePhase = commutatePhaseBC;
//	commutation->CommuntationTableCW[indexPhaseBA].ActivatePhase = commutatePhaseBA;
//	commutation->CommuntationTableCW[indexPhaseCA].ActivatePhase = commutatePhaseCA;
//	commutation->CommuntationTableCW[indexPhaseCB].ActivatePhase = commutatePhaseCB;
//
//	commutation->CommuntationTableCCW[indexPhaseAB].SetPhasePWM = setPWMPhaseBA;
//	commutation->CommuntationTableCCW[indexPhaseAC].SetPhasePWM = setPWMPhaseCA;
//	commutation->CommuntationTableCCW[indexPhaseBC].SetPhasePWM = setPWMPhaseCB;
//	commutation->CommuntationTableCCW[indexPhaseBA].SetPhasePWM = setPWMPhaseAB;
//	commutation->CommuntationTableCCW[indexPhaseCA].SetPhasePWM = setPWMPhaseAC;
//	commutation->CommuntationTableCCW[indexPhaseCB].SetPhasePWM = setPWMPhaseBC;
//
//	commutation->CommuntationTableCCW[indexPhaseAB].ActivatePhase = commutatePhaseBA;
//	commutation->CommuntationTableCCW[indexPhaseAC].ActivatePhase = commutatePhaseCA;
//	commutation->CommuntationTableCCW[indexPhaseBC].ActivatePhase = commutatePhaseCB;
//	commutation->CommuntationTableCCW[indexPhaseBA].ActivatePhase = commutatePhaseAB;
//	commutation->CommuntationTableCCW[indexPhaseCA].ActivatePhase = commutatePhaseAC;
//	commutation->CommuntationTableCCW[indexPhaseCB].ActivatePhase = commutatePhaseBC;
}

void Commutation_RunCalibration
(
	COMMUTATION_T * commutation,
	uint8_t * returnIndexAB,
	uint8_t * returnIndexAC,
	uint8_t * returnIndexBC,
	uint8_t * returnIndexBA,
	uint8_t * returnIndexCA,
	uint8_t * returnIndexCB,
	void (*setPWMPhaseABC)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	uint16_t pwm,
	void (*enablePhaseABC)(bool enA, bool enB, bool enC),
	void (*delay)(uint32_t),
	uint32_t delayTime
)
{
	enablePhaseABC(1,1,1);

	setPWMPhaseABC(pwm, 0, 0); 		delay(delayTime); 	*returnIndexBC = commutation->GetHallState();
	setPWMPhaseABC(pwm, pwm, 0);	delay(delayTime); 	*returnIndexBA = commutation->GetHallState();
	setPWMPhaseABC(0, pwm, 0);		delay(delayTime); 	*returnIndexCA = commutation->GetHallState();
	setPWMPhaseABC(0, pwm, pwm); 	delay(delayTime); 	*returnIndexCB = commutation->GetHallState();
	setPWMPhaseABC(0, 0, pwm); 		delay(delayTime); 	*returnIndexAB = commutation->GetHallState();
	setPWMPhaseABC(pwm, 0, pwm); 	delay(delayTime); 	*returnIndexAC = commutation->GetHallState();

	enablePhaseABC(0,0,0);
}

void Commutation_MapCommuntationTableRunCalibration
(
	COMMUTATION_T * commutation,
	uint8_t * returnIndexAB,
	uint8_t * returnIndexAC,
	uint8_t * returnIndexBC,
	uint8_t * returnIndexBA,
	uint8_t * returnIndexCA,
	uint8_t * returnIndexCB,
//	void (*setPWMPhaseAB)(uint16_t),
//	void (*setPWMPhaseAC)(uint16_t),
//	void (*setPWMPhaseBC)(uint16_t),
//	void (*setPWMPhaseBA)(uint16_t),
//	void (*setPWMPhaseCA)(uint16_t),
//	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t),
	void (*setPWMPhaseABC)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	uint16_t pwm,
	void (*enablePhaseABC)(bool enA, bool enB, bool enC),
	void (*delay)(uint32_t),
	uint32_t delayTime
) 
{
//	commutatePhaseAB(pwm);
//	commutatePhaseAC(pwm);
	setPWMPhaseABC(pwm, 0, 0);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBC;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM		= setPWMPhaseBC;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBC;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCB;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCB;
	if (returnIndexBC) *returnIndexBC = commutation->GetHallState();
	
//	commutatePhaseAC(pwm);
//	commutatePhaseBC(pwm);
	setPWMPhaseABC(pwm, pwm, 0);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBA;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBA;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBA;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAB;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAB;
	if (returnIndexBA) *returnIndexBA = commutation->GetHallState();
	
//	commutatePhaseBC(pwm);
//	commutatePhaseBA(pwm);
	setPWMPhaseABC(0, pwm, 0);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCA;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCA;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCA;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAC;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAC;
	if (returnIndexCA) *returnIndexCA = commutation->GetHallState();
	
//	commutatePhaseBA(pwm);
//	commutatePhaseCA(pwm);
	setPWMPhaseABC(0, pwm, pwm);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCB;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCB;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCB;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBC;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBC;
	if (returnIndexCB) *returnIndexCB = commutation->GetHallState();
	
//	commutatePhaseCA(pwm);
//	commutatePhaseCB(pwm);
	setPWMPhaseABC(0, 0, pwm);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAB;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAB;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAB;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseBA;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseBA;
	if (returnIndexAB) *returnIndexAB = commutation->GetHallState();
	
//	commutatePhaseCB(pwm);
//	commutatePhaseAB(pwm);
	setPWMPhaseABC(pwm, 0, pwm);
	if(enablePhaseABC) enablePhaseABC(1,1,1);
	delay(delayTime);
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAC;
//	commutation->CommuntationTableCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseAC;
//	commutation->CommuntationTableCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseAC;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].SetPhasePWM 		= setPWMPhaseCA;
//	commutation->CommuntationTableCCW[commutation->GetHallState()].ActivatePhase 	= commutatePhaseCA;
	if (returnIndexAC) *returnIndexAC = commutation->GetHallState();

	enablePhaseABC(0,0,0);
}

void Commutation_MapCommuntationTableHallDefault
(	
	COMMUTATION_T * commutation,				
//	void (*setPWMPhaseAB)(uint16_t),
//	void (*setPWMPhaseAC)(uint16_t),
//	void (*setPWMPhaseBC)(uint16_t),
//	void (*setPWMPhaseBA)(uint16_t),
//	void (*setPWMPhaseCA)(uint16_t),
//	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(uint16_t),
	void (*commutatePhaseAC)(uint16_t),
	void (*commutatePhaseBC)(uint16_t),
	void (*commutatePhaseBA)(uint16_t),
	void (*commutatePhaseCA)(uint16_t),
	void (*commutatePhaseCB)(uint16_t)
) 
{
	Commutation_MapCommuntationTable
	(
		 commutation,
		 //setPWMPhaseAB, setPWMPhaseAC, setPWMPhaseBC, setPWMPhaseBA, setPWMPhaseCA, setPWMPhaseCB,
		 commutatePhaseAB,	commutatePhaseAC, commutatePhaseBC, commutatePhaseBA, commutatePhaseCA, commutatePhaseCB,
		 6, 2, 3, 1, 5, 4
	);
}

void Commutation_MapCommuntationTableFaultStates
( 
	COMMUTATION_T * commutation,
	void (*fault000)(void),
	void (*fault111)(void)
) 
{
//	commutation->CommuntationTable[0].SetPhasePWM = fault000;
//	commutation->CommuntationTable[7].SetPhasePWM = fault111;
	commutation->CommuntationTable[0].ActivatePhase = fault000;
	commutation->CommuntationTable[7].ActivatePhase = fault111;
}

void Commutation_Init
(
	COMMUTATION_T * commutation,
//	COMMUTATION_DIRECTION_T dir,
	uint8_t (*getHallSensors)(void),
//	void (*setPWMPhaseAB)(uint16_t),
//	void (*setPWMPhaseAC)(uint16_t),
//	void (*setPWMPhaseBC)(uint16_t),
//	void (*setPWMPhaseBA)(uint16_t),
//	void (*setPWMPhaseCA)(uint16_t),
//	void (*setPWMPhaseCB)(uint16_t),
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
	void (*fault000)(void),
	void (*fault111)(void)
)
{
	commutation->GetHallState = getHallSensors;
	
//	if (dir == DIRECTION_CW)		commutation->CommuntationTable = commutation->CommuntationTableCW;
//	else /*(dir == DIRECTION_CCW)*/	commutation->CommuntationTable = commutation->CommuntationTableCCW;
	
	Commutation_MapCommuntationTable
	(
		 commutation,
//		 setPWMPhaseAB, setPWMPhaseAC, setPWMPhaseBC, setPWMPhaseBA, setPWMPhaseCA, setPWMPhaseCB,
		 commutatePhaseAB,	commutatePhaseAC, commutatePhaseBC, commutatePhaseBA, commutatePhaseCA, commutatePhaseCB,
		 indexPhaseAB, indexPhaseAC, indexPhaseBC, indexPhaseBA, indexPhaseCA, indexPhaseCB
	);

	Commutation_MapCommuntationTableFaultStates(commutation, fault000, fault111);
}


//void Commutation_SetPhasePWM(COMMUTATION_T * commutation, uint16_t pwm)
//{
//	commutation->CommuntationTable[commutation->GetHallState()].SetPhasePWM(pwm);
//}

//void (*Commutation_GetFunctionActivatePhase(COMMUTATION_T * commutation))(uint16_t)
//{
//	return commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase;
//}
//
//void (*Commutation_GetFunctionSetPhasePWM(COMMUTATION_T * commutation))(uint16_t)
//{
//	return commutation->CommuntationTable[commutation->GetHallState()].SetPhasePWM;
//}

//uint8_t Commutation_GetHallState(COMMUTATION_T * commutation)
//{
//	return commutation->GetHallState();
//}
