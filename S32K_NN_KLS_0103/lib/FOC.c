#include "Variable.h"
#include "Function.h"
#include "FOC.h"

/*********************************************************************************
                                   ClarkePark
 *********************************************************************************/

SIN_COS_STR FOC_SinCos1;
I_CS_STR ParkParm_I1;
V_CS_STR ParkParm_V1;
PWM_STR PWM1;


/*==========================================================================
 * 函数名:SinCos
 * 功能描述:计算sin和cos值。
 *
 * 参数:struct{
							uint16_t  qAngle;
							int16_t   qSin;
							int16_t   qCos;
							}SIN_COS_STR;
 * 参数解释:输入qAngle和正弦表格，计算得出qSin，qCos
 *
 * 返回值:无
 * 调用实例：SinCos(&FOC_SinCos, (int32_t *)(&sinetable));
 *
 * 作 者:        ted, 2018 年 08 月 22 日
 * 修改记录:
 *
 *===========================================================================
 */

APP_SECTION void SinCos(SIN_COS_STR *pSinCos, int32_t *sin_tb)
{
  int32_t  SinCos_err;
  uint32_t SinCos_temp;
  uint32_t SinCos_temp_H,SinCos_temp_L;
  uint32_t  Index_Sin,Index_Cos,Index_Sin_up,Index_Cos_up;

  //定向角为16位数据，360°对应2的16次方
  SinCos_temp = (uint32_t)((*pSinCos).qAngle) >> 1;
  
  SinCos_temp_H = (uint32_t)(SinCos_temp >> 16);
  SinCos_temp_L = (uint32_t)(SinCos_temp & 0x0000ffff);
  
  Index_Sin = (uint32_t)SinCos_temp_H;
  Index_Cos = (Index_Sin + 0x20) & 0x7f;
  
  if(SinCos_temp_L == 0) 
  { 
  	(*pSinCos).qSin = sin_tb[Index_Sin];
  	(*pSinCos).qCos = sin_tb[Index_Cos];
  } 
  else 
  {
    Index_Sin_up = (Index_Sin + 0x01) & 0x7f;
    Index_Cos_up = (Index_Cos + 0x01) & 0x7f;
    
    SinCos_err = (int32_t)(sin_tb[Index_Sin_up] - sin_tb[Index_Sin]) *  SinCos_temp_L;
    (*pSinCos).qSin = (int32_t)(SinCos_err >> 16) + sin_tb[Index_Sin];
    
    SinCos_err = (int32_t)(sin_tb[Index_Cos_up] - sin_tb[Index_Cos]) *  SinCos_temp_L;
    (*pSinCos).qCos = (int32_t)(SinCos_err >> 16) + sin_tb[Index_Cos];
  }

}


/*==========================================================================
 * 函数名:ClarkePark
 * 功能描述:clarkpark变换，Iab变换为Idq。
 *
 * 参数:
 * 参数解释:输入Ia,Ib和正余弦值，计算得出Id，Iq
 *
 * 返回值:无
 * 调用实例：ClarkePark(&ParkParm_I, &FOC_SinCos);
 *
 * 作 者:        ted, 2018 年 08 月 22 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void ClarkePark(I_CS_STR *pIcs, SIN_COS_STR *pSinCos)
{
  int32_t  ClarkePark_temp,ParkParm_qIalpha,ParkParm_qIbeta;

  ParkParm_qIalpha = (*pIcs).Ia;

  //18919    1/√3   Q15         //37838    2/√3   Q15
  ClarkePark_temp = (int32_t)(*pIcs).Ia * 18919 + (int32_t)(*pIcs).Ib * 37838;   //1/gen3 18919
  ParkParm_qIbeta = (int16_t)(ClarkePark_temp >> 15);

  ClarkePark_temp = (int32_t)ParkParm_qIalpha * (*pSinCos).qCos + (int32_t)ParkParm_qIbeta * (*pSinCos).qSin;
  (*pIcs).Id = (int16_t)(ClarkePark_temp >> 15);

  ClarkePark_temp = (int32_t)ParkParm_qIbeta * (*pSinCos).qCos - (int32_t)ParkParm_qIalpha * (*pSinCos).qSin;
  (*pIcs).Iq = (int16_t)(ClarkePark_temp >> 15);
}

/*==========================================================================
 * 函数名:CalcRefVec
 * 功能描述:计算参考电压，Vdq变换为V123。
 *
 * 参数:
 * 参数解释:输入Vd,Vq和正余弦值，计算得出V1,V2,V3
 *
 * 返回值:无
 * 调用实例：CalcRefVec(&ParkParm_V, &FOC_SinCos);
 *
 * 作 者:        ted, 2018 年 08 月 22 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void CalcRefVec(V_CS_STR *pVcs, SIN_COS_STR *pSinCos)
{
  int32_t temp,temp1,temp2;

  //InvPark
  temp = ((int32_t)(*pVcs).Vd * (*pSinCos).qCos) - ((int32_t)(*pVcs).Vq * (*pSinCos).qSin);
  temp1 = (int32_t)(temp >> 15);//Valpha

  temp = ((int32_t)(*pVcs).Vd * (*pSinCos).qSin) + ((int32_t)(*pVcs).Vq * (*pSinCos).qCos);
  temp2 = (int32_t)(temp >> 15);//Vbeta

  (*pVcs).V1 = temp2;

  //28377    √3/2    Q15						//16384    1/2    Q15
  temp1 = (int32_t)temp1 * 28377;
  temp2 = -(int32_t)(temp2 * 16384);

  temp = temp1 + temp2;
  (*pVcs).V2 = (int32_t)(temp >> 15);

  temp = temp2 - temp1;
  (*pVcs).V3 = (int32_t)(temp >> 15);
}
/*==========================================================================
 * 函数名:CalcSVGen
 * 功能描述:计算PWM输出值。
 *
 * 参数:
 * 参数解释:输入V1,V2,V3，计算得出PWMA,B,C
 *
 * 返回值:无
 * 调用实例：CalcSVGen(&ParkParm_V, &PWM);
 *
 * 作 者:        ted, 2018 年 08 月 22 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void CalcSVGen(V_CS_STR *pVcs, PWM_STR *pPWM)
{
	SVGEN_STR SVGen;
	if( (*pVcs).V1  >= 0 )
  {
    // (xx1)
    if( (*pVcs).V2  >= 0 )
    {
      // (x11)
      // Must be Sector 3 since Sector 7 not allowed
      // Sector 3: (0,1,1)  0-60 degrees
      //SVGenT1 = SVGenParm_qVr2;
      //SVGenT2 = SVGenParm_qVr1;
      CalcTimes((*pVcs).V2,(*pVcs).V1,(*pPWM).Mod,&SVGen);
      (*pPWM).Cha = SVGen.Ta;
      (*pPWM).Chb = SVGen.Tb;
      (*pPWM).Chc = SVGen.Tc;
      //Sector =1;
    }
    else
    {
      // (x01)
      if( (*pVcs).V3  >= 0 )
      {
        // Sector 5: (1,0,1)  120-180 degrees
        //SVGenT1 = SVGenParm_qVr1;
        //SVGenT2 = SVGenParm_qVr3;
        CalcTimes((*pVcs).V1,(*pVcs).V3,(*pPWM).Mod,&SVGen);
        (*pPWM).Cha = SVGen.Tc;
        (*pPWM).Chb = SVGen.Ta;
        (*pPWM).Chc = SVGen.Tb;
        //Sector = 3;
      }
      else
      {
        // Sector 1: (0,0,1)  60-120 degrees
        //SVGenT1 = -SVGenParm_qVr2;
        //SVGenT2 = -SVGenParm_qVr3;
        CalcTimes(-(*pVcs).V2,-(*pVcs).V3,(*pPWM).Mod,&SVGen);
        (*pPWM).Cha = SVGen.Tb;
        (*pPWM).Chb = SVGen.Ta;
        (*pPWM).Chc = SVGen.Tc;
        //Sector = 2;
      }
    }
  }
  else
  {
    // (xx0)
    if( (*pVcs).V2  >= 0 )
    {
      // (x10)
      if( (*pVcs).V3  >= 0 )
      {
        // Sector 6: (1,1,0)  240-300 degrees
        //SVGenT1 = SVGenParm_qVr3;
        //SVGenT2 = SVGenParm_qVr2;
        CalcTimes((*pVcs).V3,(*pVcs).V2,(*pPWM).Mod,&SVGen);
        (*pPWM).Cha = SVGen.Tb;
        (*pPWM).Chb = SVGen.Tc;
        (*pPWM).Chc = SVGen.Ta;
        //Sector = 5;
      }
      else
      {
        // Sector 2: (0,1,0)  300-0 degrees
        //SVGenT1 = -SVGenParm_qVr3;
        //SVGenT2 = -SVGenParm_qVr1;
        CalcTimes(-(*pVcs).V3,-(*pVcs).V1,(*pPWM).Mod,&SVGen);
        (*pPWM).Cha = SVGen.Ta;
        (*pPWM).Chb = SVGen.Tc;
        (*pPWM).Chc = SVGen.Tb;

        //Sector =6;
      }
    }
    else
    {
      // (x00)
      // Must be Sector 4 since Sector 0 not allowed
      // Sector 4: (1,0,0)  180-240 degrees
      //SVGenT1 = -SVGenParm_qVr1;
      //SVGenT2 = -SVGenParm_qVr2;
      CalcTimes(-(*pVcs).V1,-(*pVcs).V2,(*pPWM).Mod,&SVGen);
      (*pPWM).Cha = SVGen.Tc;
      (*pPWM).Chb = SVGen.Tb;
      (*pPWM).Chc = SVGen.Ta;
      //Sector = 4;
    }
  }

  ///最高占空比97%,1.5us低电平时间,保证高端管能打开,低保真3%-97%对应MAX_PWM-MIN_PWM
  if((*pPWM).Cha>(*pPWM).Max)
  	(*pPWM).Cha = (*pPWM).Max;
  else if((*pPWM).Cha<(*pPWM).Min)
  	(*pPWM).Cha=(*pPWM).Min;

  if((*pPWM).Chb>(*pPWM).Max)
  	(*pPWM).Chb = (*pPWM).Max;
  else if((*pPWM).Chb<(*pPWM).Min)
  	(*pPWM).Chb=(*pPWM).Min;

  if((*pPWM).Chc>(*pPWM).Max)
  	(*pPWM).Chc = (*pPWM).Max;
  else if((*pPWM).Chc<(*pPWM).Min)
  	(*pPWM).Chc=(*pPWM).Min;
}

/*==========================================================================
 * 函数名:CalcTimes
 * 功能描述:计算Ta,Tb,Tc。
 *
 * 参数:
 * 参数解释:
 *
 * 返回值:无
 * 调用实例：
 *
 * 作 者:        ted, 2018 年 08 月 22 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void CalcTimes(uint32_t t1,uint32_t t2,uint32_t pwm_mod,SVGEN_STR *pSvgen)
{
  uint32_t t_sum,temp1,SVGenT1,SVGenT2;

  SVGenT1 = (uint32_t)((t1 * pwm_mod) >> 15);
  SVGenT2 = (uint32_t)((t2 * pwm_mod) >> 15);

  t_sum = SVGenT1 + SVGenT2;
  if(t_sum > pwm_mod)  //采取过调制处理
  {
    temp1 = (uint32_t)((uint32_t)((uint32_t)pwm_mod << 15) / t_sum);
    SVGenT1 = (uint32_t)(((uint32_t)SVGenT1 * temp1) >> 15);
    SVGenT2 = (uint32_t)(((uint32_t)SVGenT2 * temp1) >> 15);
  }

  (*pSvgen).Tc = (uint32_t)(((uint16_t)pwm_mod - (SVGenT1 + SVGenT2)) >> 1);  // t0/2
  (*pSvgen).Tb = (uint32_t)((*pSvgen).Tc + SVGenT2);
  (*pSvgen).Ta = (uint32_t)((*pSvgen).Tb + SVGenT1);

}

