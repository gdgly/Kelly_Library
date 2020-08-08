/*
 * Voltage.c
 *
 *  Created on: 2018年11月8日
 *      Author: Administrator
 */

#include "Function.h"
#include "Variable.h"
#include "Platform.h"

static uint8_t PWM_VALUE;
static uint8_t LowVoltageReduceAD,LowVoltageResumeAD,LowVoltageADDivisor;
static uint8_t OverVoltageReduceAD,OverVoltageResumeAD,OverVoltageADDivisor;
static uint16_t Controller_Voltage;

volatile uint16_t B_Voltage,B_Voltage_64;
volatile uint8_t LowVoltageAD,OverVoltageAD;
/*==========================================================================
* 函数名:Vol_Parms_Init
* 功能描述:电压参数初始化
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:        Icy, 2013 年 7 月 24 日
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Vol_Parms_Init(void)
{
	uint8_t LowVoltageAD_H,LowVoltageAD_L;
	int16_t LV_Rev_PCB_delta,LV_Rev_Vol_delta;

  Controller_Voltage = (uint16_t)((uint16_t)(Default_Cal_data[INFO_CONTROLLER_VOL_H] << 8) +
                                             Default_Cal_data[INFO_CONTROLLER_VOL_L]
                                            );

	 /*********************************************************************************
	                             Check Battery voltage
	  *********************************************************************************/

	  LowVoltageAD_H=Get_Voltage_AD( ((uint16_t)(Default_Cal_data[CAL_LOW_VOLTAGE_H] << 8) + Default_Cal_data[CAL_LOW_VOLTAGE_L]) );
		if(Default_Cal_data[CAL_LV_REVISE_PERCENT] >= 100)
			LowVoltageAD_L = LowVoltageAD_H;
		else
			LowVoltageAD_L = (uint8_t)((uint32_t)(LowVoltageAD_H * Default_Cal_data[CAL_LV_REVISE_PERCENT]) / 100);

	  LV_Rev_PCB_delta=(int16_t)((int8_t)(Default_Cal_data[CAL_LV_REVISE_PCB_H_TEMP])-(int8_t)(Default_Cal_data[CAL_LV_REVISE_PCB_L_TEMP]));
		LV_Rev_Vol_delta=(int16_t)(LowVoltageAD_H-LowVoltageAD_L);
	  if(PCB_En == 1 && LV_Rev_PCB_delta > 0 && LV_Rev_Vol_delta > 0)
	  {
	    //计算温度值
			//PCB_Temp = Get_Temp((uint8_t)(ATD_Value_u16[ADC_TEMP_AD] >> ADx_AD8_Coeff));

			if(PCB_Temp <= (int8_t)(Default_Cal_data[CAL_LV_REVISE_PCB_L_TEMP]))
				LowVoltageAD = LowVoltageAD_L;
			else if(PCB_Temp >= (int8_t)(Default_Cal_data[CAL_LV_REVISE_PCB_H_TEMP]))
				LowVoltageAD = LowVoltageAD_H;
			else
				LowVoltageAD=(uint8_t)((uint32_t)(LV_Rev_Vol_delta*(PCB_Temp-(int8_t)(Default_Cal_data[CAL_LV_REVISE_PCB_L_TEMP])))/LV_Rev_PCB_delta) + LowVoltageAD_L;
	  }
	  else
	  {
	  	LowVoltageAD = LowVoltageAD_H;
	  }

	  LowVoltageReduceAD=(uint8_t)((uint16_t)(LowVoltageAD*115)/100);//112 is 100%+1/8
		LowVoltageResumeAD=(uint8_t)((uint16_t)(LowVoltageAD*106)/100);

		if(Default_Cal_data[CAL_LV_WEAK_MAX_CURRENT_PERCENTAGE] >= 100)
		  LowVoltageADDivisor=Calculate_Percent(LowVoltageReduceAD,LowVoltageAD,1);
		else
		  LowVoltageADDivisor=Calculate_Percent(LowVoltageReduceAD,LowVoltageAD,(100 - Default_Cal_data[CAL_LV_WEAK_MAX_CURRENT_PERCENTAGE]));

		OverVoltageAD=Get_Voltage_AD( ((uint16_t)(Default_Cal_data[CAL_OVER_VOLTAGE_H] << 8) + Default_Cal_data[CAL_OVER_VOLTAGE_L]) );

		OverVoltageReduceAD=(uint8_t)((uint16_t)(OverVoltageAD*88)/100);//88 is 100%-1/8

		OverVoltageResumeAD=(uint8_t)((uint16_t)(OverVoltageAD*94)/100);

		OverVoltageADDivisor=Calculate_Percent(OverVoltageAD,OverVoltageReduceAD,90);
}
/*==========================================================================
* 函数名:Voltage_Weaking
* 功能描述:电压弱化
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Voltage_Weaking(void)
{
	uint16_t Curve_OutMax=0;
	uint8_t Low_Bat_weak = 100;
	uint16_t Over_Vol_Weak=0;
	uint32_t Us_Q15=0;

  ///确定发电电流的限幅值，过压弱化
  //When battery voltage goes up to a certain value ,weaken regeneration
  if(ATD_Value_u8[ADC_B_AD] <= OverVoltageReduceAD)
    Over_Vol_Weak =0;
  else if(ATD_Value_u8[ADC_B_AD] >= OverVoltageAD)
    Over_Vol_Weak = (uint16_t)((uint32_t)((uint32_t)(OverVoltageAD - OverVoltageReduceAD)<<8) / OverVoltageADDivisor);
  else
    Over_Vol_Weak = (uint16_t)((uint32_t)((uint32_t)(ATD_Value_u8[ADC_B_AD] - OverVoltageReduceAD)<<8) / OverVoltageADDivisor);

  //When battery voltage goes up to a certain value ,weaken regeneration
  if(Over_Vol_Weak > 0)// over 1/16,reduce
  	Regen_OutMax = Temp_OutMax - (uint16_t)((uint32_t)((uint32_t)Temp_OutMax * Over_Vol_Weak) >> 8);
  else
  	Regen_OutMax = Temp_OutMax;

	/*********************************************************************************
							 Armature battery voltage weakening strategy
	*********************************************************************************/
	//detect battery voltage

	//欠压弱化电池电流百分比
	if(ATD_Value_u8[ADC_B_AD] < LowVoltageReduceAD)
	{
		if(ATD_Value_u8[ADC_B_AD] > LowVoltageAD)
			Low_Bat_weak = 100 - (uint8_t)((uint16_t)(100 * (uint8_t)(LowVoltageReduceAD - ATD_Value_u8[ADC_B_AD])) /
														LowVoltageADDivisor);
		else
			Low_Bat_weak = Default_Cal_data[CAL_LV_WEAK_MAX_CURRENT_PERCENTAGE];
	}
	else
		Low_Bat_weak = 100;

	Low_Bat_weak = (uint8_t)((uint16_t)(Low_Bat_weak * Default_Cal_data[CAL_MAX_CURRENT_PERCENTAGE]) / 100);

	/****************双曲线弱化转矩电流限幅值***************/
	Us_Q15 = ParkParm_V1.Us_Q15;
	PWM_VALUE = (uint8_t)((uint32_t)(Us_Q15 * 100) >> 15);
	if(PWM_VALUE > 100)
		PWM_VALUE = 100;

	//双曲线规律弱化最大输出电流以保证电池恒功率输出
	if(PWM_VALUE > Low_Bat_weak)
		Curve_OutMax = (uint16_t)((uint32_t)(Low_Bat_weak * Temp_OutMax) / PWM_VALUE);
	else
		Curve_OutMax = Temp_OutMax;

	///////////////////////////////////////////////////////

	if(StandStill_Err1 == 1)  //堵转弱化转矩电流最大值
	{
		Curve_OutMax = (uint16_t)((uint32_t)(Curve_OutMax * 90) / 100);
	}
	if(Curve_OutMax < (OUTMAX_Q14 >> 3))   //最小12.5%
		Curve_OutMax = (OUTMAX_Q14 >> 3);

	///确定驱动电流的限幅值
	Drive_OutMax = Curve_OutMax;  //弱化最后的最大转矩电流Q值
}
/*==========================================================================
* 函数名:Get_B_Voltage
* 功能描述:获得电池实际电压
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Get_B_Voltage(void)
{
	static uint32_t B_Sample_Times=0,Batt_Count=0;
	uint16_t Temp_B_AD=0;

	if(B_Sample_Times<64)//if samping 640ms
	{
		B_Sample_Times++;

		Batt_Count+=ATD_Value_u16[ADC_B_AD];
	}
	else
	{
		Temp_B_AD=(uint16_t)(Batt_Count >> 6);
		B_Voltage_64 = Get_Voltage(Temp_B_AD);
		B_Voltage = B_Voltage_64 >> 6;
		Batt_Count=0;
		B_Sample_Times=0;
	}
}
 /*==========================================================================
* 函数名:Voltage_Monitor
* 功能描述:欠压过压处理
*
* 参数:
* 参数解释:
*
* 返回值:
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION void Voltage_Monitor(void)
{
  uint8_t T_B_AD=0;
  static uint8_t Low_Vol_Count=0,Batt_Sampling_Times=0;
  static uint16_t Batt_Count=0;

  //欠压检测和过压恢复检测
  if(Batt_Sampling_Times<20)//if samping 40ms
	{
	  Batt_Sampling_Times++;

	  Batt_Count+=ATD_Value_u8[ADC_B_AD];
	}
	else
	{
		T_B_AD=(uint8_t)((uint32_t)Batt_Count/Batt_Sampling_Times);

  	Batt_Count=0;

  	Batt_Sampling_Times=0;

		if(T_B_AD<LowVoltageAD)
  	{
  	  if(Low_Vol_Count < 15)//30s切断
  	    Low_Vol_Count++;
  	  else
    	{
    	  Global_En   = 0;
        LowVolt_Err = 1;//Set Low Battery error code
        Wait_Timer  = 1;
        CloseAll();
        Low_Vol_Count=0;
    	}
  	}
  	else
      Low_Vol_Count=0;
	}

  //发电过压恢复
  if((Regen_OverVolt_Err==1) && (ATD_Value_u8[ADC_B_AD]<=OverVoltageResumeAD) && (BRK_SW==BRK_OFF))//when regen over volt,until volt low that resume volt
  {
  		Regen_OverVolt_Err = 0;
  }

	if(Regen_OverVolt_Err == 0 && B_OverVolt_Err == 0)
		OverVolt_Err = 0;

  //欠压等待
	if(Wait_Timer!=0)
	{
	  Wait_Timer++;
	  if (Wait_Timer>=50)//5s
	  {
	    if((TPSx_Output <= 0 && Stop_Flag == 1) && (ATD_Value_u8[ADC_B_AD]>=LowVoltageResumeAD))//if TPS is 0, clear the error
	    {
		    Wait_Timer=0;
		    LowVolt_Err=0;//Clear Low Battery error code
	    }
	    else
	    {
	      Wait_Timer=1;
	    }
	  }
	}
}

/*==========================================================================
* 函数名:Get_Voltage
* 功能描述:将采样AD值转化为实际电压值
*
* 参数:  Voltage_AD
* 参数解释:AD值
*
* 返回值: 电压
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/                                                           // <<6
APP_SECTION uint16_t Get_Voltage(uint16_t Voltage_AD) //8采样,最大值255,返回值=[64*电压值(V)]
{
	uint16_t i=0;

  switch(Controller_Voltage)
  {
    case 24:
    case 36:
    case 48:
      //24V,36V,48V控制器,V=(Vad/4.06)*64=(100*Vad/406)*64=(3200*Vad)/203
      i=(uint32_t)((3200>>ADx_AD8_Coeff)*Voltage_AD)/203;
      break;

    case 60:
    case 72:
            //72V控制器,V=(Vad/2.71)*64=(100*Vad/271)*64=(6400*Vad)/271
      i=(uint32_t)((6400>>ADx_AD8_Coeff)*Voltage_AD)/271;
      break;

    case 96:
            //96V控制器,V=(Vad/2.08)*64=(100*Vad/208)*64=(400*Vad)/13
      i=(uint32_t)((400>>ADx_AD8_Coeff)*Voltage_AD)/13;
      break;

    case 120:
            //120V控制器,V=(Vad/1.84)*64=(100*Vad/184)*64=(800*Vad)/23
      i=(uint32_t)((800>>ADx_AD8_Coeff)*Voltage_AD)/23;
      break;

    case 144:
            //144V控制器,V=(Vad/1.39)*64=(100*Vad/139)*64=(6400*Vad)/139
      i=(uint32_t)((6400>>ADx_AD8_Coeff)*Voltage_AD)/139;
      break;

    case 320:
            //144V控制器,V=(Vad/0.63)*64=(100*Vad/63)*64=(6400*Vad)/63
      i=(uint32_t)((6400>>ADx_AD8_Coeff)*Voltage_AD)/63;
      break;

    default:
      OverVolt_Err = 1;
  }

	return (i);
}

/*==========================================================================
* 函数名:Get_Voltage_AD
* 功能描述:将实际电压转化为对应的采样AD值
*
* 参数:  Voltage
* 参数解释:电压
*
* 返回值: AD值
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION uint16_t Get_Voltage_AD(uint16_t Voltage)
{
	uint16_t i=0;


//255/(9.45*5)=5.3968253968253968253968253968254
//255/(12.55*5)=4.0637450199203187250996015936255
//255/(18.79*5)=2.7142096860031931878658861096328
//255/(27.69*5)=1.8418201516793066088840736728061
//255/(36.6*5)=1.3934426229508196721311475409836

  switch(Controller_Voltage)
  {
    case 24:
    case 36:

      //i=(uint16_t)((uint16_t)(54*Voltage)+(uint8_t)((uint16_t)(0*Voltage)/10))/10;
      //break;

    case 48:

      i=(uint16_t)((uint16_t)(40*Voltage)+(uint8_t)((uint16_t)(6*Voltage)/10))/10;
      break;

    case 60:
    case 72:

      i=(uint16_t)((uint16_t)(27*Voltage)+(uint8_t)((uint16_t)(2*Voltage)/10))/10;
      break;

    case 96:

      i=(uint16_t)((uint16_t)(20*Voltage)+(uint8_t)((uint16_t)(8*Voltage)/10))/10;
      break;

    case 120:

      i=(uint16_t)((uint16_t)(18*Voltage)+(uint8_t)((uint16_t)(4*Voltage)/10))/10;
      break;

    case 144:

      i=(uint16_t)((uint16_t)(13*Voltage)+(uint8_t)((uint16_t)(9*Voltage)/10))/10;
      break;

    case 320:

      i=(uint16_t)((uint16_t)(6*Voltage)+(uint8_t)((uint16_t)(3*Voltage)/10))/10;
      break;

    default:
      OverVolt_Err = 1;
  }
	return (i);

}
