/* ###################################################################
**     Filename    : Events.c
**     Project     : KBS
**     Processor   : SKEAZN64MLH2
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-07-22, 17:04, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Events.h"
#include "Variable.h"
#include "Function.h"
#include "Platform.h"
#include "Motor.h"

#ifdef __cplusplus
extern "C" {
#endif 
	
static uint8_t  Scheduler_Status = TASK_SCHEDULER_OK,Scheduler_Thread_ID,Scheduler_Thread_ID_Backup;

APP_SECTION void ISR_Timer (void)
{
	Get_ATD_Data(1);

	if(Default_Cal_data[INFO_IDENTIFY] != 0xAA)
		Vector_Control();

	//LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
	(void)FTM0->SC;
	FTM0->SC = FTM0->SC & (0xfffffdff);
}


APP_SECTION void Get_ATD_Data(uint32_t TMIR_En)
{
	static uint32_t Task_Count=0,CNT_100us=0;
	uint32_t i=0;
	PDB1->CH[0].C1 = 0x00fe00ff;
	PDB1->CH[1].C1 =0x00;
	PDB1->SC |= PDB_SC_SWTRIG_MASK;

	//////////////////////////////////////////////////////////////////////////
		if(TMIR_En == 1)//慢AD采样
		{
			ADC0->SC1[0] &= ~ADC_SC1_ADCH_MASK;	      /* Clear prior ADCH bits */
			ADC0->SC1[0] = ADC_SC1_ADCH(Task_Count);
				//系统调度
				if(Task500us_En == 1)
				{
					CNT_100us++;
					if(CNT_100us >= 5)
					{
						CNT_100us = 0;
						Task_500US();
					}
				}

			while((ADC0->SC1[0] & ADC_SC1_COCO_MASK)==0){}
			ATD_Value_u16[Task_Count] = ADC0->R[0];
			Task_Count++;
			Task_Count &= 0x07;
		}
  ///////////////////////////////////////////////////////////////////////////

	while((ADC1->SC1[7] & ADC_SC1_COCO_MASK)==0){}
	for(i=0;i<8;i++)
		ATD_Value_u16[8+i] =  ADC1->R[i];
}

APP_SECTION void Vector_Control(void)
{
	int32_t IA=0,IB=0,IC=0;
	//计算IA
	IA = (int32_t)(Ctrl_1.PA.IAD_ZERO - ATD_Value_u16[Ctrl_1.PA.IAD_CH]);
	IB = (int32_t)(Ctrl_1.PB.IAD_ZERO - ATD_Value_u16[Ctrl_1.PB.IAD_CH]);
	IC = (int32_t)(Ctrl_1.PC.IAD_ZERO - ATD_Value_u16[Ctrl_1.PC.IAD_CH]);

	if(PWM1.Cha > PWM1.PE70)
		IA = (int16_t)(-(int16_t)(IB + IC));
	if(PWM1.Chb > PWM1.PE70)
		IB = (int16_t)(-(int16_t)(IA + IC));

	ParkParm_I1.Ia = (int32_t)(IA * Temp_CF) >> 3;
	ParkParm_I1.Ib = (int32_t)(IB * Temp_CF) >> 3;

	if( !M_En || !Global_En )//非驱动非发电状态
	{
		PWM_PORT = PWM1_DI;
		PWM1.Cha = PWM1.Chb = PWM1.Chc = 0;// 0%

		PIParmD1.qdSum = 0;
		PIParmD1.qOut = 0;      //保证第一次进入电流D环时能进行PI调节
		PIParmQ1.qdSum = 0;
		PIParmQ1.qOut = 0;      //保证第一次进入电流Q环时能进行PI调节
		PIParmQref1.qdSum = 0;
		PIParmQref1.qOut = 0;   //保证第一次进入速度环时能进行PI调节
		PIParmU.qdSum = 0;
		PIParmU.qOut = 0;

		ParkParm_V1.Vq = 0;
		ParkParm_V1.Vd = 0;
		ParkParm_V1.Us_Q15 = 0;
		ParkParm_I1.IqRef = 0;
		ParkParm_I1.Iq = 0;
		ParkParm_I1.IdRef = 0;
		ParkParm_I1.Id = 0;
		Id_delta = 0;

		CalcEMF(&Ctrl_1,&Obser1,&Motor1);
		ParkParm_V1.Vq = (int32_t)(Obser1.EMF_qVq);
		PIParmQ1.qdSum = (int32_t)(Obser1.EMF_qVq << 13);
		AcquirePosition();//计算磁链角
		SinCos(&FOC_SinCos1, (int32_t *)(&sinetable));

		if(Motor1.Mot_AbsRPM == 0)  //非输出状态，如果电机转速为零,则将电机实际转向置为给定转向，保证输出衔接平稳
		{
			Stop_Flag=1;
			Motor1.Cur_Mode = Rev_Mode;
			if(Motor1.Cur_Mode == 0)
				Motor1.Cur_Dir = 1;
			else
				Motor1.Cur_Dir = -1;
		}
		else
			Stop_Flag=0;
	}
	else
	{
		AcquirePosition();//计算磁链角
		SinCos(&FOC_SinCos1, (int32_t *)(&sinetable));// Calculate qSin,qCos from qAngle
		ClarkePark(&ParkParm_I1, &FOC_SinCos1);
		Control();
		// Calculate qValpha, qVbeta from qSin,qCos,qVd,qVq
		// Calculate Vr1,Vr2,Vr3 from qValpha, qVbeta
		CalcRefVec(&ParkParm_V1, &FOC_SinCos1);
		// Calculate and set PWM duty cycles from Vr1,Vr2,Vr3
		CalcSVGen(&ParkParm_V1, &PWM1);
		PWM_PORT = PWM1_EN;
	}
	SET_32(Ctrl_1.PA.PWM_CH, PWM1.Cha);
	SET_32(Ctrl_1.PB.PWM_CH, PWM1.Chb);
	SET_32(Ctrl_1.PC.PWM_CH, PWM1.Chc);
}

/*==========================================================================
* 函数名:Task_500US
* 功能描述:500us定时函数，决定要调度任务的ID
*
* 参数:    
* 参数解释:
*
* 返回值:
*
* 作 者:    Ted ,2012 年 8 月 13 日      
* 修改记录: 
*
*===========================================================================
*/
APP_SECTION void Task_500US(void)   
{
  static uint8_t u8Scheduler_Counter = 0,u8_100ms_Counter = 0,u8_50ms_Counter = 0,u8_10ms_Counter = 0;

  u8Scheduler_Counter++;

  if((u8Scheduler_Counter & 0x01) == 0x01)
  {
    u8_100ms_Counter++;

    if(u8_100ms_Counter >= 100)
    {
      Scheduler_Thread_ID = TASKS_100_MS;
      
      u8_100ms_Counter    = 0;
    }
    else
    {
      Scheduler_Thread_ID = TASKS_1_MS;
    }
  }
  else
  {
    if((u8Scheduler_Counter & 0x02) == 0x02)
    {
      u8_50ms_Counter++;

      if(u8_50ms_Counter >= 25)
      {
        Scheduler_Thread_ID = TASKS_50_MS;

        u8_50ms_Counter     = 0;
      }
      else
      {
        Scheduler_Thread_ID = TASKS_2_MS_A; 
      }
    }
    else
    {
      if((u8Scheduler_Counter & 0x03) == 0x00)
      {
        u8_10ms_Counter++;

        if(u8_10ms_Counter >= 5)
        {
          Scheduler_Thread_ID = TASKS_10_MS;

          u8_10ms_Counter     = 0;
        }
        else
        {
          Scheduler_Thread_ID = TASKS_2_MS_B;
        }
      }
    }
  }
}
/*==========================================================================
* 函数名:Task_Scheduler
* 功能描述:任务调度函数，根据任务的ID调度当前的任务
*
* 参数:    
* 参数解释:
*
* 返回值:
*
* 作 者:    Ted ,2012 年 8 月 13 日      
* 修改记录: 
*
*===========================================================================
*/
APP_SECTION void Task_Scheduler(void)                                            //函数中只用了1ms和10ms任务，其余是为功能扩展做备用
{
  switch(Scheduler_Thread_ID)
  {
    case 0:
      break;
      
    case 1:
    case 2:
    {
      Scheduler_Thread_ID_Backup = Scheduler_Thread_ID;                  //用来检测任务的执行时间是否在500us内完成

      EXECUTE_1MS_TASKS();

      if(Scheduler_Thread_ID == TASKS_100_MS)
      {
        EXECUTE_100MS_TASKS();
      }

      if(Scheduler_Thread_ID_Backup == Scheduler_Thread_ID)              //500us内完成任务调度
      {
      	Scheduler_Thread_ID = NO_TASK_PENDING;
      } 
      else
      {
        Scheduler_Status = TASK_SCHEDULER_OVERLOAD_1MS;
      }
    }
    break;
      
    case 3:
    case 4:
    {
      Scheduler_Thread_ID_Backup = Scheduler_Thread_ID;

      EXECUTE_2MS_A_TASKS();

      if(Scheduler_Thread_ID == TASKS_50_MS)
      {
        EXECUTE_50MS_TASKS();
      }

      if(Scheduler_Thread_ID_Backup == Scheduler_Thread_ID)
      {
        Scheduler_Thread_ID = NO_TASK_PENDING;
      } 
      else
      {
        Scheduler_Status = TASK_SCHEDULER_OVERLOAD_2MS_A;
      }
    }
    break;
      
    case 5:
    case 6:
    {
      Scheduler_Thread_ID_Backup = Scheduler_Thread_ID;

      EXECUTE_2MS_B_TASKS();

      if(Scheduler_Thread_ID == TASKS_10_MS)
      {
        EXECUTE_10MS_TASKS();
      }

      if(Scheduler_Thread_ID_Backup == Scheduler_Thread_ID)
      {
        Scheduler_Thread_ID = NO_TASK_PENDING;
      } 
      else
      {
        Scheduler_Status = TASK_SCHEDULER_OVERLOAD_2MS_B;
      } 
    }
    break;
    
  }
  return;
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
