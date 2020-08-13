/*
 * Variable.h
 *
 *  Created on: 2017年8月21日
 *      Author: ted
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_


#include "typedefine.h"
#include "Function.h"
#include "Platform.h"

extern const uint8_t Led_Blinking[16];

extern const int8_t Temperature_Array[32];

extern const signed char HL_Temp_Array[172];

extern const int16_t KTY84_Temp_Array[15];
extern const int16_t KTY83_Temp_Array[12];

extern const int32_t sinetable[];

extern const uint16_t X_Coords[X_POINTS];

extern const uint16_t IR_Coff[X_POINTS];

extern const uint8_t Default_Cal_data[CAL_DATA_SIZE];

extern const uint16_t Curve_Data[256];

extern uint8_t Cal_data[CAL_RAM_SIZE];//reserve for Calibration Data

/*********************************************************************************
                                 采样,计数,标志位
 *********************************************************************************/
extern volatile uint8_t NTL_Error,Reset_Err,Regen_OverVolt_Err,B_OverVolt_Err;

extern volatile uint8_t ATD_Value_u8[ATD_CHAN];

extern volatile uint16_t ATD_Value_u16[ATD_CHAN];

extern volatile uint16_t Temp_CF,Index_low,Index_up;

extern volatile uint8_t Wait_Timer;

extern uint8_t  Task500us_En;

extern volatile uint32_t test4,Timer_500us;

extern volatile uint8_t M_En,Global_En,Regen_En,Stop_Flag;

extern volatile uint8_t WithFoot,Releas_BRK_DisHP,Neutral_DisHP,PCB_En;

extern volatile uint8_t Rev_Mode,New_Rev_Mode,Neutral_Flag;

extern volatile uint8_t SW_ON,SW_OFF;

extern volatile int8_t Ref_Direction;

extern volatile uint8_t StandstillCount1;

/*********************************************************************************
                                     踏板计算
 *********************************************************************************/
extern volatile uint16_t TPSx_Output,Temp_T_TPSx,BRAKEx;

extern volatile uint8_t StartupStatus;

extern volatile uint16_t HS_Rated_I,Current_Max_AD,Current_Max_Q;

typedef struct{
    int16_t   Ref;
    int32_t   Cmd64;
    int32_t   Tgt64;
    int16_t   Tgt;

}TORGUE_STR;

extern  TORGUE_STR  TPSTorque;
extern  TORGUE_STR  BRKTorque;
/*********************************************************************************
                                      电压计算
 *********************************************************************************/
extern volatile uint16_t B_Voltage,B_Voltage_64;

extern volatile uint8_t LowVoltageAD,OverVoltageAD;

/*********************************************************************************
                                      温度弱化
 *********************************************************************************/
extern volatile uint8_t HL_Temp;

extern volatile int8_t PCB_Temp;

extern volatile int16_t Motor_Temp1;

extern volatile uint16_t Temp_OutMax;

/*********************************************************************************
                                       其他
 *********************************************************************************/
extern volatile Error_STR Alarm_Err;

extern volatile Error_STR8 Error_Struct0,Error_Struct1;

extern volatile uint8_t Error_Code;

extern volatile uint32_t Check_sum_Err;
/*********************************************************************************
 *                                                                               *
 *                                   矢量部分                                    *
 *                                                                               *
 *********************************************************************************/
typedef struct{
    int32_t   qdSum;
    int32_t   qKp;
    int32_t   qKi;
    int32_t		qKd;
    int32_t   qErrMax;
    int32_t		qLastErr;
    int32_t   qOutMax;
    int32_t   qOutMin;
    int32_t   qInRef;
    int32_t   qInMeas;
    int32_t   qOut;
    }tPIParm;

extern  tPIParm  PIParmQref1;
extern  tPIParm  PIParmQ1;
extern  tPIParm  PIParmD1;
extern  tPIParm  PIParmU;

typedef struct
{
	struct
	{
		 uint32_t  PWM_CH;
		 uint32_t  IAD_CH;
		 uint32_t  IAD_ZERO;
		 uint32_t  VAD_CH;
		 uint32_t  EN_CH;
	} PA;
	struct
	{
		 uint32_t  PWM_CH;
		 uint32_t  IAD_CH;
		 uint32_t  IAD_ZERO;
		 uint32_t  VAD_CH;
		 uint32_t  EN_CH;
	} PB;
	struct
	{
		 uint32_t  PWM_CH;
		 uint32_t  IAD_CH;
		 uint32_t  IAD_ZERO;
		 uint32_t  VAD_CH;
		 uint32_t  EN_CH;
	} PC;

} PSC_STR;

extern PSC_STR Ctrl_1;

typedef struct
{
	int32_t  Array[8];
	int32_t  Loop;
	int32_t  Tgt_RPM;
	int32_t  Mot_RPM;
	int32_t  Mot_AbsRPM;
	int32_t  RPM_Convert;
	int32_t  Cur_Mode;
	int32_t	 Cur_Dir;
} MOTOR_STR;

extern MOTOR_STR Motor1;

typedef struct
{
	int32_t EMF_qVq;
	int32_t EMF_qVq_Abs;
	uint32_t Start_Allow;
	int32_t  Mot_RPM;
}OBSERVER_STR;

extern OBSERVER_STR Obser1;

typedef struct
{
	uint32_t Init_qAngle[8];
	uint32_t Init_Dir[4];
	uint32_t Cap_Period;
	uint32_t Last_Period;
	uint32_t CurSpeed;
	uint32_t qAngle_Hall;
	uint32_t qAngle;
  uint32_t Delta;
  uint32_t Over;
	int32_t Sine_Mode;
}HALL_STR;

extern HALL_STR Hall_Str1;

extern volatile int16_t  Real_OutMax;

extern volatile uint16_t  Drive_OutMax,Regen_OutMax;

extern volatile int16_t Tr_MTemp1;

extern volatile int16_t Id_Min,Id_delta;

#endif /* VARIABLE_H_ */
