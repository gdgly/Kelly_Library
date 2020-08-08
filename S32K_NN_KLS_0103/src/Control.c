
#include "Variable.h"
#include "Function.h"
#include "Platform.h"
#include "Motor.h"

static int16_t  LS_ACDR_Kp,LS_ACDR_Ki,LS_ACQR_Kp,LS_ACQR_Ki,HS_ACDR_Kp,HS_ACDR_Ki,HS_ACQR_Kp,HS_ACQR_Ki;
static uint16_t T_Up_RPM,T_Down_RPM;

APP_SECTION void Control(void)
{
	static uint8_t Last_UpDown_Flag1 = 0;
	static uint32_t Count_2ms=0;

  if(Count_2ms%20 == 0)  //2ms
  {
    Count_2ms = 0;
    /*速度环 计算转矩电流给定值*/
    if(M_En && Global_En)
    {
			if(Real_OutMax >= 0)
			{
				PIParmQref1.qOutMax = Real_OutMax;
				PIParmQref1.qOutMin = 0;
			}
			else
			{
				PIParmQref1.qOutMax = 0;
				PIParmQref1.qOutMin = Real_OutMax;
			}
  		PIParmQref1.qInMeas = Motor1.Mot_RPM; //当前转速
  		PIParmQref1.qInRef  = (int32_t)Motor1.Tgt_RPM; //设定转速
  		CalcPI(&PIParmQref1);
  		ParkParm_I1.IqRef = (int16_t)(PIParmQref1.qOut);
    }
  }
  else if(Count_2ms%12 == 0)
  {
  	FdWeakening();
  }
  else if(Count_2ms%16 == 0)
  {
  	if(Last_UpDown_Flag1 == 0 && Motor1.Mot_AbsRPM > T_Up_RPM)
		{
			PIParmD1.qKp = HS_ACDR_Kp;
			PIParmD1.qKi = HS_ACDR_Ki;
			PIParmQ1.qKp = HS_ACQR_Kp;
			PIParmQ1.qKi = HS_ACQR_Ki;
			Last_UpDown_Flag1 = 1;
		}
		else if(Last_UpDown_Flag1 == 1 && Motor1.Mot_AbsRPM < T_Down_RPM)
		{
			PIParmD1.qKp = LS_ACDR_Kp;
			PIParmD1.qKi = LS_ACDR_Ki;
			PIParmQ1.qKp = LS_ACQR_Kp;
			PIParmQ1.qKi = LS_ACQR_Ki;
			Last_UpDown_Flag1 = 0;
		}
  }

  Count_2ms++;

  /****************************************************************************
                               电流环 D Q
  ****************************************************************************/
  // PI control for D
  PIParmD1.qInMeas = ParkParm_I1.Id;
  PIParmD1.qInRef  = ParkParm_I1.IdRef;
  CalcPI(&PIParmD1);
  ParkParm_V1.Vd    = (int16_t)(PIParmD1.qOut);
  
  // PI control for Q
  PIParmQ1.qInMeas = ParkParm_I1.Iq;
  PIParmQ1.qInRef  = ParkParm_I1.IqRef;
  CalcPI(&PIParmQ1);
  ParkParm_V1.Vq   = PIParmQ1.qOut;
}

APP_SECTION void PID_Parms_Init(void)
{
  // ============= PI D Term ===============      
  PIParmD1.qKp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_ACDR_KP_Q13_H])<<8)+Default_Cal_data[CAL_ACDR_KP_Q13_L]);
  PIParmD1.qKi = (int16_t)(((uint16_t)(Default_Cal_data[CAL_ACDR_KI_Q13_H])<<8)+Default_Cal_data[CAL_ACDR_KI_Q13_L]);
  PIParmD1.qErrMax = 4095;
  PIParmD1.qOutMax = (int32_t)(((uint16_t)(Default_Cal_data[INFO_ACDR_OUT_MAX_Q15_H])<<8)+Default_Cal_data[INFO_ACDR_OUT_MAX_Q15_L]);
  PIParmD1.qOutMin = -PIParmD1.qOutMax;
  InitPI(&PIParmD1);

	// ============= PI Q Term ===============
  PIParmQ1.qKp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_ACQR_KP_Q13_H])<<8)+Default_Cal_data[CAL_ACQR_KP_Q13_L]);
  PIParmQ1.qKi = (int16_t)(((uint16_t)(Default_Cal_data[CAL_ACQR_KI_Q13_H])<<8)+Default_Cal_data[CAL_ACQR_KI_Q13_L]);
  PIParmQ1.qErrMax = 4095;
  PIParmQ1.qOutMax = (int32_t)(((uint16_t)(Default_Cal_data[INFO_ACQR_OUT_MAX_Q15_H])<<8)+Default_Cal_data[INFO_ACQR_OUT_MAX_Q15_L]);
  PIParmQ1.qOutMin = -PIParmQ1.qOutMax;
  InitPI(&PIParmQ1);

	T_Up_RPM = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_TRANSITION_UP_RPM_H])<<8)+Default_Cal_data[CAL_TRANSITION_UP_RPM_L]);
	T_Down_RPM = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_TRANSITION_DOWN_RPM_H])<<8)+Default_Cal_data[CAL_TRANSITION_DOWN_RPM_L]);

  LS_ACDR_Kp=PIParmD1.qKp;
  LS_ACDR_Ki=PIParmD1.qKi;
  LS_ACQR_Kp=PIParmQ1.qKp;
  LS_ACQR_Ki=PIParmQ1.qKi;

  HS_ACDR_Kp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_HS_ACDR_KP_Q13_H])<<8)+Default_Cal_data[CAL_HS_ACDR_KP_Q13_L]);
  HS_ACDR_Ki = (int16_t)(((uint16_t)(Default_Cal_data[CAL_HS_ACDR_KI_Q13_H])<<8)+Default_Cal_data[CAL_HS_ACDR_KI_Q13_L]);
  HS_ACQR_Kp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_HS_ACQR_KP_Q13_H])<<8)+Default_Cal_data[CAL_HS_ACQR_KP_Q13_L]);
  HS_ACQR_Ki = (int16_t)(((uint16_t)(Default_Cal_data[CAL_HS_ACQR_KI_Q13_H])<<8)+Default_Cal_data[CAL_HS_ACQR_KI_Q13_L]);
	
	// ============= PI Qref Term ===============
  PIParmQref1.qKp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_T_ASR_KP_Q13_H])<<8)+Default_Cal_data[CAL_T_ASR_KP_Q13_L]);
  PIParmQref1.qKi = (int16_t)(((uint16_t)(Default_Cal_data[CAL_T_ASR_KI_Q13_H])<<8)+Default_Cal_data[CAL_T_ASR_KI_Q13_L]);
  PIParmQref1.qErrMax = (int16_t)(((uint16_t)(Default_Cal_data[CAL_T_ASR_ERROR_Q15_MAX_H])<<8)+Default_Cal_data[CAL_T_ASR_ERROR_Q15_MAX_L]);
  InitPI(&PIParmQref1);

  // ============= PI U Term ===============
  PIParmU.qKp = (int16_t)(((uint16_t)(Default_Cal_data[CAL_AUR_KP_Q13_H])<<8)+Default_Cal_data[CAL_AUR_KP_Q13_L]);
  PIParmU.qKi = (int16_t)(((uint16_t)(Default_Cal_data[CAL_AUR_KI_Q13_H])<<8)+Default_Cal_data[CAL_AUR_KI_Q13_L]);       
  PIParmU.qErrMax = (int16_t)(((uint16_t)(Default_Cal_data[CAL_AUR_ERROR_MAX_Q15_H])<<8)+Default_Cal_data[CAL_AUR_ERROR_MAX_Q15_L]);     
  PIParmU.qOutMax = 0;   
  PIParmU.qOutMin = Id_Min;
  InitPI(&PIParmU);
}

APP_SECTION void InitPI(tPIParm *pParm)
{
 (*pParm).qdSum = 0;
 (*pParm).qOut = 0;
}

APP_SECTION void CalcPI(tPIParm *pParm)
{
  int32_t CalcPI_err,CalcPI_temp,CalcPI_Delta_Int,CalcPI_Pro,CalcPI_Diff=0;
  
  CalcPI_err = (*pParm).qInRef - (*pParm).qInMeas;
  
  //***限制误差信号,防止比例项数据溢出***//
  if(CalcPI_err > (*pParm).qErrMax)    
    CalcPI_err = (*pParm).qErrMax;
  else if(CalcPI_err < -((*pParm).qErrMax))
    CalcPI_err = -((*pParm).qErrMax);
  
  /// P=Kp*err*8 ///
  CalcPI_Pro = ((int32_t)(*pParm).qKp * CalcPI_err)<<3;      //比例项
  
  //对速度环的比例项限幅
  if(CalcPI_Pro > (((*pParm).qOutMax)<<13))   //比例项最大值为8192*32768
    CalcPI_Pro = ((*pParm).qOutMax)<<13;
  else if(CalcPI_Pro < (((*pParm).qOutMin)<<13))
    CalcPI_Pro = ((*pParm).qOutMin)<<13;
  ////////////////////////////////////////////////////////////////////////

  ///加速时输出值等于上限值,或者,减速时输出值等于下限值,停止积分,防止深度饱和、int32_t数据溢出
  if(!((((*pParm).qOut == (*pParm).qOutMax) && ((*pParm).qInRef > (*pParm).qInMeas)) ||
      (((*pParm).qOut == (*pParm).qOutMin) && ((*pParm).qInRef < (*pParm).qInMeas)))/*||
      (pParm==(&PIParmTCmp))*/)
  {   
    CalcPI_Delta_Int = (int32_t)(*pParm).qKi * CalcPI_err;  //积分增量Delta
    
    /*对速度环的积分增量值进行限幅
    if(CalcPI_Delta_Int>3000)
    {
      CalcPI_Delta_Int=3000;
    }
    else if(CalcPI_Delta_Int<-3000)
    {
      CalcPI_Delta_Int=-3000;
    }
    */
    (*pParm).qdSum += CalcPI_Delta_Int;                   //积分项
    
    //对积分项限幅，避免深度饱和//
    if((*pParm).qdSum > (((*pParm).qOutMax)<<13))  //由于积分项为int32_t,所以(*pParm).qOutMax应<=Max/(Kp*8192)
      (*pParm).qdSum = ((*pParm).qOutMax)<<13;     //(32768*65536-1)=Max
    else if((*pParm).qdSum < (((*pParm).qOutMin)<<13))
      (*pParm).qdSum = ((*pParm).qOutMin)<<13;
    //////////////////////////////////////
  }
  CalcPI_temp = (*pParm).qdSum + CalcPI_Pro + CalcPI_Diff;  //计算[(P+I)*8192],int32_t
  
  //对[(P+I)*8192]限幅,防止计算出的(P+I)的int16_t数据溢出或超出限幅值//
  if(CalcPI_temp > (((*pParm).qOutMax)<<13))
    CalcPI_temp = ((*pParm).qOutMax)<<13;
  else if(CalcPI_temp < (((*pParm).qOutMin)<<13))
    CalcPI_temp = ((*pParm).qOutMin)<<13; 
  //////////////////////////////////////////////////////////////////////
  
  //Q环的(比例项+积分项)的最大值=8192*32768+8192*Vqmax,当Vqmax>229375时,会超过int32_t的数据范围,数据溢出
  //当Vqmax=37837时，Kp最大值为6.9282
  //其他PID环可标定的PI参数比Q环稍大
  (*pParm).qOut = (int32_t)(CalcPI_temp >> 13); //计算(P+I),int32_t
  
  /////////////////////////////////////////////////////////////////////////////////////////////////
}


