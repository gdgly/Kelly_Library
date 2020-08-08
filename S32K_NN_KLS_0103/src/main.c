/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "S32K142.h" /* include peripheral declarations S32K142 */
#include "clocks_and_modes.h"
#include "s32_core_cm4.h"
#include "typedefine.h"
#include "Function.h"
#include "Events.h"
#include "Variable.h"
#include "Bootloader.h"
#include "CAN.h"
#include "Motor.h"

static uint8_t Send_ok1=0,Send_ok2=0;

int main(void)
{
  if(Default_Cal_data[INFO_BOOTLOADER]==0xFF)//Burn Flash Mode
  {
    boot();
  }
  else//normally mode
  {
  	Main_Function();
  }
}
APP_SECTION void Main_Function(void)
{
		uint32_t i=0,StartUp_WtTi=0;
		/**************************************************************************************************
																	系统初始化
		***************************************************************************************************/
		_DI();
		disable_WDOG();

		SOSC_init_8MHz();
		NormalRUNmode_80MHz();//初始化时钟
		//HSRUNmode_112MHz();

		Memory_Init();//初始化内存变量
		MCU_Init();//初始化中断和外设
		CANInit(1,500,1,(uint32_t *)(&ID_table));
		CloseAll();//切断所有输出
		_EI();

		/**************************************************************************************************
																	复位延时 控制器初始化
		***************************************************************************************************/
		for(i=0;i<1000;i++) //非上电复位后全局变量均被清零,延时让部分变量值刷新,等待电池电压稳定,等待采样值更新
		{										//延时一段时间，保证采样到准确的转速值，传入油门计算和PID
			Cpu_Delay100US();
			WDOG_Feed();
		}
		Reset_Handle();								//判断单片机复位源,进行复位处理

		Function_Init();              //控制器功能标定
		Motor_Init();
		Temp_Parms_Init();
		Vol_Parms_Init();
		Pedal_Parms_Init();

		/*********************************************************************************
															上电状态检测
		*********************************************************************************/
		StartUp_WtTi = (uint16_t)(Default_Cal_data[CAL_STARTUP_WAIT_TIME] * 800 + 1);//CAL_STARTUP_WAIT_TIME unit is 0.1 sec

		if(POR_RST !=0 )//退出复位后，若F_POR,F_LVD12或F_LVD27_VREG标志置位
		{																																//则软件需认其为上电复位
			for(i=0;i<StartUp_WtTi;i++)//delay awhile for get B+AD
			{
				Cpu_Delay100US();
				WDOG_Feed();
			}
		}

		POR_Status_Detect();
		Motor_Temp_Handle();

		/*********************************************************************************
																			 进入主循环
		*********************************************************************************/
		WDOG_Feed();
		Task500us_En = 1;   //使能调度程序

		for(;;)
		{
			for(i=0;i<ATD_CHAN;i++)
			{
				ATD_Value_u8[i] = (uint8_t)(ATD_Value_u16[i] >> ADx_AD8_Coeff);
			}

			/*********************************************************************************
																		有错退出输出
			 *********************************************************************************/
			Angle_Sensor_Err();
			V14_Err=0;
			if(ATD_Value_u8[ADC_V_AD] > 250 || ATD_Value_u8[ADC_V_AD] < 153)//error if beyond 9V to 15V
			{
				V14_Err   = 1;              //置位V+错误
				COIL_OUT = FALSE;          //关闭继电器输出
				Global_En = 0;
				CloseAll();                 //关闭所有输出
			}
			else
			{

				if(OverVolt_Err ||TPS_Error ||LowVolt_Err  || OverTemp || Reset_Err /*|| NTL_Error*/ ||
					Identify_Err1 || MOS_OverTemp1 || Motor_Temp_Err1 || Cur_Sensor_Err1 || Pos_Sensor_Err1)
				{
					Global_En = 0;
					CloseAll();
				}
				else
					Global_En=1;
			}

			Task_Scheduler();
			WDOG_Feed();

			Call_ETS();
			WDOG_Feed();
		}
    /* to avoid the warning message for GHS and IAR: statement is unreachable*/
		for(;;);
}

APP_SECTION void Task_1ms(void)
{
  static uint8_t Counter=0;

  Counter++;

  if(Counter % 2 != 0)
  {
  	TPSx_Output = Acc_Percent(ATD_Value_u16[ADC_TPS1_AD]); //由油门AD计算出对应的转速 TPSx_Output
		BRAKEx = Brake_Percent(ATD_Value_u16[ADC_BRK1_AD]);
    Releas_Brake_Disable_HP();                //退出刹车禁止高踏板，如触发错误则置TPSx_Output=0
    Rev_Status_Check(); //换向动作检测

    if(Global_En == 1)
    	Calc_TqeMd_Ref();  //计算不同状态下的转矩命令值和目标转速
    else
    {
      TPSTorque.Ref = 0;
      BRKTorque.Ref = 0;
    }
  }
  else
  {
		Calculate_Speed(Hall_Str1.CurSpeed,&Motor1,&Obser1);    //输出当前转速Speed_RPM

    if(Global_En == 0)
    {
      TPSx_Output = 0;
    }
    //对TPSx_Output进行缓冲平滑操作，加速率，刹车率，释放率等起作用，得到目标转矩，判断是否进入防溜状态
    Calc_TqeMd_Tgt();
  }
}

APP_SECTION void Task_2msA(void)
{
  Temperature_Weaking();
  Voltage_Weaking();
  IRSample_Coff();
}

APP_SECTION void Task_2msB(void)
{
  uint8_t Temp_Data[8]={0,0,0,0,0,0,0,0};
  uint32_t temp_Vs_32=0,ID,Datalen;
  static uint8_t Counter=0;

  if(CANRecData(1,(uint32_t*)(&ID),(uint8_t*)(&Datalen),Temp_Data) == 0)
  	CANSendData(1,1, ID,Datalen,Temp_Data);

  if(M_En && Global_En)//2ms做一次电压环，计算励磁电流给定值的修正量Id_delta，与速度环错开，减少中断执行时间
	{
		//为保证temp_Vs_32数据不溢出,Vqmax<=56756,Vdmax<=32767
		temp_Vs_32 = (uint32_t)(ParkParm_V1.Vq*ParkParm_V1.Vq) + (uint32_t)(ParkParm_V1.Vd*ParkParm_V1.Vd);
		ParkParm_V1.Us_Q15 = sqrt_16(temp_Vs_32);
	}

  Counter++;
  if(Counter >= 51)
  {
    Counter = 1;
  }

  if(Counter % 5 == 0)  //10ms
  {
    Get_B_Voltage();
  }

  if(Counter == 1 || Counter  == 27 || Send_ok1 != 0)
  {
#if	KELLY_CAN == 0
		//Send_ok1 = CAN1_SendFrame((uint32_t)0x10F8109A,0,8,Temp_Data);
#else
		//Send_ok1 = CAN1_SendFrame((uint32_t)0x0CF11F05,0,8,Temp_Data);
#endif
  }
  if(Counter == 13 || Counter  == 38 || Send_ok2 != 0)
  {
#if KELLY_CAN == 0
		//Send_ok2 = CAN1_SendFrame((uint32_t)0x10F8108D,0,8,Temp_Data);
#else
		//Send_ok2 = CAN1_SendFrame((uint32_t)0x0CF11E05,0,8,Temp_Data);
#endif
  }
  if(Counter == 3)
  {
  }
  else if(Counter == 7)
  {
  }
  else if(Counter == 17)
  {
   // LED_Process();
  }
#if KELLY_CAN == 0
  else if(Counter == 22)
	{
		//Alarm_Process();
	}
#endif
  else if(Counter == 33)
  {

  }
}

APP_SECTION void Task_100ms(void)
{
	//油门>0，或 非防溜输出状态 进行堵转判断
  if(TPSx_Output > 0)
  {
    if(StandstillCount1 <= 20)
      StandstillCount1++;

		if(StandstillCount1 >= 20)    //超过2s没有速度脉冲信号到来，视为堵转
			StandStill_Err1 = 1;
		else
			StandStill_Err1 = 0;
  }
  else
  {
    StandstillCount1 = 0;
    StandStill_Err1  = 0;
  }

  Motor_Temp_Handle();
  Voltage_Monitor();
}
