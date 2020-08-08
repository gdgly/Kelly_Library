/*
 * Motor.h
 *
 *  Created on: 2020Äê2ÔÂ1ÈÕ
 *      Author: jt
 */

#ifndef MOTOR_H_
#define MOTOR_H_

/* Including shared modules, which are used for whole project */
#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"
#include "FOC.h"
#include "Variable.h"

void IRSample_Coff(void);

void ISR_HALL1_AB(void);
void ISR_HALL1_C(void);
void ISR_FTM2_Ovf(void);

uint8_t Hall_Sensor_Init(HALL_STR *pHall,int32_t Swap_Phase,uint32_t BaseAddress);
void Hall_Sensor_Err(void);
void Hall_Angle_Acc(HALL_STR *pHall,uint32_t Hall_Value,int32_t Current_Mode);
void Hall_Angle_Parms_Init(void);
void Hall_Correct_Angle(HALL_STR *pHall,uint32_t Hall_Value,int32_t Current_Mode);
void Hall_Cap_Calc(FTM_Type * FTM,HALL_STR *pHall,MOTOR_STR *pMotor,uint32_t Hall_Value);

void FdWeakening(void);
void AcquirePosition(void);
void Angle_Sensor_Err(void);
uint16_t Pre_Drive(uint32_t qAngle,uint8_t order,int16_t Target_Ia);
void IdentifyMotorParms(void);
void Motor_Init(void);
void InitPI(tPIParm *pParm);
void CalcPI(tPIParm *pParm);
int16_t Torque_Buffer_Function(TORGUE_STR *Torque,uint16_t Bld_Rate,uint16_t Rls_Rate);
void CalcEMF(PSC_STR *pCtrl,OBSERVER_STR *pObser,MOTOR_STR *pMotor);
void Calculate_Speed(uint32_t Cur_Speed, MOTOR_STR *pMotor,OBSERVER_STR *pObser);

#endif /* MOTOR_H_ */
