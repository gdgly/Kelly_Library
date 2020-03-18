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

#define INVERSE_HALL(hall) (~hall&0x07)

typedef enum
{
	DIRECTION_CW,	/* Clockwise direction */
	DIRECTION_CCW,	/* Counter Clockwise direction */
} BLDC_DIRECTION_T;

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
	void (*SetPhasePWM)(uint8_t pwm);
	void (*ActivatePhase)(uint8_t pwm);
	//PhaseID?
} BLDC_COMMUTATION_PHASE_T;

typedef struct 
{
	volatile BLDC_DIRECTION_T Direction; 
	//PhaseID?
	uint8_t (*GetHallState)(void);	//HALL_SENSOR_T (*GetHallSensors)(void);	//CommuntationTableSelect[(uint8_t)hall](pwm);
	uint8_t SavedHallState;
	
	BLDC_COMMUTATION_PHASE_T * CommuntationTable;
	BLDC_COMMUTATION_PHASE_T CommuntationTableCW[8];
	BLDC_COMMUTATION_PHASE_T CommuntationTableCCW[8];

	//add in between steps improve resolution, reduce speed, for stepper mode drive
	//BLDC_COMMUTATION_PHASE_T CommuntationTableStepper[8][2];
	
	//union
//	{
//		struct 
//		{
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseAB;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseAC;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseBC;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseBA;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseCA;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseCB;
//		}
//		struct 
//		{
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseA;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseB;
		//	BLDC_COMMUTATION_PHASE_T CommutationPhaseC;
		//set combined phase functions in module, individual phases also need disable function
		//might need high low functions, if using individual FET control.
//		}
//	}
	
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseAB;
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseAC;
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseBC;
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseBA;
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseCA;
//	BLDC_COMMUTATION_PHASE_T CommutationPhaseCB;
	
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
BLDC_COMMUTATION_T;

inline void BLDC_Commutation_ISR(BLDC_COMMUTATION_T * commutation, uint8_t pwm)
{
	//	if (commutation->Direction == DIRECTION_CCW)
	//		~(commutation->GetHallSensors())&0x07
	commutation->CommuntationTable[commutation->GetHallState()].ActivatePhase(pwm);
}

bool BLDC_Commutation_Poll(BLDC_COMMUTATION_T * commutation, uint8_t pwm);
void (*BLDC_Commutation_GetFunctionActivatePhase(BLDC_COMMUTATION_T * commutation))(uint8_t);
void (*BLDC_Commutation_GetFunctionSetPhasePWM(BLDC_COMMUTATION_T * commutation))(uint8_t);
void BLDC_Commutation_SetDirection(BLDC_COMMUTATION_T * commutation, BLDC_DIRECTION_T dir);

void BLDC_Commutation_MapCommuntationTableRunCalibration
(	
	BLDC_COMMUTATION_T * commutation,
	uint8_t * returnIndexAB,
	uint8_t * returnIndexAC,
	uint8_t * returnIndexBC,
	uint8_t * returnIndexBA,
	uint8_t * returnIndexCA,
	uint8_t * returnIndexCB,
	void (*setPWMPhaseAB)(uint8_t),
	void (*setPWMPhaseAC)(uint8_t),
	void (*setPWMPhaseBC)(uint8_t),
	void (*setPWMPhaseBA)(uint8_t),
	void (*setPWMPhaseCA)(uint8_t),
	void (*setPWMPhaseCB)(uint8_t),
	uint16_t pwm,
	void (*commutatePhaseAB)(uint8_t),
	void (*commutatePhaseAC)(uint8_t),
	void (*commutatePhaseBC)(uint8_t),
	void (*commutatePhaseBA)(uint8_t),
	void (*commutatePhaseCA)(uint8_t),
	void (*commutatePhaseCB)(uint8_t),
	void (*enablePhaseABC)(void),
	void (*delay)(uint32_t),
	uint32_t delayTime
);

void BLDC_Commutation_MapCommuntationTable
(	
	BLDC_COMMUTATION_T * commutation,				
	void (*setPWMPhaseAB)(uint8_t),
	void (*setPWMPhaseAC)(uint8_t),
	void (*setPWMPhaseBC)(uint8_t),
	void (*setPWMPhaseBA)(uint8_t),
	void (*setPWMPhaseCA)(uint8_t),
	void (*setPWMPhaseCB)(uint8_t),
	void (*commutatePhaseAB)(uint8_t),
	void (*commutatePhaseAC)(uint8_t),
	void (*commutatePhaseBC)(uint8_t),
	void (*commutatePhaseBA)(uint8_t),
	void (*commutatePhaseCA)(uint8_t),
	void (*commutatePhaseCB)(uint8_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB
);

void BLDC_Commutation_Init
(
	BLDC_COMMUTATION_T * commutation,
	BLDC_DIRECTION_T dir,
	uint8_t (*getHallSensors)(void),
	void (*setPWMPhaseAB)(uint8_t),
	void (*setPWMPhaseAC)(uint8_t),
	void (*setPWMPhaseBC)(uint8_t),
	void (*setPWMPhaseBA)(uint8_t),
	void (*setPWMPhaseCA)(uint8_t),
	void (*setPWMPhaseCB)(uint8_t),
	void (*commutatePhaseAB)(uint8_t),
	void (*commutatePhaseAC)(uint8_t),
	void (*commutatePhaseBC)(uint8_t),
	void (*commutatePhaseBA)(uint8_t),
	void (*commutatePhaseCA)(uint8_t),
	void (*commutatePhaseCB)(uint8_t),
	uint8_t indexPhaseAB,
	uint8_t indexPhaseAC,
	uint8_t indexPhaseBC,
	uint8_t indexPhaseBA,
	uint8_t indexPhaseCA,
	uint8_t indexPhaseCB,
	void (*releasePWM)(void)
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
