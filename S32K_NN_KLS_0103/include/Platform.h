/*
 * Platform.h
 *
 *  Created on: 2018年8月1日
 *      Author: Administrator
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "clocks_and_modes.h"
#include "s32_core_cm4.h"
#include "typedefine.h"
#include "Function.h"
#include "Events.h"
#include "Variable.h"

/*********************************************************
 * 											RAM FLASH配置
 *********************************************************/
#define CAL_DATA_FLASH_ADDRESS              0x00002000
#define CAL_DATA_SIZE                       512

#define CAL_RAM_SIZE												1024

#define CODE_START_ADDRESS                  0x00003000
#define CODE_END_ADDRESS                    0x00020000

#define RAM_START_ADDRESS										0x1FFFE000
#define RAM_SIZE														0x4800

#define SP_START														0x20002800
#define SP_END															0x20003000

#define RESET_COUNT                         (*(uint8_t *)(0x20002BFF))//force read and write to a address
#define PCB_TEMP_SV                         (*( int8_t *)(0x200027FF))//force read and write to a address

#define POR_RST		(RCM->SRS & 0x80)

#define dFosc       2500000   //Clock frequency (Hz)=80000000/FRE_DIVIDER
#define FRE_DIVIDER   32

//(1/dFosc)*FRE_DIVIDER*H_SPD_PERIOD=(1/30)*16*875us=466us
#define H_SPD_PERIOD  1411 //622us //875//466us,为编码器的一个脉冲周期 //1750//933us//

/*********************************************************************************
                             端口定义
*********************************************************************************/
#define FOOT_SW       PTC_PDIR_P17//FOOT_IN
#define BRK_SW				PTC_PDIR_P14//BRK_IN

#define	REV_SW				PTC_PDIR_P15// REV_IN
#define	FWD_SW				PTC_PDIR_P0// FWD_IN
#define	ECO_IN				PTD_PDIR_P7//ECO_IN
#define	COIL_OUT			PTD_PDOR_P5
#define	MTR_OUT			  PTD_PDOR_P6
#define	ALM_OUT			  PTC_PDOR_P1

#define	HALL_SA1				PTE_PDIR_P4
#define	HALL_SB1				PTE_PDIR_P5
#define	HALL_SC1				PTE_PDIR_P10
#define HALL_SD1				PTE_PDIR_P11
#define HALL_PORT1		  ( ((PTE_PDIR >> 4) & 0x03) | ((PTE_PDIR >> 8) & 0x04) )

#define PWM_PORT 					(*(uint32_t *)(0x400FF100u))
#define PWM1_EN					(PWM_PORT | 0x00000007)
#define PWM1_DI					(PWM_PORT & 0xFFFFFFF8)

/*
#define PWM1_A					0x40038038u
#define PWM1_B					0x40038040u
#define PWM1_C					0x40038048u

#define EN1_CHA					0x00000001
#define EN1_CHB					0x00000002
#define EN1_CHC					0x00000004
*/
#define PWM1_A					0x40038038u
#define PWM1_C					0x40038040u
#define PWM1_B					0x40038048u

#define EN1_CHA					0x00000001
#define EN1_CHC					0x00000002
#define EN1_CHB					0x00000004
/*********************************************************************************
                             AD采样
*********************************************************************************/
#define ADx_AD8_Coeff  2       //ADx与8位AD之间的移位位数
#define IAD_TO_Q15     5			 //电流ADx到Q15的移位位数

#define AD_5V         1023
#define AD_MID        512
#define AD_3V         614
#define AD_1V         205
#define AD_0Point5V   102
#define AD_4Point5V   921

#define OUTMAX_Q14					16384

#define ADC_TEMP_AD        			0
#define ADC_R1_AD          		1
#define ADC_V_AD          	  2
#define ADC_R3_AD          		3
#define ADC_TPS1_AD           	4
#define ADC_VS_AD            	5
#define ADC_MOT1_AD         		5
#define ADC_BRK1_AD           	7
#define ADC_B_AD        			8
#define ADC_IA1_AD          		9
#define ADC_IC1_AD          	  10
#define ADC_IB1_AD          		11
#define ADC_VA1_AD           		12
#define ADC_VC1_AD           	13
#define ADC_VB1_AD            	14
#define ADC_HS_TEMP_AD           	15

void Memory_Init(void);
void MCU_Init(void);
void IO_Init(void);
void Reset_Handle(void);
void POR_Status_Detect(void);
void PWM_Output(uint32_t Channel,uint32_t Duty);
void Cpu_Delay100US(void);


#endif /* PLATFORM_H_ */
