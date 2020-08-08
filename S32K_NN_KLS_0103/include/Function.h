/*
 * Function.h
 *
 *  Created on: 2017年8月21日
 *      Author: ted
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

/* Including shared modules, which are used for whole project */
#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"
#include "FOC.h"

/***********************************************************************
                           功能选择
**********************************************************************/
#define SOFT_VER_H                          0x01
#define SOFT_VER_L                          0x01//03

#define With_CANBUS                         0//0 不带CAN口，1 带CAN口

#define KELLY_CAN														1//0，英博尔CAN协议; 1，凯利CAN协议；

#define EBIKE																1//0  电动汽车，1 电摩

#define  BRK_ON      											0//0 BRK_SW低电平有效，1 BRK_SW高电平有效
#define  BRK_OFF     											1//1 BRK_SW低电平有效，0 BRK_SW高电平有效

#define  FOOT_ON														1//0 FOOT_SW低电平有效，1 FOOT_SW高电平有效
#define  FOOT_OFF     											0//1 FOOT_SW低电平有效，0 FOOT_SW高电平有效

/***********************************************************************
                           输出参数
**********************************************************************/
#define OPEN_LOOP               0
#define SPEED_MODE              1
#define TORQUE_MODE             2

#define NO_SPEED_SENSOR         0  //无速度传感器
#define INC_ENCODER             1   //增量编码器
#define HALL_SENSOR             2   //hall
#define RESOLVER                3   //旋转变压器
#define LINE_HALL								4		//线性霍尔

#define SPEED_LOW               0     //低速模式
#define TRANSITION              1     //切换模式
#define SPEED_HIGH              2     //高速模式

#define fmax_Encoder      20000    //控制器能识别的最大编码器脉冲信号频率,10kHz
#define nmax_Firmware     8000     //固件最大电机转速,用于确定转速定标基准值,8000RPM
#define AD2S_Vel_Max			30000//29297		 //AD2S1210最大跟踪速率eRPM=60*500*8000/8192
																														//500eRPS	8M晶振  8.192M晶振
#define ANGLE_0_Q24         0
#define ANGLE_30_Q24				1398101
#define ANGLE_60_Q24        2796202
#define ANGLE_90_Q24        4194304
#define ANGLE_120_Q24       5592405
#define ANGLE_150_Q24       6990506
#define ANGLE_180_Q24       8388608
#define ANGLE_210_Q24       9786709
#define ANGLE_240_Q24       11184810
#define ANGLE_270_Q24       12582912
#define ANGLE_300_Q24       13981013
#define ANGLE_330_Q24       15379114

#define ANGLE_60_Q				10922

#define ERASE_FLASH_A_PAGE                  0
#define WRITE_FLASH_A_BYTE                  1
#define WRITE_FLASH_BURST                   2

 #define Q15(X) \
   ((X < 0.0) ? (int)(32768*(X) - 0.5) : (int)(32767*(X) + 0.5))

 #define Q13(X) \
   ((X < 0.0) ? (int)(8192*(X) - 0.5) : (int)(8191*(X) + 0.5))

#define 	 PI_Q6  	201                       //Q6(pi)
#define    Sq3OV2    28377    // √3  / 2 Q15
#define    OneBySq3    18919    // 1/√3 Q15
#define    TwoBySq3    37838    // 1/√3 Q15

#define		Low_RPM			500
#define		Mid_RPM			1000
#define		High_RPM		2000
/***********************************************************************
                          标志
**********************************************************************/
#define BRAKE_SW_WITH_BRAKE_AD              1
#define BRAKE_SW_WITHOUT_BRAKE_AD           2
#define BRAKE_AD_WITHOUT_BRAKE_SW           3
#define NO_BRAKE_AD_SW                      4

#define STARTUP_POWERON                     1
#define STARTUP_NORMALLY                    2
#define STARTUP_OTHER                       3

#define LED_START                           0x00
#define LED_HIGH_BLINKING                   0x20
#define LED_MIDDLE                          0x30
#define LED_LOW_BLINKING                    0x40

#define ALARM_START                           0x00
#define ALARM_HIGH_BLINKING                   0x20
#define ALARM_MIDDLE                          0x30
#define ALARM_LOW_BLINKING                    0x40

#define TPS0_5V                             1
#define TPS1_4V                             2
#define TPS0_5K                             3

#define ENABLE_AB							1
#define ENABLE_AC							2
#define ENABLE_BC							3
#define ENABLE_ABC						4

#define IDLE_STATUS           0
#define SINGLE_PHASE_DC       1
#define SINGLE_PHASE_AC_LF    2
#define SINGLE_PHASE_AC_HF    3
#define THREE_PHASES_AC       4

#define RESOLVER_POSITION	    5
#define HALL_POSITION		      6

#define EXP_NEVER_TODO        0
#define EXP_DOING             1
#define EXP_FINISHED          2

#define EXP_TIMES  4  //实验次数,必须大于2
#define SAMPLE_POINTS         128

#define TPS_MAX_SAMPLING                    32//Must be 4 multiples

#define OVERFLOW_MAX  4

#define  LIGHT_OFF     0
#define  LIGHT_ON      1

#define MAX(A,B)                            ((A)>=(B)?(A):(B))
#define MIN(A,B)                            ((A)<=(B)?(A):(B))

#define  ON                 1
#define  OFF                0

#define  OPEN_LEVEL      		1
#define  CLOSE_LEVEL     		0

#define TRUE								1
#define FALSE								0

#define X_POINTS		  24

#define SET_32(RegName, val)                (*(uint32_t *)(RegName)) = (uint32_t)(val)
#define SET_16(RegName, val)                (*(uint16_t *)(RegName)) = (uint16_t)(val)
#define SET_8(RegName, val)                 (*(uint8_t *)(RegName)) = (uint8_t)(val)

#define GET_32(RegName)                     (*(uint32_t *)(RegName))
#define GET_16(RegName)                     (*(uint16_t *)(RegName))
#define GET_8(RegName)                      (*(uint8_t *)(RegName))

/***********************************************************************
                           标定区数据宏定义
**********************************************************************/
/******************标定区0~127控制器、车辆参数*********************/
/*************控制器参数0~86****************/
/*型号及版本0~19*/
#define INFO_MODULE_NAME		                  0

#define INFO_SPECIAL_VERSION_1                8
#define INFO_SPECIAL_VERSION_2                9
#define INFO_SPECIAL_VERSION_3                10
#define INFO_SPECIAL_VERSION_4                11

#define INFO_SERIAL_NUMBER			              12
#define INFO_SOFTWARE_VER			                16

/*控制器其他参数20~86*/
#define CAL_APPLICATION_CONFIG			          20//20,21,22
#define CAL_STARTUP_WAIT_TIME			            23

#define INFO_CONTROLLER_VOL_H			            24
#define INFO_CONTROLLER_VOL_L               	25
#define CAL_LOW_VOLTAGE_H                     26
#define CAL_LOW_VOLTAGE_L                     27
#define CAL_OVER_VOLTAGE_H                    28
#define CAL_OVER_VOLTAGE_L                    29
//#define 预充电电压H															30
//#define 预充电电压L															31

//高压弱化电流	32~35

#define INFO_SENSOR_RATED_I_H               	36
#define INFO_SENSOR_RATED_I_L               	37
#define INFO_SENSOR_RATED_AD_H               	38
#define INFO_SENSOR_RATED_AD_L               	39
#define INFO_PHASE_A1_ZERO_CURRENT_H      	    40
#define INFO_PHASE_A1_ZERO_CURRENT_L      	    41
#define INFO_PHASE_B1_ZERO_CURRENT_H      	    42
#define INFO_PHASE_B1_ZERO_CURRENT_L      	    43
#define INFO_PHASE_C1_ZERO_CURRENT_H      	    44
#define INFO_PHASE_C1_ZERO_CURRENT_L      	    45
#define INFO_PHASE_A2_ZERO_CURRENT_H      	    46
#define INFO_PHASE_A2_ZERO_CURRENT_L      	    47
#define INFO_PHASE_B2_ZERO_CURRENT_H      	    48
#define INFO_PHASE_B2_ZERO_CURRENT_L      	    49
#define INFO_PHASE_C2_ZERO_CURRENT_H      	    50
#define INFO_PHASE_C2_ZERO_CURRENT_L      	    51


#define CAL_CURRENT_PERCENT		              	52
#define CAL_MAX_CURRENT_PERCENTAGE            53
#define CAL_LV_WEAK_MAX_CURRENT_PERCENTAGE    54
//#define 保留 55


#define INFO_IDENTIFY                   	    56

/*温度保护参数*/
#define INFO_CONTROLLER_SHUTDOWN_TEMP         57
#define INFO_CONTROLLER_SHUTDOWN_RESUME_TEMP  58
#define INFO_HIGH_WEKEN_TEMP                	59
#define INFO_HIGH_TEMP_WEAK_PERCENT         	60
#define INFO_OUTPUT_WEAKEN_TEMP               61
#define INFO_OUTPUT_WEAK_PERCENT           	  62
#define INFO_HIGH_MEASURE_TEMP             	  63
#define INFO_HIGH_MEASURE_TEMP_PERCENT     	  64
#define INFO_LOW_MEASURE_TEMP              	  65
#define INFO_LOW_MEASURE_TEMP_PERCENT      	  66
#define INFO_TEMPERATURE_TRIM              	  67
#define INFO_WENKEN_START_TEMP              	68
#define INFO_HL_TEMP_TRIM               	69//温度基准为50
#define INFO_HL_SHUTDOWN_TEMP           	70//高低端高温关断值120C
#define INFO_HL_HIGH_TEMP_WEAK          	71//高低端高温弱化值100C
#define INFO_HL_HIGH_TEMP_WEAK_PERCENT  	72//光耦隔离KDH的高低端高温弱化比例
#define INFO_HL_RESUME_TEMP             	73

#define CAL_LV_REVISE_PCB_L_TEMP						74
#define CAL_LV_REVISE_PCB_H_TEMP            75
#define CAL_LV_REVISE_PERCENT						    76

#define CAL_HL_REVISE_PCB_L_TEMP						77
#define CAL_HL_REVISE_PCB_H_TEMP            78
#define INFO_HL_REVISE_WEAK_L_TEMP					79

//风扇控制温度80~81

#define CAL_WHEEL_RADIUS_H										82//车轮半径mm
#define CAL_WHEEL_RADIUS_L										83
#define CAL_GEAR_RATIO_H                      84//减速比*100
#define CAL_GEAR_RATIO_L                      85//

#define INFO_BOOTLOADER                    	  86

//保留87~126

#define CAL_PWM_FREQ	                        127

/**********************标定区128~255控制参数*****************/
#define CAL_CONTROL_MODE                    	128
#define CAL_TPS_LOWERR_PERCENT                129
#define CAL_TPS_HIGHERR_PERCENT               130
#define CAL_TPS_TYPE                        	131
#define CAL_TPS_DEAD_ZONE_LOW               	132
#define CAL_TPS_DEAD_ZONE_HIGH              	133
#define CAL_TPS_MAP_FWD			                  134
#define CAL_TPS_MAP_REV				              	135
#define CAL_BRAKE_TYPE                      	136      //保留
#define CAL_BRAKE_DEAD_ZONE_LOW             	137      //保留
#define CAL_BRAKE_DEAD_ZONE_HIGH            	138      //保留
#define CAL_BRK_MAP				                    139

#define INFO_MAX_OUTPUT_F_H                   140
#define INFO_MAX_OUTPUT_F_L                   141
#define CAL_MAX_MOTOR_RPM_H                   142
#define CAL_MAX_MOTOR_RPM_L                   143
#define CAL_FW_SPEED_LIMIT				            144
#define CAL_REV_SPEED_LIMIT                   145
#define CAL_FWD_SM_LIMIT				            	146
#define CAL_REV_SM_LIMIT                   	 	147
#define CAL_FWD_SL_LIMIT				            	148
#define CAL_REV_SL_LIMIT                   	 	149
#define CAL_T_COMP_PERCENT                    150
//151
#define CAL_T_ASR_KP_Q13_H                    152
#define CAL_T_ASR_KP_Q13_L                    153
#define CAL_T_ASR_KI_Q13_H                    154
#define CAL_T_ASR_KI_Q13_L                    155
#define CAL_T_ASR_KD_Q13_H                    156
#define CAL_T_ASR_KD_Q13_L                    157
#define CAL_T_ASR_ERROR_Q15_MAX_H             158
#define CAL_T_ASR_ERROR_Q15_MAX_L             159

#define CAL_T_IVT_BRK_MINRPM_H                160
#define CAL_T_IVT_BRK_MINRPM_L                161
#define CAL_MIN_BRK_RPM_H					            162
#define CAL_MIN_BRK_RPM_L					            163

#define CAL_T_BRK_AD_PERCENT                  164
#define CAL_T_BRK_SW_PERCENT                  165
#define CAL_T_RLSTPS_BRK_PERCENT					    166
#define CAL_T_NTL_BRK_PERCENT					        167
#define CAL_T_IVT_BRK_PERCENT                 168

//169~174,SPEED_MODE
#define CAL_T_ACCEL_TIME			                175
#define CAL_T_ACC_RLS_TIME  	                176
#define CAL_T_BRAKE_TIME			                177
#define CAL_T_BRAKE_RLS_TIME			            178

#define CAL_ACQR_KP_Q13_H                     180
#define CAL_ACQR_KP_Q13_L                     181
#define CAL_ACQR_KI_Q13_H                     182
#define CAL_ACQR_KI_Q13_L                     183
#define INFO_ACQR_OUT_MAX_Q15_H               184
#define INFO_ACQR_OUT_MAX_Q15_L               185

#define CAL_ACDR_KP_Q13_H                     186
#define CAL_ACDR_KP_Q13_L                     187
#define CAL_ACDR_KI_Q13_H                     188
#define CAL_ACDR_KI_Q13_L                     189
#define INFO_ACDR_OUT_MAX_Q15_H               190
#define INFO_ACDR_OUT_MAX_Q15_L               191

#define CAL_AUR_KP_Q13_H                      192
#define CAL_AUR_KP_Q13_L                      193
#define CAL_AUR_KI_Q13_H                      194
#define CAL_AUR_KI_Q13_L                      195
#define CAL_AUR_ERROR_MAX_Q15_H               196
#define CAL_AUR_ERROR_MAX_Q15_L               197

#define CAL_TRANSITION_UP_RPM_H								198
#define CAL_TRANSITION_UP_RPM_L								199
#define CAL_TRANSITION_DOWN_RPM_H							200
#define CAL_TRANSITION_DOWN_RPM_L							201


#define CAL_HS_ACQR_KP_Q13_H                     202
#define CAL_HS_ACQR_KP_Q13_L                     203
#define CAL_HS_ACQR_KI_Q13_H                     204
#define CAL_HS_ACQR_KI_Q13_L                     205

#define CAL_HS_ACDR_KP_Q13_H                     206
#define CAL_HS_ACDR_KP_Q13_L                     207
#define CAL_HS_ACDR_KI_Q13_H                     208
#define CAL_HS_ACDR_KI_Q13_L                     209

/**************标定区384~511电机参数**********************/
#define CAL_NOM_VOLTAGE_H											384
#define CAL_NOM_VOLTAGE_L                     385
#define CAL_NOM_CURRENT_H                     386
#define CAL_NOM_CURRENT_L                     387
#define CAL_NOM_FREQUENCY_H                   388
#define CAL_NOM_FREQUENCY_L                   389
#define CAL_NOM_MOTOR_RPM_H                   390
#define CAL_NOM_MOTOR_RPM_L                   391
#define CAL_NAME_PLATE_MAX_F_H                392
#define CAL_NAME_PLATE_MAX_F_L                393
#define CAL_NAME_PLATE_MAX_RPM_H              394
#define CAL_NAME_PLATE_MAX_RPM_L              395

#define CAL_PARMS1_Q15_H                         396
#define CAL_PARMS1_Q15_L                         397
#define CAL_PARMS2_Q15_H                         398
#define CAL_PARMS2_Q15_L                         399
#define CAL_PARMS3_Q30_H                         400
#define CAL_PARMS3_Q30_L                         401
#define CAL_PARMS4_Q30_H                         402
#define CAL_PARMS4_Q30_L                         403

#define CAL_MOTOR_POLES                     	404
#define CAL_HIGH_TEMP_EN                    	405
#define CAL_HIGH_TEMP_CUT                   	406
#define CAL_HIGH_TEMP_RESUME                	407
#define CAL_HIGH_TEMP_WEAK_START				      408
#define CAL_HIGH_TEMP_WEAK_PERCENT					  409

#define CAL_SPEED_SENSOR_TYPE               	410
#define CAL_ENCODER_PULSES_H                	411
#define CAL_ENCODER_PULSES_L                	412
#define CAN_ENCODER_DIV												413

#define CAL_RESOLVER_SWAP_PHASES              414
#define CAL_RESOLVER_POLES										415
#define CAL_RESOLVER_INIT_ANGLE_H             416
#define CAL_RESOLVER_INIT_ANGLE_L             417

#define CAL_LINEHALL_ZERO_H              418
#define CAL_LINEHALL_ZERO_L              419
#define CAL_LINEHALL_AMP_H               420
#define CAL_LINEHALL_AMP_L               421
#define CAL_LINEHALL_MAX_H               422
#define CAL_LINEHALL_MAX_L               423
#define CAL_LINEHALL_MIN_H               424
#define CAL_LINEHALL_MIN_L               425

#define CAL_SBC_Angle_0                       426
#define CAL_SBA_Angle_60                      427
#define CAL_SCA_Angle_120                     428
#define CAL_SCB_Angle_180                     429
#define CAL_SAB_Angle_240                     430
#define CAL_SAC_Angle_300                     431

#define CAL_FWD_HallA_H                       432
#define CAL_FWD_HallA_L                       433
#define CAL_REV_HallA_H                       434
#define CAL_REV_HallA_L                       435
#define CAL_HALL_ANGLE_OFFSET                 436

/*********************************************************************************
                           结构体
*********************************************************************************/
//faults'struct
typedef volatile union
   {
   uint16_t str;
struct {
		uint16_t   E0         :1;  // 0
		uint16_t   E1         :1;  // 1
		uint16_t   E2         :1;  // 2
		uint16_t   E3         :1;  // 3
		uint16_t   E4         :1;  // 4
		uint16_t   E5         :1;  // 5
		uint16_t   E6         :1;  // 6
		uint16_t   E7         :1;  // 7
    uint16_t   E8         :1;  // 8
    uint16_t   E9         :1;  // 9
    uint16_t   E10        :1;  // 10
    uint16_t   E11        :1;  // 11
    uint16_t   E12        :1;  // 12
    uint16_t   E13        :1;  // 13
    uint16_t   E14        :1;  // 14
    uint16_t   E15        :1;  // 15

   }Bit_STR;
} Error_STR;

typedef volatile union
   {
   uint16_t str;
struct {
		uint16_t   E0         :1;  // 0
		uint16_t   E1         :1;  // 1
		uint16_t   E2         :1;  // 2
		uint16_t   E3         :1;  // 3
		uint16_t   E4         :1;  // 4
		uint16_t   E5         :1;  // 5
		uint16_t   E6         :1;  // 6
		uint16_t   E7         :1;  // 7
   }Bit_STR;
} Error_STR8;


#define LowVolt_Err        		Error_Struct0.Bit_STR.E0
#define OverVolt_Err    			Error_Struct0.Bit_STR.E1
#define V14_Err     					Error_Struct0.Bit_STR.E2
#define Startup_TPS_Err     	Error_Struct0.Bit_STR.E3
#define TPS_Error         		Error_Struct0.Bit_STR.E4
#define ILOP_Error        		Error_Struct0.Bit_STR.E5
#define OverTemp 							Error_Struct0.Bit_STR.E6
#define SV14_Err 						Error_Struct0.Bit_STR.E7

#define Identify_Err1        	Error_Struct1.Bit_STR.E0
#define StandStill_Err1    		Error_Struct1.Bit_STR.E1
#define MOS_OverTemp1     		Error_Struct1.Bit_STR.E2
#define Motor_Temp_Err1     	Error_Struct1.Bit_STR.E3
#define Pos_Sensor_Err1       Error_Struct1.Bit_STR.E4
#define Cur_Sensor_Err1       Error_Struct1.Bit_STR.E5
#define E16_Error 						Error_Struct1.Bit_STR.E6
#define E17_Error 						Error_Struct1.Bit_STR.E7


#define Alarm_Startup_TPS_Err       	Alarm_Err.Bit_STR.E1
#define Alarm_RS0           		 			Alarm_Err.Bit_STR.E2
#define Alarm_ILOP_Error         			Alarm_Err.Bit_STR.E3
#define Alarm_OverTemp             		Alarm_Err.Bit_STR.E4
#define Alarm_Cur_Sensor_Err          Alarm_Err.Bit_STR.E6
#define Alarm_Encoder_Err       			Alarm_Err.Bit_STR.E7
#define Alarm_BMS_Err       			    Alarm_Err.Bit_STR.E8
#define Alarm_LowVolt_Err       			Alarm_Err.Bit_STR.E9
#define Alarm_OverVolt_Err        		Alarm_Err.Bit_STR.E10
#define Alarm_Motor_Temp_Err     			Alarm_Err.Bit_STR.E11
#define Alarm_TPS_Error        				Alarm_Err.Bit_STR.E13

//任务调度
#define NO_TASK_PENDING           0x00
#define TASKS_1_MS                0x01
#define TASKS_100_MS              0x02
#define TASKS_2_MS_A              0x03
#define TASKS_50_MS               0x04
#define TASKS_2_MS_B              0x05
#define TASKS_10_MS               0x06

#define TASK_SCHEDULER_OK               0x00
#define TASK_SCHEDULER_OVERLOAD_1MS     0x01
#define TASK_SCHEDULER_OVERLOAD_2MS_A   0x02
#define TASK_SCHEDULER_OVERLOAD_2MS_B   0x03

void Task_1ms(void);
void Task_2msA(void);
void Task_2msB(void);
void Task_10ms(void);
void Task_50ms(void);
void Task_100ms(void);

#define  EXECUTE_1MS_TASKS()   {Task_1ms();}
#define  EXECUTE_100MS_TASKS() {Task_100ms();}
#define  EXECUTE_2MS_A_TASKS() {Task_2msA();}
#define  EXECUTE_50MS_TASKS()  {}
#define  EXECUTE_2MS_B_TASKS() {Task_2msB();}
#define  EXECUTE_10MS_TASKS()  {}

/*********************************************************************************
                            函数申明
*********************************************************************************/
void Main_Function(void);

void ETS_Action(void);
void Call_ETS(void);

void OpenFlash(void);
void CloseFlash(void);

void Task_Scheduler(void) ;
void Task_500US(void);

void Function_Init(void);
void Current_Parms_Init(void);

void CloseAll(void);

void Pedal_Parms_Init(void);
uint16_t Acc_Percent(uint16_t Tps_AD_Value);
void Clear_TPS_Array(void);
uint16_t Brake_Percent(uint16_t Brk_AD_Value);
void Releas_Brake_Disable_HP(void);

void RPM_Parms_Init(void);

void Temp_Parms_Init(void);
void Temperature_Weaking(void);
void Motor_Temp_Handle(void);
int8_t Get_Temp(uint8_t Count);
uint8_t Get_HL_Temp(uint8_t Count);
int16_t Get_KTY84_Temp(uint16_t Count);
int16_t Get_KTY83_Temp(uint16_t Count);

void Vol_Parms_Init(void);
void Voltage_Weaking(void);
void Get_B_Voltage(void);
uint16_t Get_Voltage(uint16_t Voltage_AD);
uint16_t Get_Voltage_AD(uint16_t Voltage);
void Voltage_Monitor(void);

void LED_Process(void);
void Alarm_Process(void);

uint8_t Calculate_Percent(uint8_t a,uint8_t b,uint8_t c);
uint16_t sqrt_16(uint32_t M);
int32_t PT_Function(int32_t input,int32_t *Int,uint32_t times);
uint16_t u8SearchNear(uint16_t u16Input_Data,uint16_t *u16Data_Array,uint16_t u8Size_of_Array);
uint16_t u8SearchSides(uint16_t u16Input_Data,uint16_t *u16Data_Array,uint16_t u8Size_of_Array);

APP_SECTION void Control(void);
APP_SECTION void PID_Parms_Init(void);
APP_SECTION void Motor_Parms_Init(void);
APP_SECTION void Rev_Status_Check(void);
APP_SECTION void Calc_TqeMd_Ref(void);
APP_SECTION void Calc_TqeMd_Tgt(void);

#define ATD_CHAN				16

void Init_IO(void);
void Init_LPIT0 (void);
void Init_ADC_SwTrig(void);

void Init_ADC0_HwTrig (void);
void Init_ADC1_HwTrig (void);

uint32_t ADC0_SwTrigConvert (uint32_t Ch);
uint32_t ADC1_SwTrigConvert (uint32_t Ch);

void ADC1_HwTrigConvert(void);
void Init_PDB(void);

void Init_Cap_Period (FTM_Type * FTM,uint32_t PCC_INDEX,uint32_t CHn);
void Init_QD_Mode (FTM_Type * FTM,uint32_t PCC_INDEX,uint32_t Pulses,uint32_t Swap);
void Init_Hall_Cap (FTM_Type * FTM,uint32_t PCC_INDEX);
void Init_PWM (FTM_Type * FTM, uint32_t PCC_INDEX,uint32_t Modul_Hz);
void Start_PWM (FTM_Type * FTM);

void LPUART0_init(void);
uint8_t LPUART0_transmit_char(uint8_t Chr);
uint8_t LPUART0_receive_char(uint8_t *Chr);
void LPUART1_init(void);
uint8_t LPUART1_transmit_char(uint8_t Chr);
uint8_t LPUART1_receive_char(uint8_t *Chr);



#endif /* FUNCTION_H_ */
