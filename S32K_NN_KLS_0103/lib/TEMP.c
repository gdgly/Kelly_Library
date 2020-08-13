/*
 * TEMP.c
 *
 *  Created on: 2018年11月7日
 *      Author: Administrator
 */
#include "Function.h"
#include "Variable.h"
#include "Platform.h"

static uint8_t High_Temp_Divisor,Output_Weak_Divisor,High_Measure_divisor,Low_Measure_divisor,HL_High_Temp_Divisor,Motor_High_Temp_Divisor;
static uint8_t HL_Weak_Temp;
static uint16_t MT_OutMax;

volatile uint8_t HL_Temp;
volatile int8_t PCB_Temp;
volatile int16_t Motor_Temp1;
volatile uint16_t Temp_OutMax;


/*==========================================================================
* 函数名:Temp_Parms_Init
* 功能描述:温度参数初始化
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
APP_SECTION void Temp_Parms_Init(void)
{
	int16_t HL_Rev_PCB_delta,HL_Rev_HL_delta;

  /****************************Temperature Weaking Divisor****************************
    **
    **                             (a - b) * 100
    **                Divisor = -----------------------
    **                                  c
    **
    ***********************************************************************************/
    //温度弱化除数设置
    High_Temp_Divisor=Calculate_Percent(Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_TEMP],Default_Cal_data[INFO_HIGH_WEKEN_TEMP],Default_Cal_data[INFO_HIGH_TEMP_WEAK_PERCENT]);
    Output_Weak_Divisor=Calculate_Percent(Default_Cal_data[INFO_HIGH_WEKEN_TEMP],Default_Cal_data[INFO_WENKEN_START_TEMP],Default_Cal_data[INFO_OUTPUT_WEAK_PERCENT]);
    High_Measure_divisor=Calculate_Percent(Default_Cal_data[INFO_OUTPUT_WEAKEN_TEMP],Default_Cal_data[INFO_HIGH_MEASURE_TEMP],Default_Cal_data[INFO_HIGH_MEASURE_TEMP_PERCENT]);
    Low_Measure_divisor=Calculate_Percent(Default_Cal_data[INFO_HIGH_MEASURE_TEMP],Default_Cal_data[INFO_LOW_MEASURE_TEMP],Default_Cal_data[INFO_LOW_MEASURE_TEMP_PERCENT]);

    HL_Rev_PCB_delta=(int16_t)((int8_t)(Default_Cal_data[CAL_HL_REVISE_PCB_H_TEMP])-(int8_t)(Default_Cal_data[CAL_HL_REVISE_PCB_L_TEMP]));
    HL_Rev_HL_delta=(int16_t)(Default_Cal_data[INFO_HL_HIGH_TEMP_WEAK]-Default_Cal_data[INFO_HL_REVISE_WEAK_L_TEMP]);
    if(PCB_En == 1 && HL_Rev_PCB_delta > 0 && HL_Rev_HL_delta > 0)
    {
      //计算温度值
  		//PCB_Temp = Get_Temp((uint8_t)(ATD_Value_u16[ADC_TEMP_AD] >> ADx_AD8_Coeff));

  		if(PCB_Temp <= (int8_t)(Default_Cal_data[CAL_HL_REVISE_PCB_L_TEMP]))
  			HL_Weak_Temp = Default_Cal_data[INFO_HL_REVISE_WEAK_L_TEMP];
  		else if(PCB_Temp >= (int8_t)(Default_Cal_data[CAL_HL_REVISE_PCB_H_TEMP]))
  			HL_Weak_Temp = Default_Cal_data[INFO_HL_HIGH_TEMP_WEAK];
  		else
  			HL_Weak_Temp=(uint8_t)((uint32_t)(HL_Rev_HL_delta*(PCB_Temp-(int8_t)(Default_Cal_data[CAL_HL_REVISE_PCB_L_TEMP])))/HL_Rev_PCB_delta) + Default_Cal_data[INFO_HL_REVISE_WEAK_L_TEMP];
    }
    else
    {
    	HL_Weak_Temp = Default_Cal_data[INFO_HL_HIGH_TEMP_WEAK];
    }

    HL_High_Temp_Divisor=Calculate_Percent(Default_Cal_data[INFO_HL_SHUTDOWN_TEMP],HL_Weak_Temp,Default_Cal_data[INFO_HL_HIGH_TEMP_WEAK_PERCENT]);

    if(Default_Cal_data[CAL_HIGH_TEMP_WEAK_PERCENT] != 0)
  		Motor_High_Temp_Divisor=Calculate_Percent(Default_Cal_data[CAL_HIGH_TEMP_CUT],Default_Cal_data[CAL_HIGH_TEMP_WEAK_START],Default_Cal_data[CAL_HIGH_TEMP_WEAK_PERCENT]);
    else
    	Motor_High_Temp_Divisor=0;
}
/*==========================================================================
* 函数名:Temperature_Weaking
* 功能描述:温度弱化、低压弱化、双曲线弱化、堵转弱化转矩电流
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
APP_SECTION void Temperature_Weaking(void)
{
	static uint16_t PCB_OutMax=0,HL_OutMax=0;
	uint32_t Out_Max=0;

	/*********************************************************************************
														 温度弱化
	*********************************************************************************/
	//计算温度值
	if(PCB_En == 1)
		PCB_Temp = Get_Temp(ATD_Value_u8[ADC_TEMP_AD]);
	else
		PCB_Temp = 25;
	HL_Temp   = Get_HL_Temp(ATD_Value_u8[ADC_HS_TEMP_AD]);

	/*********************输出赋初值***************************/
	if(Default_Cal_data[CAL_CURRENT_PERCENT] < 100)
		Out_Max = (uint32_t)(OUTMAX_Q14 * Default_Cal_data[CAL_CURRENT_PERCENT]) / 100;
	else
		Out_Max = OUTMAX_Q14;

	PCB_OutMax = Out_Max;
	HL_OutMax = Out_Max;
	/*********************高低端切断，恢复，弱化***************************/

	//高低端切断与恢复 //高低端120°C关断，AD值为94
	if(HL_Temp>=Default_Cal_data[INFO_HL_SHUTDOWN_TEMP])
	{
		OverTemp=1;
		HL_OutMax=0;
		CloseAll();
	}

	//高低端过温需恢复为110°C    PCB过温需恢复为90°C
	if((OverTemp && HL_Temp>=Default_Cal_data[INFO_HL_RESUME_TEMP])||( OverTemp && PCB_Temp>Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_RESUME_TEMP]))
	{
		HL_OutMax=0;
		CloseAll();
	}
	else if(OverTemp && HL_Temp < Default_Cal_data[INFO_HL_RESUME_TEMP] && PCB_Temp < Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_RESUME_TEMP])//恢复
	{
		if(TPSx_Output <= 0 && Stop_Flag == 1)
			OverTemp=0;
	}
	//高低端弱化    90°~120°，弱化70%    //90C   153AD                                   120C      94AD
	//高低端弱化    50°~120°，弱化70%    //90C   153AD                                   120C      94AD
	if((HL_Temp>=HL_Weak_Temp)&&(HL_Temp<Default_Cal_data[INFO_HL_SHUTDOWN_TEMP]))
	{
		//if(HL_Temp > 80)
			//TempWarn=1;  //80C温度警告
		if(HL_High_Temp_Divisor!=0)
			HL_OutMax = HL_OutMax-(uint16_t)((uint32_t)(HL_OutMax*(uint8_t)(HL_Temp-HL_Weak_Temp))/HL_High_Temp_Divisor);
		else
			HL_OutMax = 0;
	}

	/*********************PCB切断与恢复***************************/
	//PCB 110°切断
	if(PCB_Temp>Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_TEMP])    // Disable both field and armature
	{									//110
		OverTemp=1;
		PCB_OutMax=0;
		CloseAll();

	}
	//PCB 90°恢复温度
	if(OverTemp && PCB_Temp>Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_RESUME_TEMP])
	{												//90
		PCB_OutMax=0;
		CloseAll();

	}
	else if(OverTemp && HL_Temp < Default_Cal_data[INFO_HL_RESUME_TEMP] && PCB_Temp < Default_Cal_data[INFO_CONTROLLER_SHUTDOWN_RESUME_TEMP])//恢复
	{
		if(TPSx_Output <= 0 && Stop_Flag == 1)
			OverTemp=0;
	}
	/*********************PCB弱化***************************/
	//100~110°，削去35%，再弱化50%
	if(PCB_Temp>Default_Cal_data[INFO_HIGH_WEKEN_TEMP]) //reduce max current
	{							                     //100
		//TempWarn=1;
		PCB_OutMax -=(uint32_t)(PCB_OutMax*Default_Cal_data[INFO_OUTPUT_WEAK_PERCENT])/100;
		if(High_Temp_Divisor!=0)
			PCB_OutMax -= (uint32_t)(PCB_OutMax*(uint8_t)(PCB_Temp-Default_Cal_data[INFO_HIGH_WEKEN_TEMP]))/High_Temp_Divisor;
		else
			PCB_OutMax = 0;
	}
	//80~100°弱化35%
	else if(PCB_Temp>=Default_Cal_data[INFO_WENKEN_START_TEMP])//reduce max current that 30 percent of the between xxC and 90C
	{								  						//80				                                                //100
		if(Output_Weak_Divisor!=0)
			PCB_OutMax -= (uint32_t)(PCB_OutMax*(uint8_t)(PCB_Temp-Default_Cal_data[INFO_WENKEN_START_TEMP]))/Output_Weak_Divisor;//Output_Weak_Divisor;
		else
			PCB_OutMax = 0;
	}
	//50~80°不弱化//
	/*0~50° 弱化30%，已去除
	else if(PCB_Temp>=Default_Cal_data[INFO_HIGH_MEASURE_TEMP] && PCB_Temp<Default_Cal_data[INFO_OUTPUT_WEAKEN_TEMP]) //reduce max current that 20 percent of the between xxC and 20C
	{                                            //0                                  //50
		PCB_OutMax -= (uint16_t)(PCB_OutMax*(uint8_t)(Default_Cal_data[INFO_OUTPUT_WEAKEN_TEMP]-PCB_Temp))/High_Measure_divisor;
	}*/

	//-40°~0°，削去30%，再弱化40%
	else if(PCB_Temp<Default_Cal_data[INFO_HIGH_MEASURE_TEMP])//Compensate Ron reduction at cold ,reduce max current that 50 percent of the between 20C and -40C
	{
		if(Low_Measure_divisor!=0)                                    //0
			PCB_OutMax -= (uint32_t)(PCB_OutMax*(uint8_t)(Default_Cal_data[INFO_HIGH_MEASURE_TEMP]-PCB_Temp))/Low_Measure_divisor;
		else
			PCB_OutMax = 0;
	}
	//如果温度回归到正常区间，清除温度报警错误
	//if ((HL_Temp<=80)&&(PCB_Temp<=Default_Cal_data[INFO_HIGH_WEKEN_TEMP]))
	//if ((HL_Temp<=HL_Weak_Temp)&&(PCB_Temp<=Default_Cal_data[INFO_HIGH_WEKEN_TEMP]))
		//TempWarn=0;                         //90                                     //100

	//经过弱化后的最小电流
	Temp_OutMax = MIN(MIN(PCB_OutMax,HL_OutMax),MT_OutMax);

	if(Temp_OutMax < (OUTMAX_Q14 >> 2))   //最小25%//温度弱化后的最大输出
		Temp_OutMax = (OUTMAX_Q14 >> 2);
}

/*==========================================================================
* 函数名:Motor_Temp_Handle
* 功能描述:电机温度处理函数
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
APP_SECTION void Motor_Temp_Handle(void)
{
	static int16_t Motor_Temp=0;
	//电机过温切断和恢复
	MT_OutMax = OUTMAX_Q14;   //输出赋初值
	if(Default_Cal_data[CAL_HIGH_TEMP_EN]!=0)
	{
		if(ATD_Value_u8[ADC_MOT1_AD] <= 230)
		{
			if(Default_Cal_data[CAL_HIGH_TEMP_EN] == 1)
				Motor_Temp1 = Get_KTY84_Temp(ATD_Value_u16[ADC_MOT1_AD]);			//10bit  AD
			else
				Motor_Temp1 = Get_KTY83_Temp(ATD_Value_u16[ADC_MOT1_AD]);

			Tr_MTemp1 = Motor_Temp1;

			if(Motor_Temp1 >= (int16_t)Default_Cal_data[CAL_HIGH_TEMP_CUT])
				Motor_Temp_Err1=1;
			else if(Motor_Temp1 < (int16_t)Default_Cal_data[CAL_HIGH_TEMP_RESUME])
			{
				if(Temp_T_TPSx <= 0 && Stop_Flag == 1)
					Motor_Temp_Err1=0;
			}
		}
		else
		{
			Motor_Temp_Err1=0;
			Tr_MTemp1 = 21;
		}
	}
	else
	{
		Tr_MTemp1   = 21;
    Motor_Temp1 = 0;
	}

	Motor_Temp = Motor_Temp1;

	if(Motor_Temp_Err1 == 1)
		MT_OutMax = 0;
	if(Motor_High_Temp_Divisor != 0)
	{
		if((Motor_Temp >= (int16_t)Default_Cal_data[CAL_HIGH_TEMP_WEAK_START])&&(Motor_Temp < (int16_t)Default_Cal_data[CAL_HIGH_TEMP_CUT]))
		{
				MT_OutMax = MT_OutMax-(uint16_t)((uint32_t)(MT_OutMax*(uint16_t)(Motor_Temp-(int16_t)Default_Cal_data[CAL_HIGH_TEMP_WEAK_START]))/Motor_High_Temp_Divisor);
		}
	}
}
/*==========================================================================
* 函数名:Get_Temp
* 功能描述:板温计算函数
*
* 参数:  Count
* 参数解释:采样值
*
* 返回值:温度
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION int8_t Get_Temp(uint8_t Count)
{
	uint8_t m=0,n=0;
	int8_t Temp=0;
	//check valid ATD value
	if(Count<=8)
		return 115;	//If temperature higher than 115C, use 115C.
	else
	{
		m=Count-8;
		n=m>>3;       //Divide by 8, get array member
		Temp=Temperature_Array[n]-((Temperature_Array[n]-Temperature_Array[(uint8_t)(n+1)])*(m&0x7)>>3);
		Temp+=(int8_t)Default_Cal_data[INFO_TEMPERATURE_TRIM];//Temperature Trim

		return(Temp);
	}
}
/*==========================================================================
* 函数名:Get_HL_Temp
* 功能描述:高低端温度计算函数
*
* 参数:  Count
* 参数解释:采样值
*
* 返回值:温度
*
* 作 者:
* 修改记录:
*
*===========================================================================
*/
APP_SECTION uint8_t Get_HL_Temp(uint8_t Count)     //30℃以下不精确
{
	uint8_t Temp=0;
	//check valid ATD value
	if((Count>=0) && (Count<=2))
	  return  150;
  else if((Count>2)&&(Count<80))
    Temp=130;
	else if((Count>=80)&&(Count<=251))
	{
	  Count=Count-80;
		Temp=(unsigned char)HL_Temp_Array[Count];
  }
  else if((Count>251)&&(Count<=255))
    Temp=0;

	return(Temp);
}

/*
const int16_t KTY84_Temp_Array[15]={-30,-12,6,24,42,61,80,100,121,143,167,193,222,254,292}; //11位AD对应的温度

//KTY84-150
int16_t Get_KTY84_Temp(uint16_t Count)
{
	uint8_t n=0;
	uint16_t m=0,temp_ad;
	int16_t Temp=0;

	//check valid ATD value
	temp_ad=(uint32_t)(Count*128)/ATD_Value_u8[ADC_VS_AD];

	if(temp_ad<575)
		Temp=-30;	//If temperature lower than -30C, use -30C.
	else if((temp_ad>=575)&&(temp_ad<=1471))
	{
		m=temp_ad-575;
		n=m>>6;       //Divide by 64, get array member
		Temp=KTY84_Temp_Array[n]+((KTY84_Temp_Array[(uint8_t)(n+1)]-KTY84_Temp_Array[n])*(m&0x3F)>>6);
		Temp+=(int8_t)Default_Cal_data[INFO_TEMPERATURE_TRIM];//Temperature Trim
	}
	else
	 	Temp=292;

	return(Temp);
}
*/

/*//KTY84-150 //8位AD对应的温度
int16_t Get_KTY84_Temp(uint16_t Count)
{
	uint8_t m=0,n=0;
	uint16_t temp_ad;
	int16_t Temp=0;

	//check valid ATD value
	temp_ad=(uint32_t)(Count*128)/ATD_Value_u8[ADC_VS_AD];

	if(temp_ad<73)
		Temp=-28;	//If temperature lower than -28C, use -28C.
	else if((temp_ad>=73)&&(temp_ad<=185))
	{
		m=temp_ad-73;
		n=m>>3;       //Divide by 8, get array member
		Temp=KTY84_Temp_Array[n]+((KTY84_Temp_Array[(uint8_t)(n+1)]-KTY84_Temp_Array[n])*(m&0x7)>>3);
		Temp+=(int8_t)Default_Cal_data[INFO_TEMPERATURE_TRIM];//Temperature Trim
	}
	else
	 	Temp=300;

	return(Temp);
}
*/

//KTY84-150 //10位AD对应的温度
APP_SECTION int16_t Get_KTY84_Temp(uint16_t Count)
{
	uint8_t n=0;
	uint16_t m=0,temp_ad;
	int16_t Temp=0;

	//check valid ATD value
	temp_ad=(uint32_t)(Count*512)/ATD_Value_u16[ADC_VS_AD];
	//temp_ad=Count;

	if(temp_ad<288)
		Temp=-30;	//If temperature lower than -30C, use -30C.
	else if((temp_ad>=288)&&(temp_ad<736))
	{
		m=temp_ad-288;
		n=m>>5;       //Divide by 32, get array member
		Temp=KTY84_Temp_Array[n]+((KTY84_Temp_Array[(uint8_t)(n+1)]-KTY84_Temp_Array[n])*(m&0x1F)>>5);
		Temp+=(int8_t)Default_Cal_data[INFO_TEMPERATURE_TRIM];//Temperature Trim
	}
	else
	 	Temp=293;

	return(Temp);
}


//############################################################################
//函数功能：83系列传感器采样电机温度
//函数名：Get_KTY83_Temp()
//
//描述：根据采样AD值推算出电机实际温度
//
//
//作者：
//时间：2012-07-03
//############################################################################
APP_SECTION int16_t Get_KTY83_Temp(uint16_t Count)
{

	uint8_t n=0;
	uint16_t m=0,temp_ad;
	int16_t Temp=0;

	//check valid ATD value
	temp_ad=(uint32_t)(Count*512)/ATD_Value_u16[ADC_VS_AD];
	//temp_ad=Count;

	if(temp_ad<398)
		Temp=-30;	//If temperature lower than -30C, use -30C.
	else if((temp_ad>=398)&&(temp_ad < 750))
	{
		m=temp_ad-398;
		n=m>>5;       //Divide by 32, get array member
		Temp=KTY84_Temp_Array[n]+((KTY84_Temp_Array[(uint8_t)(n+1)]-KTY84_Temp_Array[n])*(m&0x1F)>>5);
		Temp+=(int8_t)Default_Cal_data[INFO_TEMPERATURE_TRIM];//Temperature Trim
	}
	else
	 	Temp=190;

	return(Temp);
}

