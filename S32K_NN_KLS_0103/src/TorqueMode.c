#include "Function.h"
#include "Variable.h"
#include "Platform.h"
#include "Motor.h"

#ifndef _TORQUE_MODE_C
#define _TORQUE_MODE_C

static uint16_t Max_Fwd_RPM,Max_Rev_RPM;
static uint16_t IVTBRK_Min_RPM,Min_BRK_RPM;
static uint16_t T_Acc_Rate,T_Acc_Rel_Rate,T_BRK_Rate,T_BRK_Rel_Rate;
static int32_t Target_Vq_max,Target_Vq_min;

/*==========================================================================
* 函数名:RPM_Parms_Init
* 功能描述:电机控制参数设置
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
APP_SECTION void RPM_Parms_Init(void)
{
  uint16_t max_rpm_output=0,max_rpm_firmware=0,RPM_Constraint=0;
  uint16_t Max_Motor_RPM;
  /************************************************************
                           转速计算参数
  *************************************************************/
  max_rpm_output = (uint32_t)(60 * (((uint16_t)(Default_Cal_data[INFO_MAX_OUTPUT_F_H])<<8)+Default_Cal_data[INFO_MAX_OUTPUT_F_L])) / (Default_Cal_data[CAL_MOTOR_POLES] >> 1);
  max_rpm_firmware = nmax_Firmware;
	RPM_Constraint = MIN(max_rpm_output,max_rpm_firmware);

  /************************************************************
                            输出参数
  *************************************************************/
	Max_Motor_RPM = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_MAX_MOTOR_RPM_H])<<8)+Default_Cal_data[CAL_MAX_MOTOR_RPM_L]);
  if(Max_Motor_RPM > RPM_Constraint)
  	Max_Motor_RPM = RPM_Constraint;

  if(Default_Cal_data[CAL_FW_SPEED_LIMIT] < 100)
  	Max_Fwd_RPM = (uint16_t)((uint32_t)(Max_Motor_RPM * Default_Cal_data[CAL_FW_SPEED_LIMIT]) / 100);
  else
  	Max_Fwd_RPM = Max_Motor_RPM;


  if(Default_Cal_data[CAL_REV_SPEED_LIMIT] < 100)
  	Max_Rev_RPM = (uint16_t)((uint32_t)(Max_Motor_RPM * Default_Cal_data[CAL_REV_SPEED_LIMIT]) / 100);
  else
  	Max_Rev_RPM = Max_Motor_RPM;

  IVTBRK_Min_RPM = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_T_IVT_BRK_MINRPM_H])<<8)+Default_Cal_data[CAL_T_IVT_BRK_MINRPM_L]);
  Min_BRK_RPM = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_MIN_BRK_RPM_H])<<8)+Default_Cal_data[CAL_MIN_BRK_RPM_L]);

  Target_Vq_max = (int32_t)(((uint16_t)(Default_Cal_data[INFO_ACQR_OUT_MAX_Q15_H])<<8)+Default_Cal_data[INFO_ACQR_OUT_MAX_Q15_L]);
  Target_Vq_min =  -Target_Vq_max;

  //Calc_RefTorque函数1s调用500次，速率标定等级为0.1
	T_Acc_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_ACCEL_TIME]);
	T_Acc_Rel_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_ACC_RLS_TIME]);
	T_BRK_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_BRAKE_TIME]);
	T_BRK_Rel_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_BRAKE_RLS_TIME]);

  return;
}

/*==========================================================================
* 函数名:Rev_Status_Check
* 功能描述:判断方向开关状态
*
* 参数:    
* 参数解释:
*
* 返回值:
*
* 作 者:        Icy, 2013 年 5 月 30 日
* 修改记录: 
*
*===========================================================================
*/
APP_SECTION void Rev_Status_Check(void)
{
 	static uint8_t Last_Neutral_Flag =0;
 	uint32_t Rev1_Allow=0;

	if(FWD_SW == REV_SW)                        //开关状态相同，空挡
	{
		Neutral_Flag    = 1;
		TPSx_Output     = 0;                       //油门清零
		StartupStatus   = STARTUP_NORMALLY;        //空挡状态下不检测高踏板
		Startup_TPS_Err = 0;
		NTL_Error = 0;                     //清除高踏板错误
		Last_Neutral_Flag = Neutral_Flag;
	}
	else                   //非空挡状态
	{
		if(Neutral_Flag != Last_Neutral_Flag && Neutral_DisHP == 1)
		{
			if(Temp_T_TPSx > 0)
			{
				Startup_TPS_Err = 1;
				TPSx_Output = 0;
				NTL_Error = 1;
				Clear_TPS_Array();
			}
			else
			{
				Startup_TPS_Err = 0;
				NTL_Error = 0;
				Last_Neutral_Flag = Neutral_Flag;
			}
		}

		if(FWD_SW == SW_ON && REV_SW == SW_OFF)
			New_Rev_Mode = 0;                          //前进
		else
			New_Rev_Mode = 1;                          //后退

		if(Rev_Mode != New_Rev_Mode)                 //如果与上次空挡前的状态不同
		{
			if(Motor1.Mot_AbsRPM < IVTBRK_Min_RPM || New_Rev_Mode == Motor1.Cur_Mode)
				Rev1_Allow = 1;

			if(Rev1_Allow == 1)
			{
				Rev_Mode = New_Rev_Mode;
			}
			else
			{
				TPSx_Output = 0;
				Clear_TPS_Array();
			}
		}
		if(Rev_Mode == 0)
			Ref_Direction = 1;
		else
			Ref_Direction = -1;

		Neutral_Flag    = 0;
	}
}

/*==========================================================================
* 函数名:Torque_Buffer_Function
* 功能描述:根据转矩命令值、建立率、释放率计算出转矩目标值
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
APP_SECTION int16_t Torque_Buffer_Function(TORGUE_STR *Torque,uint16_t Bld_Rate,uint16_t Rls_Rate)
{
  if((*Torque).Ref != 0)
  {
    (*Torque).Cmd64 = (int32_t)((int16_t)(*Torque).Ref << 7);
    if((*Torque).Tgt64 < (*Torque).Cmd64)
    {
      (*Torque).Tgt64 += Bld_Rate;
      if((*Torque).Tgt64 > (*Torque).Cmd64)
      {
        (*Torque).Tgt64 = (*Torque).Cmd64;
      }
    }
    else if((*Torque).Tgt64 > (*Torque).Cmd64)
    {
      (*Torque).Tgt64 -= Bld_Rate;
      if((*Torque).Tgt64 < (*Torque).Cmd64)
      {
        (*Torque).Tgt64 = (*Torque).Cmd64;
      }
    }
  }
  else
  {
    (*Torque).Cmd64 = 0;
    if((*Torque).Tgt64 < 0)
    {
      (*Torque).Tgt64 += Rls_Rate;
      if((*Torque).Tgt64 > 0)
      {
        (*Torque).Tgt64 = 0;
      }
    }
    else if((*Torque).Tgt64 > 0)
    {
      (*Torque).Tgt64 -= Rls_Rate;
      if((*Torque).Tgt64 < 0)
      {
        (*Torque).Tgt64 = 0;
      }
    } 
  }
  
  (*Torque).Tgt = (int16_t)((int32_t)(*Torque).Tgt64 >> 7);
  
  return (*Torque).Tgt;
}

      
/*==========================================================================
* 函数名:Calc_TqeMd_Ref
* 功能描述:计算不同状态下的转矩命令值和目标转速
*
* 参数:    
* 参数解释:
*
* 返回值:
*
* 作 者:        Icy, 2013 年 5 月 30 日
* 修改记录: 
*
*===========================================================================
*/  

APP_SECTION void Calc_TqeMd_Ref(void)
{
  int16_t Temp_Tgt_RPM=0;
  //正常情况下的油门，刹车处理      
  if(BRAKEx > 0 || (BRK_SW == BRK_ON) || Neutral_Flag == 1 || TPSx_Output == 0)  //有制动条件满足时
  {
		if(Motor1.Mot_AbsRPM > Min_BRK_RPM && Motor1.Cur_Mode == Rev_Mode && (ATD_Value_u8[Ctrl_1.PA.VAD_CH] > 3 || ATD_Value_u8[Ctrl_1.PB.VAD_CH] > 3))
		{
			if(BRAKEx > 0)
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * BRAKEx * Default_Cal_data[CAL_T_BRK_AD_PERCENT]) / 10000) * (-Ref_Direction));
			}
			else if(BRK_SW == BRK_ON && Default_Cal_data[CAL_T_BRK_SW_PERCENT] != 0) //刹车开关优先级最高
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * Default_Cal_data[CAL_T_BRK_SW_PERCENT]) / 100) * (-Ref_Direction));
			}
			else if(Neutral_Flag == 1 && Default_Cal_data[CAL_T_NTL_BRK_PERCENT] != 0)  //空挡优先级次之
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * Default_Cal_data[CAL_T_NTL_BRK_PERCENT]) / 100) * (-Ref_Direction));
			}
			else //松踏板优先级最低
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * (Default_Cal_data[CAL_T_RLSTPS_BRK_PERCENT])) / 100) * (-Ref_Direction));
				if(BRKTorque.Ref == 0)
				{
					if(Ref_Direction == 1)  //无制动力矩时讲目标速度设置为最大，防止速度环调出比较大的制动电流
						Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Fwd_RPM) * Ref_Direction);
					else
						Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Rev_RPM) * Ref_Direction);
				}
			}

			if(BRKTorque.Ref != 0)
				Regen_En = 1;
		}
		else  //速度太低时，释放制动力矩，目标速度为0，缓慢停止
		{
			BRKTorque.Ref = 0;
			Temp_Tgt_RPM = 0;
			Regen_En = 0;
		}

		TPSTorque.Ref = 0; 
    //如果运行方向和制动力矩方向相反，目标速度置为0，减速发电
    if((Rev_Mode == 0 && Real_OutMax <= 0) || (Rev_Mode == 1 && Real_OutMax >= 0))
    	Temp_Tgt_RPM = 0;
    
    if(Rev_Mode == 0) //正转Iq:(-Imax/2)~Imax
    {
    	PIParmQ1.qOutMax = Target_Vq_max;
    	PIParmQ1.qOutMin = 0;
    }
    else    //反转Iq:-Imax~(Imax/2)
    {
    	PIParmQ1.qOutMax = 0;
    	PIParmQ1.qOutMin = Target_Vq_min;
    } 
  }
  else 
  {
    BRKTorque.Ref = 0;
    Regen_En = 0;
    if(TPSx_Output > 0)  //不满足制动条件，按照油门输出力矩
    {
      TPSTorque.Ref = TPSx_Output * Ref_Direction;
      if(Ref_Direction == 1)//无制动力矩时讲目标速度设置为最大，防止速度环调出比较大的制动电流
      	Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Fwd_RPM) * Ref_Direction);
      else
      	Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Rev_RPM) * Ref_Direction);
    }

		PIParmQ1.qOutMax = Target_Vq_max;
		PIParmQ1.qOutMin = Target_Vq_min;
  }

  Motor1.Tgt_RPM = Temp_Tgt_RPM;
}
/*==========================================================================
* 函数名:Calc_TqeMd_Tgt
* 功能描述:计算目标转矩
*
* 参数:    
* 参数解释:
*
* 返回值:
*
* 作 者:        Icy, 2013 年 5 月 29 日
* 修改记录: 
*
*===========================================================================
*/
APP_SECTION void Calc_TqeMd_Tgt(void)
{  
  uint16_t T_Acc_Rate_temp=0,T_BRK_Rate_temp=0; 
  int16_t Target_Out=0;
  //根据油门值和刹车状态计算转矩给定值
  //根据转矩给定值计算出转矩电流给定值

  if(M_En == 1 || Obser1.Start_Allow == 1)  //允许重新启动时或正常输出状态时，才允许建立转矩
  {
		T_Acc_Rate_temp = T_Acc_Rate;
		T_BRK_Rate_temp = T_BRK_Rate; 
  }
  else  //不允许重新启动时，不允许建立转矩，防止衔接时输出太冲
  {
    T_Acc_Rate_temp = 0;
    T_BRK_Rate_temp = 0;
  }
    
  TPSTorque.Tgt = Torque_Buffer_Function(&TPSTorque,T_Acc_Rate_temp,T_Acc_Rel_Rate);
  BRKTorque.Tgt = Torque_Buffer_Function(&BRKTorque,T_BRK_Rate_temp,T_BRK_Rel_Rate);
  
  Target_Out = (int16_t)(TPSTorque.Tgt + BRKTorque.Tgt);
  
  if(Target_Out > (int16_t)OUTMAX_Q14) Target_Out = (int16_t)OUTMAX_Q14;
  if(Target_Out < (int16_t)(-(int16_t)OUTMAX_Q14)) Target_Out = (int16_t)(-(int16_t)OUTMAX_Q14);

  if(Rev_Mode == 0) //正转Iq:-Regen_Iq_Max~Drive_Iq_Max
  {
    if(Target_Out > (int16_t)Drive_OutMax)
    	Target_Out = (int16_t)Drive_OutMax;
    else if(Target_Out < (int16_t)(-Regen_OutMax))
    	Target_Out = (int16_t)(-Regen_OutMax);
  }
  else    //反转Iq:-Drive_Iq_Max~Regen_Iq_Max
  {
    if(Target_Out > (int16_t)Regen_OutMax)
    	Target_Out = (int16_t)Regen_OutMax;
    else if(Target_Out < (int16_t)(-Drive_OutMax))
    	Target_Out = (int16_t)(-Drive_OutMax);
  }
  Real_OutMax = (int16_t)((int32_t)(Target_Out * Current_Max_Q) >> 14);

  //打开或关闭输出使能
  if((Real_OutMax != 0 || BRKTorque.Ref != 0 || TPSTorque.Ref != 0 || TPSTorque.Tgt != 0 || BRKTorque.Tgt != 0)&& Global_En == 1)
  {
    if(M_En == 0)// run once on startup 
    {
    	PWM_PORT = PWM1_DI;
    }
    if(Obser1.Start_Allow == 1 /*|| TPSTorque.Ref != 0*/)
    {
      M_En = 1;
      Obser1.Start_Allow = 0;
    }
  }
  else if(Real_OutMax == 0 && TPSTorque.Tgt == 0 && BRKTorque.Tgt == 0)
  {
     M_En = 0;
  }

}


#endif // _TORQUE_MODE_C
