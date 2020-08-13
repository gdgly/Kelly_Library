/*
 * Hall.c
 *
 *  Created on: 2020��1��31��
 *      Author: jt
 */

#include "Events.h"
#include "Variable.h"
#include "Function.h"
#include "Platform.h"
#include "Motor.h"
static uint32_t AngFlux_qKdelta;
static uint32_t temp_speed;

/*==========================================================================
* ������:Hall_Angle_Parms_Init
* ��������:��ʼ���ǶȲ���
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2010 �� 9 �� 24 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Hall_Angle_Parms_Init(void)
{
  int32_t Swap_Phase1=0;
  //ÿ�����ƻ������ĽǶ�delta=�����ڽǶ�/(һ��������ʱ��/���ƻ�ʱ��)����100us���ƻ�Ϊ��,��׽Hall C�İ�����
  //�����ڽǶ�Ϊ1<<24,������ʱ��Ϊcap_value*2/fclk,���ƻ�ʱ��Ϊ1/PWM1.dloop��ֻ��cap_value����ת�ٱ仯��
  //delta=(1<<24)*fclk/PWM1.dloop/2/cap_value;����delta = AngFlux_qKdelta/cap_value;
	AngFlux_qKdelta = (uint32_t)((uint32_t)(dFosc / PWM1.dloop) << 24) / (2);
	//�ٶȹ�ʽrpm = 60s/������ʱ��/��������Default_Cal_data[CAL_MOTOR_POLES]Ϊ������*2
	//��rpm=60/(cap_value*2/fclk)/(Default_Cal_data[CAL_MOTOR_POLES]/2),
	//rpm = 60*fclk/Default_Cal_data[CAL_MOTOR_POLES]/cap_value
	//rpm = temp_speed/cap_value;
	temp_speed = (uint32_t)((uint32_t)(60 * (uint32_t)dFosc) / (uint32_t)(Default_Cal_data[CAL_MOTOR_POLES])); //80MHz 32��Ƶ

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+2]&0x40)!=0)//��1�ֽڵĵ�6λ
  	Swap_Phase1 = 1;         //�ı�����ת����
	if(Hall_Sensor_Init(&Hall_Str1,Swap_Phase1,CAL_SBC_Angle_0))
	{
		Pos_Sensor_Err1 = 1;
	}
	else
	{
		Hall_Str1.qAngle = Hall_Str1.Init_qAngle[HALL_PORT1];
		Pos_Sensor_Err1 = 0;
	}
}
APP_SECTION uint8_t Hall_Sensor_Init(HALL_STR *pHall,int32_t Swap_Phase,uint32_t BaseAddress)
{
	uint8_t check_sum=0,i=0,j=0,Init_Err=0;

	for(i=0;i<8;i++)//��ʼ�������Ƕ�����
		(*pHall).Init_qAngle[i] = 0x55;

	for(i=0;i<6;i++)
	{
		j=Default_Cal_data[BaseAddress + i];
		check_sum += j;
		if(j > 7)
			Init_Err = 1;
	}

	//����IDȫ�����У��ʶ���Ƿ���ȷ
	if(check_sum != 21)
		Init_Err = 1;

	if(Init_Err == 0)
	{
		if(Swap_Phase == 1)//���ʵ����ת����Ϳͻ���Ҫ�ķ���һ��ʱ��ע�˹��ܣ��ı䵱ǰ��ת����
		{
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress]    ] = (uint32_t)(ANGLE_120_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +1] ] = (uint32_t)(ANGLE_60_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +2] ] = (uint32_t)(ANGLE_0_Q24);

			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +3] ] = (uint32_t)(ANGLE_300_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +4] ] = (uint32_t)(ANGLE_240_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +5] ] = (uint32_t)(ANGLE_180_Q24);

			(*pHall).Init_Dir[2] = Default_Cal_data[BaseAddress +6];
			(*pHall).Init_Dir[3] = Default_Cal_data[BaseAddress +7];
			(*pHall).Init_Dir[0] = Default_Cal_data[BaseAddress +8];
			(*pHall).Init_Dir[1] = Default_Cal_data[BaseAddress +9];
		}
		else
		{
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress]  ] = (uint32_t)(ANGLE_0_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +1] ] = (uint32_t)(ANGLE_60_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +2] ] = (uint32_t)(ANGLE_120_Q24);

			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +3] ] = (uint32_t)(ANGLE_180_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +4] ] = (uint32_t)(ANGLE_240_Q24);
			(*pHall).Init_qAngle[ Default_Cal_data[BaseAddress +5] ] = (uint32_t)(ANGLE_300_Q24);

			(*pHall).Init_Dir[0] = Default_Cal_data[BaseAddress +6];
			(*pHall).Init_Dir[1] = Default_Cal_data[BaseAddress +7];
			(*pHall).Init_Dir[2] = Default_Cal_data[BaseAddress +8];
			(*pHall).Init_Dir[3] = Default_Cal_data[BaseAddress +9];
		}
	}

	return Init_Err;
}

APP_SECTION void ISR_FTM2_Ovf(void)
{
	Hall_Str1.Over++;

	if(Hall_Str1.Over > (OVERFLOW_MAX-1))       //����������󣬲��ټ�����Ĭ�ϳ���ֹͣ
	{
		Hall_Str1.Cap_Period = 0;
		Hall_Str1.Delta = 0;
		Hall_Str1.Last_Period = 0;
		Hall_Str1.CurSpeed=0;
		Hall_Str1.Sine_Mode = 0; //��Ϊ����ģʽ���Ա��´����ʱ���뷽������ģʽ
		Motor1.Mot_AbsRPM = 0;
		Motor1.Mot_RPM = 0;
		Hall_Str1.Over = OVERFLOW_MAX;
	}

	(void)FTM2->SC;
	FTM2->SC = FTM2->SC & (0xfffffdff);

}

APP_SECTION void ISR_HALL1_C(void)
{
	uint32_t Hall=HALL_PORT1;
	StandstillCount1 = 0;
	Hall_Cap_Calc(FTM2,&Hall_Str1,&Motor1,Hall);
	Hall_Correct_Angle(&Hall_Str1,Hall,Motor1.Cur_Mode);
	(void)FTM2->CONTROLS[4].CnSC;
	FTM2->CONTROLS[4].CnSC = FTM2->CONTROLS[4].CnSC & (0x0000ff7f);
}

APP_SECTION void ISR_HALL1_AB(void)
{
  StandstillCount1 = 0;
  Hall_Correct_Angle(&Hall_Str1,HALL_PORT1,Motor1.Cur_Mode);

	if((FTM2->CONTROLS[2].CnSC & 0x80) != 0)
		FTM2->CONTROLS[2].CnSC = FTM2->CONTROLS[2].CnSC & (0x0000ff7f);
	if((FTM2->CONTROLS[3].CnSC & 0x80) != 0)
		FTM2->CONTROLS[3].CnSC = FTM2->CONTROLS[3].CnSC & (0x0000ff7f);
}

APP_SECTION void Hall_Cap_Calc(FTM_Type * FTM,HALL_STR *pHall,MOTOR_STR *pMotor,uint32_t Hall_Value)
{
	uint32_t Current_Period=0;

	if(Hall_Value == (*pHall).Init_Dir[1] || Hall_Value == (*pHall).Init_Dir[0])
	{
		(*pMotor).Cur_Mode = 0;     //A��ǰB  ��ת
		(*pMotor).Cur_Dir = 1;
	}
	else if(Hall_Value == (*pHall).Init_Dir[3] || Hall_Value == (*pHall).Init_Dir[2])
	{
		(*pMotor).Cur_Mode = 1;     //A�ͺ�B  ��ת
		(*pMotor).Cur_Dir = -1;
	}

	Current_Period = (uint32_t)(FTM->CONTROLS[4].CnV + (uint32_t)((*pHall).Over << 16));
	(*pHall).Over = 0;

	if(Current_Period > 1000 && Current_Period < 655360)
	{
		if((*pHall).Last_Period == 0 || (*pMotor).Cur_Mode != Rev_Mode)
			(*pHall).Sine_Mode = 0;
		else
			(*pHall).Sine_Mode = 1;
		(*pHall).Cap_Period = (uint32_t)(Current_Period + (*pHall).Last_Period) >> 1;
		(*pHall).Delta = (uint32_t)((uint32_t)AngFlux_qKdelta / (*pHall).Cap_Period);
		(*pHall).Last_Period = Current_Period;
	}
	else
	{
		(*pHall).Sine_Mode = 0;
	}

	if((*pHall).Cap_Period != 0 && (*pHall).Over != OVERFLOW_MAX)
		(*pHall).CurSpeed=(uint16_t)((uint32_t)(temp_speed)/(*pHall).Cap_Period);
	else
		(*pHall).CurSpeed = 0;
}

/*==========================================================================
* ������:Hall_Sensor_Err
* ��������:�����������ж�
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2010 �� 9 �� 24 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Hall_Sensor_Err(void)
{
	if(Hall_Str1.Init_qAngle[HALL_PORT1] == 0x55)
	{
		Pos_Sensor_Err1=1;//�ر�������ұ��������ʹ���
		CloseAll();
	}
	else if(TPSx_Output <= 0 && Stop_Flag == 1)
	{
		Pos_Sensor_Err1 = 0;
	}
}

APP_SECTION void Hall_Correct_Angle(HALL_STR *pHall,uint32_t Hall_Value,int32_t Current_Mode)
{
	uint32_t Hall_qAngle = (*pHall).Init_qAngle[Hall_Value];
	if((*pHall).Sine_Mode == 0)   /////PMSM����״̬,������ģʽ
	{
		(*pHall).qAngle =Hall_qAngle;
	}
	else   //�Ѿ��л������Ҳ�����״̬  ���� ����״̬
	{
		if(Current_Mode == 0)
			(*pHall).qAngle_Hall = (uint32_t)(Hall_qAngle - ANGLE_30_Q24)& 0xffffff;
		else
			(*pHall).qAngle_Hall = (uint32_t)(Hall_qAngle + ANGLE_30_Q24)& 0xffffff;

		(*pHall).qAngle = (*pHall).qAngle_Hall;
	}
}

APP_SECTION void Hall_Angle_Acc(HALL_STR *pHall,uint32_t Hall_Value,int32_t Current_Mode)
{
	int16_t Err_qAngle = 0;

	if((*pHall).Sine_Mode==0)   /////PMSM����״̬,������ģʽ
	{
		(*pHall).qAngle = (*pHall).Init_qAngle[Hall_Value];
	}
	else   //�Ѿ��л������Ҳ�����״̬  ���� ����״̬
	{
		if(Current_Mode == 0)
			(*pHall).qAngle = (uint32_t)((*pHall).qAngle + (*pHall).Delta)& 0xffffff;
		else
			(*pHall).qAngle = (uint32_t)((*pHall).qAngle - (*pHall).Delta)& 0xffffff;

		Err_qAngle = (int16_t)((uint16_t)((*pHall).qAngle>>8) - (uint16_t)((*pHall).qAngle_Hall>>8));

		if(Err_qAngle > (int16_t)ANGLE_60_Q)//��һ��֮�ڵ��ۼ�ֵ�����޷�����ֹʧ��
		{
			(*pHall).qAngle = (uint32_t)((*pHall).qAngle_Hall + (uint32_t)ANGLE_60_Q24) & 0xffffff;
		}
		else if(Err_qAngle < (int16_t)(-ANGLE_60_Q))
		{
			(*pHall).qAngle = (uint32_t)((*pHall).qAngle_Hall - (uint32_t)ANGLE_60_Q24) & 0xffffff;
		}
	}
}
