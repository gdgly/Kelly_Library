/*
 * Pedal.c
 *
 *  Created on: 2018年11月13日
 *      Author: Administrator
 */

#include "Function.h"
#include "Variable.h"
#include "Platform.h"

static uint16_t Half_TPS_Range,FWD_MAP_TPS_AD,REV_MAP_TPS_AD,Tps_LowErr_AD,Tps_HighErr_AD;
static uint16_t BRK_Low_End,BRK_High_End;
static uint8_t FWD_TPS_MAP,REV_TPS_MAP;

volatile uint16_t TPSx_Output,Temp_T_TPSx,BRAKEx;
volatile uint8_t StartupStatus;

typedef struct
{
  uint16_t TPS_Low_End;
  uint16_t TPS_High_End;
  uint16_t TPS_Range;
  uint8_t  TPS_Loop;
  uint16_t TPS_Array[TPS_MAX_SAMPLING];
  uint16_t TPS_Out;
}TPS_STR;

TPS_STR Tps_Parm;

/*==========================================================================
* 函数名:Pedal_Parms_Init
* 功能描述:参数设置，设置油门及刹车死区
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
APP_SECTION void Pedal_Parms_Init(void)
{

  //刹车死区设置
	BRK_Low_End  = (uint16_t)((uint32_t)(Default_Cal_data[CAL_BRAKE_DEAD_ZONE_LOW] * AD_5V) / 100);
	BRK_High_End = (uint16_t)((uint32_t)(Default_Cal_data[CAL_BRAKE_DEAD_ZONE_HIGH] * AD_5V) / 100);
  //油门死区和报错阈值设置
	if(Default_Cal_data[CAL_TPS_TYPE] == TPS0_5V)
  {
    Tps_Parm.TPS_Low_End  = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_DEAD_ZONE_LOW] * AD_5V) / 100);
    Tps_Parm.TPS_High_End = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_DEAD_ZONE_HIGH] * AD_5V) / 100);
  	Tps_Parm.TPS_Range    = (uint16_t)(Tps_Parm.TPS_High_End - Tps_Parm.TPS_Low_End);
  	Tps_LowErr_AD  = 0;
  	Tps_HighErr_AD = AD_5V;
  }
  else if(Default_Cal_data[CAL_TPS_TYPE] == TPS1_4V)
  {
    Tps_Parm.TPS_Low_End  = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_DEAD_ZONE_LOW] * AD_5V) / 100);
    Tps_Parm.TPS_High_End = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_DEAD_ZONE_HIGH] * AD_5V) / 100);
  	Tps_Parm.TPS_Range    = (uint16_t)(Tps_Parm.TPS_High_End - Tps_Parm.TPS_Low_End);
  	Tps_LowErr_AD = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_LOWERR_PERCENT] * AD_5V) / 100);
  	Tps_HighErr_AD = (uint16_t)((uint32_t)(Default_Cal_data[CAL_TPS_HIGHERR_PERCENT] * AD_5V) / 100);

  }

  Half_TPS_Range = Tps_Parm.TPS_Range >> 1;
  FWD_MAP_TPS_AD = (uint32_t)((uint32_t)Half_TPS_Range * Default_Cal_data[CAL_TPS_MAP_FWD]) / 50;
  REV_MAP_TPS_AD = (uint32_t)((uint32_t)Half_TPS_Range * Default_Cal_data[CAL_TPS_MAP_REV]) / 50;
  FWD_TPS_MAP = 100 - Default_Cal_data[CAL_TPS_MAP_FWD];
  REV_TPS_MAP = 100 - Default_Cal_data[CAL_TPS_MAP_REV];

  Clear_TPS_Array(); //将Tps_Parm.TPS_Array[TPS_MAX_SAMPLING]中的数据清零，防止复位后数据不为零导致TPSx_Output不为零而引起误输出


	return;
}

/*==========================================================================
* 函数名:Releas_Brake_Disable_HP
* 功能描述:退出刹车检测高踏板函数
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
APP_SECTION void Releas_Brake_Disable_HP(void)
{
  static uint8_t Brake_Mode=0;

  if((BRAKEx > 0) || (BRK_SW == BRK_ON))
  {
    Brake_Mode = 1; 	             //进入刹车状态Brake_Mode=1
    TPSx_Output = 0;							//清除油门值
  }
  if(Releas_BRK_DisHP == 1)
  {
  	if((Brake_Mode == 1) && (BRAKEx == 0) && (BRK_SW == BRK_OFF))   //退出刹车，对高踏板错误标志进行处理
    {
      if(Temp_T_TPSx > 0)		//油门不为0，报错
      {
        Startup_TPS_Err = 1;
        TPSx_Output = 0;
      }
      else//油门为零，清除油门数组，防止误动
      {
        Brake_Mode = 0;
        Startup_TPS_Err = 0;
        Clear_TPS_Array();
        TPSx_Output = 0;
      }
    }
  }
}



/*==========================================================================
* 函数名:Acc_Percent
* 功能描述:油门计算函数
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
APP_SECTION uint16_t Acc_Percent(uint16_t Tps_AD_Value)
{
  uint8_t  i      = 0;
  uint16_t T_TPSx = 0;
  uint16_t TPS_Out_Temp = 0;
  uint32_t TPS_Out_Sum  = 0;

  if(Default_Cal_data[CAL_TPS_TYPE] == TPS1_4V)//TPS 1-4V
  {
    if(Tps_AD_Value < Tps_LowErr_AD || Tps_AD_Value > Tps_HighErr_AD)
    {
      TPS_Error = 1;
    }
  }

  if(WithFoot && FOOT_SW == FOOT_OFF)
  {
    T_TPSx            = 0;
    Tps_Parm.TPS_Out  = 0;
    Tps_Parm.TPS_Loop = 0;
	  Clear_TPS_Array();
  	Temp_T_TPSx = 0;  //用于退出刹车检测高踏板
	}
	else
	{
    if(TPS_Error == 1)
    {
      T_TPSx = 0;
    }
    else
    {
			T_TPSx = Tps_AD_Value;

			if(Default_Cal_data[CAL_TPS_TYPE] == TPS0_5V || Default_Cal_data[CAL_TPS_TYPE] == TPS1_4V)//TPS 0-5V
			{
			}
			else
			{
				T_TPSx = 0;
				Tps_Parm.TPS_Loop = 0;
				Clear_TPS_Array();
			}
    }

		if(T_TPSx < Tps_Parm.TPS_Low_End)
		{
			T_TPSx = Tps_Parm.TPS_Low_End;
		}
		else if(T_TPSx > Tps_Parm.TPS_High_End)
		{
			T_TPSx = Tps_Parm.TPS_High_End;
		}

		T_TPSx = (uint16_t)(T_TPSx - Tps_Parm.TPS_Low_End);

		Temp_T_TPSx = T_TPSx;  //用于退出刹车检测高踏板

		if(Rev_Mode == 0)  //正向MAP
		{
			if(T_TPSx <= Half_TPS_Range)
				T_TPSx = (uint32_t)((uint32_t)T_TPSx * Default_Cal_data[CAL_TPS_MAP_FWD]) / 50;
			else
				T_TPSx = (uint32_t)((uint32_t)(T_TPSx - Half_TPS_Range) * FWD_TPS_MAP) / 50 + FWD_MAP_TPS_AD;
		}
		else   //反向MAP
		{
			if(T_TPSx <= Half_TPS_Range)
				T_TPSx = (uint32_t)((uint32_t)T_TPSx * Default_Cal_data[CAL_TPS_MAP_REV]) / 50;
			else
				T_TPSx = (uint32_t)((uint32_t)(T_TPSx - Half_TPS_Range) * REV_TPS_MAP) / 50 + REV_MAP_TPS_AD;
		}

    TPS_Out_Temp = (uint16_t)((uint32_t)(T_TPSx * OUTMAX_Q14) / Tps_Parm.TPS_Range) ;  //Linear Interpolation
  }
	Tps_Parm.TPS_Array[Tps_Parm.TPS_Loop] = TPS_Out_Temp;

  Tps_Parm.TPS_Loop++;

  if(Tps_Parm.TPS_Loop >= TPS_MAX_SAMPLING)
  {
    Tps_Parm.TPS_Loop = 0;
  }

	TPS_Out_Sum = 0;

	for(i = 0;i < TPS_MAX_SAMPLING;i++)
	{
	  TPS_Out_Sum += 	Tps_Parm.TPS_Array[i];
	}

  //Check Startup TPS status
	if(StartupStatus == STARTUP_POWERON)//only check once on PowerOn status
	{
	  if(Temp_T_TPSx > 0) //if TPSx >0%,show a error
	  {
	    Startup_TPS_Err     = 1;
	    Tps_Parm.TPS_Out = 0;
	    return Tps_Parm.TPS_Out;
	  }
	  else if(Temp_T_TPSx <= 0)
	  {
	    Startup_TPS_Err = 0;
	    StartupStatus   = STARTUP_NORMALLY;
	    Clear_TPS_Array();
	  }
	}

  Tps_Parm.TPS_Out = (uint16_t)(TPS_Out_Sum / TPS_MAX_SAMPLING);
	if(Tps_Parm.TPS_Out > 32767)
    Tps_Parm.TPS_Out = 32767; //

  return Tps_Parm.TPS_Out;
}

/*==========================================================================
* 函数名:Clear_TPS_Array
* 功能描述:油门数组清零函数
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
APP_SECTION void Clear_TPS_Array(void)
{
  volatile uint8_t i;

  for(i=0;i<TPS_MAX_SAMPLING;i++)
  {
    Tps_Parm.TPS_Array[i]=0;
  }
  return;
}


/*==========================================================================
* 函数名:Brake_Percent
* 功能描述:刹车踏板计算函数
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
APP_SECTION uint16_t Brake_Percent(uint16_t Brk_AD_Value)
{
  uint16_t T_BRAKEx = 0;

  if(Default_Cal_data[CAL_BRAKE_TYPE] == TPS0_5K)
  {
    if(Brk_AD_Value > (ATD_Value_u16[ADC_V_AD] + (30<<ADx_AD8_Coeff)))//TPS1 0-5K
    {
      TPS_Error=1;
    }
  }
  else if(Default_Cal_data[CAL_BRAKE_TYPE] == TPS1_4V)//TPS 1-4V
  {
    if(Brk_AD_Value < Tps_LowErr_AD || Brk_AD_Value > Tps_HighErr_AD)
    {
      TPS_Error = 1;
    }
  }
  if(TPS_Error == 1)
  {
    T_BRAKEx = 0;
    return T_BRAKEx;
  }
  else
  {
    T_BRAKEx = Brk_AD_Value;

    if(Default_Cal_data[CAL_BRAKE_TYPE] == TPS0_5V || Default_Cal_data[CAL_BRAKE_TYPE] == TPS1_4V)//TPS 0-5V
    {
    }
    else if(Default_Cal_data[CAL_BRAKE_TYPE] == TPS0_5K)
    {
      if(Brk_AD_Value > ATD_Value_u16[ADC_V_AD])
      {
        T_BRAKEx = AD_5V;
      }
      else
      {
        T_BRAKEx = (uint16_t)((uint32_t)(Brk_AD_Value * AD_5V) / ATD_Value_u16[ADC_V_AD]);
      }
    }
    else
    {
      T_BRAKEx = 0;
      return T_BRAKEx;
    }
  }

  if(T_BRAKEx < BRK_Low_End)
  {
    T_BRAKEx = BRK_Low_End;
  }
  else if(T_BRAKEx > BRK_High_End)
  {
    T_BRAKEx = BRK_High_End;
  }

  T_BRAKEx = (uint16_t)(T_BRAKEx - BRK_Low_End);

	T_BRAKEx = (uint16_t)((uint32_t)(T_BRAKEx * 100)/(uint16_t)(BRK_High_End - BRK_Low_End)); //Linear Interpolation

  return T_BRAKEx;
}

