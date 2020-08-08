/*
 * Angle_Sensor.c
 *
 *  Created on: 2018��11��8��
 *      Author: Administrator
 */
#include "Function.h"
#include "Variable.h"
#include "Platform.h"
#include "Motor.h"

/*****************************************************************************************
������IRSample_Coff
���ܣ�   ��������ȡ���������¶ȱ仯��ϵ����
������
����ֵ��
���ߣ�
ʱ�䣺
*****************************************************************************************/
APP_SECTION void IRSample_Coff(void)
{
  uint8_t T_coord_l=0,T_coord_h=0;
  uint16_t T_data_l=0;
  uint16_t K_LL=0,K_LH=0;

  T_data_l = u8SearchSides(HL_Temp,(uint16_t *)(&X_Coords),X_POINTS);
  T_coord_l = (uint8_t)(T_data_l>> 8);
  T_coord_h = (uint8_t)(T_data_l);;
  T_data_l  = X_Coords[T_coord_l];

  K_LL = IR_Coff[T_coord_l];
  K_LH = IR_Coff[T_coord_h];

  if(K_LL > K_LH)
    Temp_CF = K_LL - (uint16_t)((K_LL - K_LH) * (HL_Temp - T_data_l)) / 5;
  else
    Temp_CF = K_LL;
}

/*==========================================================================
* ������:Motor_Init
* ��������:�����ʼ��
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
APP_SECTION void Motor_Init(void)
{
	Current_Parms_Init();
	PID_Parms_Init();
	RPM_Parms_Init();
	IdentifyMotorParms(); //�������ʶ��ʵ��Ҫ�õ�������㡢��������Ƶ�ѹ��Ĭ�ϵ�����������ѹƵ�ʡ�J1939�ʹ���ͨ��
	if(Default_Cal_data[CAL_SPEED_SENSOR_TYPE] == HALL_SENSOR)
	{
		Hall_Angle_Parms_Init();   //�Ƕȳ�ʼ��
		Init_Hall_Cap(FTM2,PCC_FTM2_INDEX);
	}
}

/*==========================================================================
* ������:Current_Parms_Init
* ��������:�ɱ궨������ʼ��
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 7 �� 30 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Current_Parms_Init(void)
{
	uint16_t Id_Max=0;
	Current_Max_AD = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_SENSOR_RATED_AD_H])<<8)+Default_Cal_data[INFO_SENSOR_RATED_AD_L]);  // 12λAD
  Current_Max_Q  = Current_Max_AD << IAD_TO_Q15;   //��������Qֵ����
  HS_Rated_I = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_SENSOR_RATED_I_H])<<8)+Default_Cal_data[INFO_SENSOR_RATED_I_L]);
  //Id_Max = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_MIN_Imr_H])<<8)+Default_Cal_data[CAL_MIN_Imr_L]);
  Id_Min = -(int16_t)((uint32_t)(Current_Max_Q * Id_Max) / HS_Rated_I);
}

/*==========================================================================
* ������:Angle_Sensor_Err
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
APP_SECTION void Angle_Sensor_Err(void)
{
	if(Default_Cal_data[CAL_SPEED_SENSOR_TYPE] == HALL_SENSOR)
	{
		Hall_Sensor_Err();
	}
}

/*==========================================================================
* ������:AcquirePosition
* ��������:�Ƕȼ���
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
APP_SECTION void AcquirePosition(void)
{
	if(Default_Cal_data[CAL_SPEED_SENSOR_TYPE] == HALL_SENSOR)
	{
		Hall_Angle_Acc(&Hall_Str1,HALL_PORT1,Motor1.Cur_Mode);
		FOC_SinCos1.qAngle = Hall_Str1.qAngle & 0xffffff;
	}
}


/*==========================================================================
* ������:Calculate_Speed
* ��������:ת�ټ��㡢ȷ��ת��������
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
APP_SECTION void Calculate_Speed(uint32_t Cur_Speed, MOTOR_STR *pMotor,OBSERVER_STR *pObser)
{
  uint8_t i;
  uint32_t RPM_Sum,Temp;

  Temp = Cur_Speed;
	(*pMotor).Array[(*pMotor).Loop] = Temp;
	(*pMotor).Loop++;
	(*pMotor).Loop &= 0x07;

	RPM_Sum = 0;
	for(i = 0;i < 8;i++)
	{
		RPM_Sum += (*pMotor).Array[i];
	}

	Temp = (uint16_t)(RPM_Sum >> 3);    //ת���˲����������С�Ҳ�����С
	(*pMotor).Mot_AbsRPM = Temp;

	if((*pMotor).Cur_Mode == 0)
		(*pMotor).Mot_RPM = Temp;
	else
		(*pMotor).Mot_RPM = -Temp;

	(*pObser).Mot_RPM = (*pMotor).Mot_RPM;

	//���ݵ�ǰĸ�ߵ�ѹ���������ת��
	//if(ATD_Value_u16[ADC_B_AD] > 0)
	//	(*pMotor).RPM_Convert = (uint16_t)((uint32_t)(Voltage_Max_AD * (*pMotor).Mot_AbsRPM) / ATD_Value_u16[ADC_B_AD]);

}

APP_SECTION void FdWeakening(void)
{
	PIParmU.qInMeas = ParkParm_V1.Us_Q15;     //��ǰ
	PIParmU.qInRef  = 29491;//�趨
	CalcPI(&PIParmU);
	Id_delta    = (int16_t)(PIParmU.qOut);
	ParkParm_I1.IdRef = Id_delta;
}

APP_SECTION void CalcEMF(PSC_STR *pCtrl,OBSERVER_STR *pObser,MOTOR_STR *pMotor)
{
  int16_t V_err=0;
  uint32_t temp_EMF_qVq=0,Cap_Flag=0;
  uint32_t Va_AD = ATD_Value_u16[(*pCtrl).PA.VAD_CH];
  uint32_t Vb_AD = ATD_Value_u16[(*pCtrl).PB.VAD_CH];
  uint32_t Vc_AD = ATD_Value_u16[(*pCtrl).PC.VAD_CH];

  if((*pObser).Mot_RPM == 0)
	{
		(*pObser).Start_Allow = 1;
		(*pObser).EMF_qVq = 0;
		(*pObser).EMF_qVq_Abs = 0;
		(*pMotor).Cur_Mode = Rev_Mode;
		if((*pMotor).Cur_Mode == 0)
			(*pMotor).Cur_Dir = 1;
		else
			(*pMotor).Cur_Dir = -1;
		return;
	}

  if(ATD_Value_u16[ADC_B_AD] > 0)
  {
  	//���������ѹ���㷴����
    V_err = (int16_t)(Va_AD - Vb_AD);
    if((V_err < 5) && (V_err > -5) && (Vc_AD > Va_AD)) //Va=Vb,��Vc>Va,VcΪ���Ҳ��Ĳ���
    {
      temp_EMF_qVq = (uint32_t)((Vc_AD - Va_AD) * 37837) / ATD_Value_u16[ADC_B_AD];
      Cap_Flag = 1;
    }

    V_err = (int16_t)(Vb_AD - Vc_AD);
    if((V_err < 5) && (V_err > -5) && (Va_AD > Vb_AD)) //Va=Vb,��Vc>Va,VcΪ���Ҳ��Ĳ���
    {
      temp_EMF_qVq = (uint32_t)((Va_AD - Vb_AD) * 37837) / ATD_Value_u16[ADC_B_AD];
      Cap_Flag = 1;
    }

    V_err = (int16_t)(Vc_AD - Va_AD);
    if((V_err < 5) && (V_err > -5) && (Vb_AD > Vc_AD)) //Va=Vb,��Vc>Va,VcΪ���Ҳ��Ĳ���
    {
      temp_EMF_qVq = (uint32_t)((Vb_AD - Vc_AD) * 37837) / ATD_Value_u16[ADC_B_AD];
      Cap_Flag = 1;
    }

    if(Cap_Flag == 1)
    {
			if(temp_EMF_qVq < 32768)
			{
				(*pObser).EMF_qVq_Abs = temp_EMF_qVq;
				(*pObser).Start_Allow = 1;
			}
			else//��������Ƴ�����ص�ѹ�����������ν���������ź��ж����νӣ�
			{
				(*pObser).EMF_qVq_Abs = 32768;
				(*pObser).Start_Allow = 0;
			}

			if((*pMotor).Cur_Mode == 0)
				(*pObser).EMF_qVq = (*pObser).EMF_qVq_Abs;
			else
				(*pObser).EMF_qVq = -(*pObser).EMF_qVq_Abs;
    }
  }
  else
  {
  	(*pObser).EMF_qVq = 0;
  	(*pObser).EMF_qVq_Abs = 0;
  	(*pObser).Start_Allow = 0;
  }
}

/*==========================================================================
* ������:Pre_Drive
* ��������:������ʶ��Ԥ��λ����
*
* ����:Ԥ��λ��ת�ӵ�Ƕ�λ�ã��������ĵ�ѹʸ����λ��
* ��������:���ػ������룬�����λ����ȷ���ش���
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 12 �� 12 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION uint16_t Pre_Drive(uint32_t qAngle,uint8_t order,int16_t Target_Ia)
{
  uint32_t i=0,Sensor_Sts=0;
  int16_t IA_AD1;
  SIN_COS_STR Pre_SinCos1;
  static V_CS_STR Pre_V1;

	Pre_SinCos1.qAngle = qAngle;
  SinCos(&Pre_SinCos1, (int32_t *)(&sinetable));

  for(i=0;i<15000;i++)
  {
		IA_AD1 = Ctrl_1.PA.IAD_ZERO - ATD_Value_u16[Ctrl_1.PA.IAD_CH];

		if(Target_Ia > 0)
		{
			if(Target_Ia>IA_AD1)
				Pre_V1.Vd += 3;  //��ռ�ձ�
			else if(Target_Ia<IA_AD1)
				Pre_V1.Vd -= 3;  //��ռ�ձ�
		}
		else if(Target_Ia < 0)
		{
			if(Target_Ia<IA_AD1)
				Pre_V1.Vd += 3;  //��ռ�ձ�
			else if(Target_Ia>IA_AD1)
				Pre_V1.Vd -= 3;  //��ռ�ձ�
		}
		if(Pre_V1.Vd>10000) Pre_V1.Vd=10000;
		else if(Pre_V1.Vd<0) Pre_V1.Vd=0;


		CalcRefVec(&Pre_V1, &Pre_SinCos1);
		CalcSVGen(&Pre_V1, &PWM1);

		SET_32(Ctrl_1.PA.PWM_CH, PWM1.Cha);
		SET_32(Ctrl_1.PB.PWM_CH, PWM1.Chb);
		SET_32(Ctrl_1.PC.PWM_CH, PWM1.Chc);

		PWM_PORT = PWM1_EN;

  	Cpu_Delay100US();
	  WDOG_Feed();
    Call_ETS();
  }

  if(Default_Cal_data[CAL_SPEED_SENSOR_TYPE] == HALL_SENSOR)  //����ͨ���ȡ��������
  {
  	Sensor_Sts = (uint16_t)((uint16_t)(HALL_PORT1));
  }

	PWM_PORT = PWM1_DI;
	Pre_V1.Vd = 0;
	Pre_V1.Vq = 0;

	SET_32(Ctrl_1.PA.PWM_CH, 0);
	SET_32(Ctrl_1.PB.PWM_CH, 0);
	SET_32(Ctrl_1.PC.PWM_CH, 0);

	for(i=0;i<15000;i++)
	{
		Cpu_Delay100US();
		WDOG_Feed();
		Call_ETS();
	}

  return (Sensor_Sts);   //���ػ�������
  //return (5654);
}
/*==========================================================================
* ������:IdentifyMotorParms
* ��������:��ʶ�Ƕȴ�����
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 12 �� 12 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void IdentifyMotorParms(void)
{
  uint16_t NomCurrent = (uint16_t)(((uint16_t)(Default_Cal_data[CAL_NOM_CURRENT_H])<<8)+Default_Cal_data[CAL_NOM_CURRENT_L]);
  int16_t Target_pImax = (int16_t)((uint32_t)((uint32_t)(NomCurrent * Current_Max_AD)>>1)/HS_Rated_I);//ͨ�����Ϊ����������һ��
  int16_t Target_nImax = (int16_t)(-Target_pImax);
  int16_t Target_phalf = Target_pImax>>1;
  int16_t Target_nhalf = (int16_t)(-(Target_pImax >> 1));
  uint16_t Sensor_Order[7]={0,0,0,0,0,0,0},Dir_Order1[4]={0,0,0,0};

	uint8_t hall=0,check_sum=0,i=0,j=0;

	if(Default_Cal_data[INFO_IDENTIFY] == 0xAA)
	{
		Identify_Err1 = 0;
		//6��״̬��λ,������ˢ����ص��������Ӧ�Ļ�������
		//ͨ�����Ϊ����������һ��
		Sensor_Order[0] = (uint16_t)(Pre_Drive(ANGLE_300_Q24,0,Target_phalf));  //��һ��ͨ�綨λ��׼��ת��λ�õ�Ƕ�300�� ��ABC����״̬Ϊ101
		Sensor_Order[1] = (uint16_t)(Pre_Drive(ANGLE_0_Q24,1,Target_pImax));      //ת��λ�õ�Ƕ�0��   ��ABC����״̬Ϊ100
		Sensor_Order[2] = (uint16_t)(Pre_Drive(ANGLE_60_Q24,2,Target_phalf));  //ת��λ�õ�Ƕ�60��  ��ABC����״̬Ϊ110
		Sensor_Order[3] = (uint16_t)(Pre_Drive(ANGLE_120_Q24,3,Target_nhalf));  //ת��λ�õ�Ƕ�120�� ��ABC����״̬Ϊ010
		Sensor_Order[4] = (uint16_t)(Pre_Drive(ANGLE_180_Q24,4,Target_nImax));  //ת��λ�õ�Ƕ�180�� ��ABC����״̬Ϊ011
		Sensor_Order[5] = (uint16_t)(Pre_Drive(ANGLE_240_Q24,5,Target_nhalf));  //ת��λ�õ�Ƕ�240�� ��ABC����״̬Ϊ001
		Sensor_Order[6] = (uint16_t)(Pre_Drive(ANGLE_300_Q24,6,Target_phalf));  //ת��λ�õ�Ƕ�300�� ��ABC����״̬Ϊ101

		CloseAll();
		OpenFlash();

		if(Default_Cal_data[CAL_SPEED_SENSOR_TYPE] == HALL_SENSOR)  //����ͨ���ȡ��������
		{
			check_sum = 0;
			for(i=1;i<7;i++)
			{
				j=(uint8_t)(Sensor_Order[i]);
				check_sum += j;
				hall = (uint8_t)(Sensor_Order[i]^Sensor_Order[i-1]);
				if(!(hall == 1 || hall==2|| hall==4) || j>7)
					Identify_Err1 =1;

				if(hall == 4)
				{
					if((Sensor_Order[i] & 0x04) != 0)
					{
						Dir_Order1[0] = (uint8_t)(Sensor_Order[i]);
						Dir_Order1[3] = (uint8_t)(Sensor_Order[i-1]);
					}
					else
					{
						Dir_Order1[1] = (uint8_t)(Sensor_Order[i]);
						Dir_Order1[2] = (uint8_t)(Sensor_Order[i-1]);
					}
				}
			}
			if(check_sum != 21)
			 Identify_Err1 = 1;

			if(Identify_Err1 == 0)
			{
				Cal_data[CAL_SBC_Angle_0]  = (uint8_t)(Sensor_Order[1]);
				Cal_data[CAL_SBA_Angle_60] = (uint8_t)(Sensor_Order[2]);
				Cal_data[CAL_SCA_Angle_120]= (uint8_t)(Sensor_Order[3]);
				Cal_data[CAL_SCB_Angle_180]= (uint8_t)(Sensor_Order[4]);
				Cal_data[CAL_SAB_Angle_240]= (uint8_t)(Sensor_Order[5]);
				Cal_data[CAL_SAC_Angle_300]= (uint8_t)(Sensor_Order[6]);

				Cal_data[CAL_FWD_HallA_H]=(uint8_t)(Dir_Order1[0]);
				Cal_data[CAL_FWD_HallA_L]=(uint8_t)(Dir_Order1[1]);
				Cal_data[CAL_REV_HallA_H]=(uint8_t)(Dir_Order1[2]);
				Cal_data[CAL_REV_HallA_L]=(uint8_t)(Dir_Order1[3]);
			}
		}

		if(Identify_Err1 == 0)
		{
			Cal_data[INFO_IDENTIFY]=0x55;
			_DI();
			CloseFlash();
			for(;;);
		}

		Call_ETS();
		WDOG_Feed();
	}

}

