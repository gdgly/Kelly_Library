/*
 * Platform.c
 *
 *  Created on: 2018年8月1日
 *      Author: Administrator
 */


#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "s32_core_cm4.h"
#include "typedefine.h"
#include "Function.h"
#include "Platform.h"
#include "Variable.h"
#include "Motor.h"

/*==========================================================================
* 函数名:Memory_Init
* 功能描述:内存初始化
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Ted, 2014 年 7 月 28 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Memory_Init(void)
{
	uint32_t i=0;

	for(i=0;i<(RAM_SIZE/4)-1;i++)
	{
			(*(uint32_t *)(RAM_START_ADDRESS+ i*4)) = 0;
	}
	for(i=0;i<CAL_RAM_SIZE;i++)
	{
		Cal_data[i] = (*(uint32_t *)(CAL_DATA_FLASH_ADDRESS+ i));
	}

  if(Default_Cal_data[CAL_PWM_FREQ] == 10)
	{
		PWM1.dloop= 10000; //角度计算执行周期100us, 1s/100us=10000
		PWM1.Mod  = 4000;
		PWM1.PE70 = 3540;
		PWM1.Max  = 3920; //98%
		PWM1.Min  = 80;  //2%
		Timer_500us=5;
	}
  else
	{
		PWM1.dloop= 6250; //角度计算执行周期160us, 1s/160us=6250
		PWM1.Mod  = 6400;
		PWM1.PE70 = 5664;
		PWM1.Max  = 6272; //98%
		PWM1.Min  = 128;  //2%
		Timer_500us=3;
	}

	if((Default_Cal_data[CAL_APPLICATION_CONFIG+2]&0x40)!=0 && (Default_Cal_data[INFO_IDENTIFY] != 0xAA))//第1字节的第7位
	{
		Ctrl_1.PA.EN_CH=EN1_CHB;
		Ctrl_1.PB.EN_CH=EN1_CHA;
		Ctrl_1.PC.EN_CH=EN1_CHC;

		Ctrl_1.PA.PWM_CH=PWM1_B;
		Ctrl_1.PB.PWM_CH=PWM1_A;
		Ctrl_1.PC.PWM_CH=PWM1_C;

		Ctrl_1.PA.IAD_CH =ADC_IB1_AD;
		Ctrl_1.PB.IAD_CH =ADC_IA1_AD;
		Ctrl_1.PC.VAD_CH =ADC_IC1_AD;

		Ctrl_1.PA.VAD_CH =ADC_VB1_AD;
		Ctrl_1.PB.VAD_CH =ADC_VA1_AD;
		Ctrl_1.PC.VAD_CH =ADC_VC1_AD;

		Ctrl_1.PA.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
		Ctrl_1.PB.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
		Ctrl_1.PC.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_L]);

	}
	else
	{
		Ctrl_1.PA.EN_CH=EN1_CHA;
		Ctrl_1.PB.EN_CH=EN1_CHB;
		Ctrl_1.PC.EN_CH=EN1_CHC;

		Ctrl_1.PA.PWM_CH=PWM1_A;
		Ctrl_1.PB.PWM_CH=PWM1_B;
		Ctrl_1.PC.PWM_CH=PWM1_C;

		Ctrl_1.PA.IAD_CH =ADC_IA1_AD;
		Ctrl_1.PB.IAD_CH =ADC_IB1_AD;
		Ctrl_1.PC.IAD_CH =ADC_IC1_AD;

		Ctrl_1.PA.VAD_CH =ADC_VA1_AD;
		Ctrl_1.PB.VAD_CH =ADC_VB1_AD;
		Ctrl_1.PC.VAD_CH =ADC_VC1_AD;

		Ctrl_1.PA.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
		Ctrl_1.PB.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
		Ctrl_1.PC.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_L]);
	}
}

/*==========================================================================
* 函数名:MCU_Init
* 功能描述:MCU初始化
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Ted, 2018 年 7 月 28 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void MCU_Init(void)
{
	Init_IRQs(FTM0_Ovf_Reload_IRQn,1);
	Init_IRQs(FTM2_Ovf_Reload_IRQn,2);
	Init_IRQs(FTM2_Ch2_Ch3_IRQn,3);
	Init_IRQs(FTM2_Ch4_Ch5_IRQn,3);

	Init_IO();
	LPUART0_init();
	LPUART1_init();
	Init_PWM(FTM0,PCC_FTM0_INDEX,Default_Cal_data[CAL_PWM_FREQ]);
	Init_ADC_SwTrig();
	Init_ADC1_HwTrig();
	Init_PDB();
	Start_PWM (FTM0);//选择FTM系统时钟（80MHz）作为时钟源，启动FTM3

	Init_WDOG();
	WDOG_Feed();

	//j = SCI1_RecvChar(&i);//调用一次，防止编译器不调用导致不编译
	//j = SCI1_SendChar(0);

	//CAN1_Init();
}

/*==========================================================================
* 函数名:IO_Init
* 功能描述:端口初始化
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Ted, 2014 年 7 月 28 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Init_IO(void)
{
	PCC->PCCn[PCC_PORTA_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTA */
	PCC->PCCn[PCC_PORTB_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PCC->PCCn[PCC_PORTE_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */

	//SDCBA
	PORTE->PCR[11]=PORT_PCR_MUX(1);
	PTE_PDDR_P11 = 0;
	PORTE->PCR[10]=PORT_PCR_MUX(4);
	PTE_PDDR_P10 = 0;
	PORTE->PCR[5]=PORT_PCR_MUX(4);
	PTE_PDDR_P5 = 0;
	PORTE->PCR[4]=PORT_PCR_MUX(4);
	PTE_PDDR_P4 = 0;

	//PWMCBA
	PORTE->PCR[9]=PORT_PCR_MUX(2);
	PTE_PDDR_P9 = 1;
	PORTE->PCR[8]=PORT_PCR_MUX(2);
	PTE_PDDR_P8 = 1;
	PORTB->PCR[5]=PORT_PCR_MUX(2);
	PTB_PDDR_P5 = 1;

	//EN-CBA
	PORTE->PCR[2]=PORT_PCR_MUX(1);
	PTE_PDDR_P2 = 1;
	PORTE->PCR[1]=PORT_PCR_MUX(1);
	PTE_PDDR_P1 = 1;
	PORTE->PCR[0]=PORT_PCR_MUX(1);
	PTE_PDDR_P0 = 1;

	//UART0 TXRX
	PORTC->PCR[3]=PORT_PCR_MUX(4);
	PTC_PDDR_P3 = 1;
	PORTC->PCR[2]=PORT_PCR_MUX(4);
	PTC_PDDR_P2 = 0;

	//UART1 TXRX
	PORTC->PCR[9]=PORT_PCR_MUX(2);
	PTC_PDDR_P9 = 1;
	PORTC->PCR[8]=PORT_PCR_MUX(2);
	PTC_PDDR_P8 = 0;

	//CAN1 TXRX
	PORTA->PCR[13]=PORT_PCR_MUX(3);
	PTA_PDDR_P13 = 1;
	PORTA->PCR[12]=PORT_PCR_MUX(3);
	PTA_PDDR_P12 = 0;

	PORTD->PCR[7]=PORT_PCR_MUX(1);//ECO_IN
	PTD_PDDR_P7 = 0;
	PORTD->PCR[6]=PORT_PCR_MUX(1);//MTR_OUT
	PTD_PDDR_P6 = 1;
	PORTD->PCR[5]=PORT_PCR_MUX(1);//COIL_OUT
	PTD_PDDR_P5 = 1;
	PORTC->PCR[1]=PORT_PCR_MUX(1);//ALM_OUT
	PTC_PDDR_P1 = 1;
	PORTC->PCR[0]=PORT_PCR_MUX(1);//FWD_IN
	PTC_PDDR_P0 = 0;
	PORTC->PCR[17]=PORT_PCR_MUX(1);//FOOT_IN
	PTC_PDDR_P17 = 0;
	PORTC->PCR[15]=PORT_PCR_MUX(1);//REV_IN
	PTC_PDDR_P15 = 0;
	PORTC->PCR[14]=PORT_PCR_MUX(1);//BRK_IN
	PTC_PDDR_P14 = 0;

	COIL_OUT=TRUE;                //主接触器吸合
}
/*==========================================================================
* 函数名:Function_Init
* 功能描述:按标定数据配置控制器功能
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Icy, 2010 年 9 月 24 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Function_Init(void)
{
  if((Default_Cal_data[CAL_APPLICATION_CONFIG] & 0x02) != 0)//第0字节的第01位
  {
    Releas_BRK_DisHP = 1;
  }
  if((Default_Cal_data[CAL_APPLICATION_CONFIG] & 0x04) != 0)//第0字节的第01位
  {
    Neutral_DisHP = 1;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1]&0x01)!=0)//第1字节的第0位//usage Foot SW
  {
    WithFoot=1;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1]&0x02)==0)//第1字节的第1位//开关逻辑标定
  {
  //低电平有效
    SW_ON  = 0;
    SW_OFF = 1;
  }
  else  //高电平有效
  {
    SW_ON  = 1;
    SW_OFF = 0;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1] & 0x04) != 0)//第1字节的第2位//标定模式选择，0-正常运行模式，MTPA查表插值，1-标定调试模式，用于标定β表，非插值算法
  {
  	PCB_En = 1;
  }

  return;
}

/*==========================================================================
* 函数名:Reset_Handle
* 功能描述:复位检测与处理
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Ted, 2014 年 7 月 28 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Reset_Handle(void)
{
  uint8_t Reset_Status=0;

	if((RCM->SRS & 0x82) !=0 )//If Power on reset
  {
    RESET_COUNT=0;
    if((Default_Cal_data[CAL_APPLICATION_CONFIG]&0x01)==1)
    {
      StartupStatus=STARTUP_POWERON; //High Pedal Disable
    }
    else
    {
      StartupStatus=STARTUP_NORMALLY;//High Pedal Enable
    }
    Reset_Status = 0x33;
  }
  else if(((RCM->SRS & 0x2EE4)!=0))//other cause reset
  {
    if(RESET_COUNT<200)
      RESET_COUNT++;
    ILOP_Error = 1;
    Reset_Status = 0x99;
  }

	if(RESET_COUNT>=10)
	{
	  Reset_Err=1;
	  ILOP_Error = 1;
  }

	if(Reset_Status == 0x99)//非上电复位
	{
		PCB_Temp = PCB_TEMP_SV;
	}
	else
	{
		PCB_Temp = Get_Temp((uint8_t)(ATD_Value_u16[ADC_TEMP_AD] >> ADx_AD8_Coeff));
		PCB_TEMP_SV = PCB_Temp;
	}
}

/*==========================================================================
* 函数名:POR_Status_Detect
* 功能描述:上电检测速度，开关和电压状态
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Ted, 2014 年 7 月 28 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void POR_Status_Detect(void)
{
  /*int16_t Cal_I_Zero=0,ZeroErr=0;

	Cal_I_Zero = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
	ZeroErr = (int16_t)(Cal_I_Zero - ATD_Value_u16[ADC_IA1_AD]);
	if(ZeroErr > 20 || ZeroErr < -20)
		Cur_Sensor_Err1 = 1;

	Cal_I_Zero = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
	ZeroErr = (int16_t)(Cal_I_Zero - ATD_Value_u16[ADC_IB1_AD]);
	if(ZeroErr > 20 || ZeroErr < -20)
		Cur_Sensor_Err1 = 1;
*/
	//检测反向开关状态
	if(FWD_SW == SW_OFF && REV_SW == SW_ON)
		Rev_Mode = 1;                          //后退
	else
		Rev_Mode = 0;                          //前进

	if(FWD_SW != REV_SW)
		NTL_Error = 1;

  Calculate_Speed(Hall_Str1.CurSpeed,&Motor1,&Obser1);    //更新转速值

	//由AD计算实际电压值
	B_Voltage_64 = Get_Voltage(ATD_Value_u16[ADC_B_AD]);
	B_Voltage = B_Voltage_64 >> 6;

	//上电后立即检测电池电压，后在100ms任务函数中检测
	if((ATD_Value_u16[ADC_B_AD] >> ADx_AD8_Coeff) < LowVoltageAD)
	{
		Global_En = 0;LowVolt_Err = 1;Wait_Timer = 1;
	}
	else if((ATD_Value_u16[ADC_B_AD] >> ADx_AD8_Coeff) > OverVoltageAD)
	{
		Global_En = 0;B_OverVolt_Err = 1;OverVolt_Err = 1;
	}
}

//延迟100us函数过程：周期数：1+1+1+133*10*3+1+2+4(nop)=4000;每个周期的时间：1/40M=0.025us，100us=4000*0.025us
APP_SECTION void Cpu_Delay100US(void)
{
	uint32_t j=0;

	//for(i=0;i<us;i++)
		for(j=0;j<1000;j++)
			{}

}

//############################################################################
//函数功能：PWM输出
//函数名：PWM_Output(uint8_t Channel,uint16_t Duty)
//
//描述：
//
//作者：Ted
//时间：2013-10-11
//############################################################################
APP_SECTION void PWM_Output(uint32_t Channel,uint32_t Duty)
{
	SET_32(Channel, Duty);
  return;
}

//############################################################################
//函数功能：关闭控制器输出
//函数名：CloseAll()
//
//描述：
//
//作者：Dwane
//时间：2013-02-23
//############################################################################
APP_SECTION void CloseAll(void)
{
	M_En = 0;
	Global_En = 0;
	PWM_PORT = PWM1_DI;
  return;
}
