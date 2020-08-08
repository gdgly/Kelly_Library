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
																	ϵͳ��ʼ��
		***************************************************************************************************/
		_DI();
		disable_WDOG();

		SOSC_init_8MHz();
		NormalRUNmode_80MHz();//��ʼ��ʱ��
		//HSRUNmode_112MHz();

		Memory_Init();//��ʼ���ڴ����
		MCU_Init();//��ʼ���жϺ�����
		CANInit(1,500,1,(uint32_t *)(&ID_table));
		CloseAll();//�ж��������
		_EI();

		/**************************************************************************************************
																	��λ��ʱ ��������ʼ��
		***************************************************************************************************/
		for(i=0;i<1000;i++) //���ϵ縴λ��ȫ�ֱ�����������,��ʱ�ò��ֱ���ֵˢ��,�ȴ���ص�ѹ�ȶ�,�ȴ�����ֵ����
		{										//��ʱһ��ʱ�䣬��֤������׼ȷ��ת��ֵ���������ż����PID
			Cpu_Delay100US();
			WDOG_Feed();
		}
		Reset_Handle();								//�жϵ�Ƭ����λԴ,���и�λ����

		Function_Init();              //���������ܱ궨
		Motor_Init();
		Temp_Parms_Init();
		Vol_Parms_Init();
		Pedal_Parms_Init();

		/*********************************************************************************
															�ϵ�״̬���
		*********************************************************************************/
		StartUp_WtTi = (uint16_t)(Default_Cal_data[CAL_STARTUP_WAIT_TIME] * 800 + 1);//CAL_STARTUP_WAIT_TIME unit is 0.1 sec

		if(POR_RST !=0 )//�˳���λ����F_POR,F_LVD12��F_LVD27_VREG��־��λ
		{																																//�����������Ϊ�ϵ縴λ
			for(i=0;i<StartUp_WtTi;i++)//delay awhile for get B+AD
			{
				Cpu_Delay100US();
				WDOG_Feed();
			}
		}

		POR_Status_Detect();
		Motor_Temp_Handle();

		/*********************************************************************************
																			 ������ѭ��
		*********************************************************************************/
		WDOG_Feed();
		Task500us_En = 1;   //ʹ�ܵ��ȳ���

		for(;;)
		{
			for(i=0;i<ATD_CHAN;i++)
			{
				ATD_Value_u8[i] = (uint8_t)(ATD_Value_u16[i] >> ADx_AD8_Coeff);
			}

			/*********************************************************************************
																		�д��˳����
			 *********************************************************************************/
			Angle_Sensor_Err();
			V14_Err=0;
			if(ATD_Value_u8[ADC_V_AD] > 250 || ATD_Value_u8[ADC_V_AD] < 153)//error if beyond 9V to 15V
			{
				V14_Err   = 1;              //��λV+����
				COIL_OUT = FALSE;          //�رռ̵������
				Global_En = 0;
				CloseAll();                 //�ر��������
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
  	TPSx_Output = Acc_Percent(ATD_Value_u16[ADC_TPS1_AD]); //������AD�������Ӧ��ת�� TPSx_Output
		BRAKEx = Brake_Percent(ATD_Value_u16[ADC_BRK1_AD]);
    Releas_Brake_Disable_HP();                //�˳�ɲ����ֹ��̤�壬�紥����������TPSx_Output=0
    Rev_Status_Check(); //���������

    if(Global_En == 1)
    	Calc_TqeMd_Ref();  //���㲻ͬ״̬�µ�ת������ֵ��Ŀ��ת��
    else
    {
      TPSTorque.Ref = 0;
      BRKTorque.Ref = 0;
    }
  }
  else
  {
		Calculate_Speed(Hall_Str1.CurSpeed,&Motor1,&Obser1);    //�����ǰת��Speed_RPM

    if(Global_En == 0)
    {
      TPSx_Output = 0;
    }
    //��TPSx_Output���л���ƽ�������������ʣ�ɲ���ʣ��ͷ��ʵ������ã��õ�Ŀ��ת�أ��ж��Ƿ�������״̬
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

  if(M_En && Global_En)//2ms��һ�ε�ѹ�����������ŵ�������ֵ��������Id_delta�����ٶȻ����������ж�ִ��ʱ��
	{
		//Ϊ��֤temp_Vs_32���ݲ����,Vqmax<=56756,Vdmax<=32767
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
	//����>0���� �Ƿ������״̬ ���ж�ת�ж�
  if(TPSx_Output > 0)
  {
    if(StandstillCount1 <= 20)
      StandstillCount1++;

		if(StandstillCount1 >= 20)    //����2sû���ٶ������źŵ�������Ϊ��ת
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
