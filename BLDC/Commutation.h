/*
 * Commutation.h
 *
 *  Created on: Dec 1, 2019
 *      Author: FireSourcery
 */

#ifndef COMMUTATION_H_
#define COMMUTATION_H_

#include <stdbool.h>
#include <stdint.h>

#define INVERSE_HALL(hall) ((~hall)&0x07)

typedef enum
{
	DIRECTION_CW,	/* Clockwise direction */
	DIRECTION_CCW,	/* Counter Clockwise direction */
} COMMUTATION_DIRECTION_T;

//typedef enum
//{
//	PHASE_DISABLE = 0,
//	PHASE_AB,
//	PHASE_AC,
//	PHASE_BC,
//	PHASE_BA,
//	PHASE_CA,
//	PHASE_CB,
//} PHASE_ID_T;

typedef struct 
{
//	void (*SetPhasePWM)(uint16_t pwm);
	void (*ActivatePhase)(void * data, uint16_t pwm);
	//PhaseID?
} COMMUTATION_PHASE_T;

typedef struct 
{
	volatile COMMUTATION_DIRECTION_T Direction; 
	uint8_t (*GetHallState)(void);	//HALL_SENSOR_T (*GetHallSensors)(void);	//CommuntationTableSelect[(uint8_t)hall](pwm);
	uint8_t SavedHallState;
	
	//COMMUTATION_PHASE_T * CommuntationTable;
	//COMMUTATION_PHASE_T CommuntationTableCW[8];
	//COMMUTATION_PHASE_T CommuntationTableCCW[8];
	COMMUTATION_PHASE_T CommuntationTable[8];

	//add in between steps improve resolution, reduce speed, for stepper mode drive
	//BLDC_COMMUTATION_PHASE_T CommuntationTableStepper[8][2];

//	void (*CommuntationTable)(uint8_t);
//	void (*CommuntationTableCW[8])(uint8_t);
//	void (*CommuntationTableCCW[8])(uint8_t);
//	void (*CommutatePhaseAB)(uint8_t pwmPercent);
//	void (*CommutatePhaseAC)(uint8_t pwmPercent);	
//	void (*CommutatePhaseBA)(uint8_t pwmPercent);
//	void (*CommutatePhaseBC)(uint8_t pwmPercent);
//	void (*CommutatePhaseCA)(uint8_t pwmPercent);	
//	void (*CommutatePhaseCB)(uint8_t pwmPercent);
}
COMMUTATION_T;

static inline void Commutation_ISR(COMMUTATION_T * commutation, void * data, uint16_t pwm)
{
	if (commutation->Direction == DIRECTION_CCW)
		commutation->CommuntationTable[(~commutation->GetHallState())&0x07].ActivatePhase(data, pwm);
	else
		commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase(data, pwm);
}

//extern void Commutation_ISR(COMMUTATION_T * commutation, uint16_t pwm);
bool Commutation_Poll(COMMUTATION_T * commutation, void * data, uint16_t pwm);
//void Commutation_SetPhasePWM(COMMUTATION_T * commutation, uint16_t pwm);
//void (*BLDC_Commutation_GetFunctionActivatePhase(COMMUTATION_T * commutation))(uint16_t);
//void (*BLDC_Commutation_GetFunctionSetPhasePWM(COMMUTATION_T * commutation))(uint16_t);
void Commutation_SetDirection(COMMUTATION_T * commutation, COMMUTATION_DIRECTION_T dir);

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
	void (*commutatePhaseAB)(void *, uint16_t),
	void (*commutatePhaseAC)(void *, uint16_t),
	void (*commutatePhaseBC)(void *, uint16_t),
	void (*commutatePhaseBA)(void *, uint16_t),
	void (*commutatePhaseCA)(void *, uint16_t),
	void (*commutatePhaseCB)(void *, uint16_t),
	void (*setPWMPhaseABC)(uint16_t pwmA, uint16_t pwmB, uint16_t pwmC),
	uint16_t pwm,
	void (*enablePhaseABC)(bool enA, bool enB, bool enC),
	void (*delay)(uint32_t),
	uint32_t delayTime
);

void Commutation_MapCommuntationTable
(
	COMMUTATION_T * commutation,
//	void (*setPWMPhaseAB)(uint16_t),
//	void (*setPWMPhaseAC)(uint16_t),
//	void (*setPWMPhaseBC)(uint16_t),
//	void (*setPWMPhaseBA)(uint16_t),
//	void (*setPWMPhaseCA)(uint16_t),
//	void (*setPWMPhaseCB)(uint16_t),
	void (*commutatePhaseAB)(void *, uint16_t),
	void (*commutatePhaseAC)(void *, uint16_t),
	void (*commutatePhaseBC)(void *, uint16_t),
	void (*commutatePhaseBA)(void *, uint16_t),
	void (*commutatePhaseCA)(void *, uint16_t),
	void (*commutatePhaseCB)(void *, uint16_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB
);

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
	void (*commutatePhaseAB)(void *, uint16_t),
	void (*commutatePhaseAC)(void *, uint16_t),
	void (*commutatePhaseBC)(void *, uint16_t),
	void (*commutatePhaseBA)(void *, uint16_t),
	void (*commutatePhaseCA)(void *, uint16_t),
	void (*commutatePhaseCB)(void *, uint16_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB,
	void (*fault000)(void),
	void (*fault111)(void)
);

// cant do compile time type check in C
//typedef enum
//{
//	HALL_SENSOR_ORDER_ABC,
//	HALL_SENSOR_ORDER_ACB,
//	HALL_SENSOR_ORDER_BAC,
//	HALL_SENSOR_ORDER_BCA,
//	HALL_SENSOR_ORDER_CAB,
//	HALL_SENSOR_ORDER_CBA,
//} HALL_SENSOR_ORDER_T;

//#define HALL_SENSOR_ORDER_ABC 	1
//#define HALL_SENSOR_ORDER_ACB 	2
//#define HALL_SENSOR_ORDER_BAC 	3
//#define HALL_SENSOR_ORDER_BCA 	4
//#define HALL_SENSOR_ORDER_CAB 	5
//#define HALL_SENSOR_ORDER_CBA 	6
//
////#define HALL_SENSOR_ORDER HALL_SENSOR_ORDER_ABC
//
//#if (HALL_SENSOR_ORDER == HALL_SENSOR_ORDER_CBA)
//	typedef enum
//	{
//		//2, 6, 4, 5, 1, 3,
//		PHASE_DISABLE = 0,
//		PHASE_AB = 0b011, //3
//		PHASE_AC = 0b010, //2
//		PHASE_BC = 0b110, //6
//		PHASE_BA = 0b100, //4
//		PHASE_CA = 0b101, //5
//		PHASE_CB = 0b001, //1
//	} PHASE_ID_T;
//#elif (HALL_SENSOR_ORDER == HALL_SENSOR_ORDER_ABC)
//	typedef enum
//	{
//		PHASE_DISABLE = 0,
//		PHASE_AB = 0b110, //6
//		PHASE_AC = 0b010, //2
//		PHASE_BC = 0b011, //3
//		PHASE_BA = 0b001, //1
//		PHASE_CA = 0b101, //5
//		PHASE_CB = 0b100, //4
//	} PHASE_ID_T;
//#endif

#endif /* COMMUTATION_H_ */
