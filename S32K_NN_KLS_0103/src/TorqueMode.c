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
* ������:RPM_Parms_Init
* ��������:������Ʋ�������
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
APP_SECTION void RPM_Parms_Init(void)
{
  uint16_t max_rpm_output=0,max_rpm_firmware=0,RPM_Constraint=0;
  uint16_t Max_Motor_RPM;
  /************************************************************
                           ת�ټ������
  *************************************************************/
  max_rpm_output = (uint32_t)(60 * (((uint16_t)(Default_Cal_data[INFO_MAX_OUTPUT_F_H])<<8)+Default_Cal_data[INFO_MAX_OUTPUT_F_L])) / (Default_Cal_data[CAL_MOTOR_POLES] >> 1);
  max_rpm_firmware = nmax_Firmware;
	RPM_Constraint = MIN(max_rpm_output,max_rpm_firmware);

  /************************************************************
                            �������
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

  //Calc_RefTorque����1s����500�Σ����ʱ궨�ȼ�Ϊ0.1
	T_Acc_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_ACCEL_TIME]);
	T_Acc_Rel_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_ACC_RLS_TIME]);
	T_BRK_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_BRAKE_TIME]);
	T_BRK_Rel_Rate = (uint32_t)(OUTMAX_Q14 << 7) / (50 * Default_Cal_data[CAL_T_BRAKE_RLS_TIME]);

  return;
}

/*==========================================================================
* ������:Rev_Status_Check
* ��������:�жϷ��򿪹�״̬
*
* ����:    
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 5 �� 30 ��
* �޸ļ�¼: 
*
*===========================================================================
*/
APP_SECTION void Rev_Status_Check(void)
{
 	static uint8_t Last_Neutral_Flag =0;
 	uint32_t Rev1_Allow=0;

	if(FWD_SW == REV_SW)                        //����״̬��ͬ���յ�
	{
		Neutral_Flag    = 1;
		TPSx_Output     = 0;                       //��������
		StartupStatus   = STARTUP_NORMALLY;        //�յ�״̬�²�����̤��
		Startup_TPS_Err = 0;
		NTL_Error = 0;                     //�����̤�����
		Last_Neutral_Flag = Neutral_Flag;
	}
	else                   //�ǿյ�״̬
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
			New_Rev_Mode = 0;                          //ǰ��
		else
			New_Rev_Mode = 1;                          //����

		if(Rev_Mode != New_Rev_Mode)                 //������ϴοյ�ǰ��״̬��ͬ
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
* ������:Torque_Buffer_Function
* ��������:����ת������ֵ�������ʡ��ͷ��ʼ����ת��Ŀ��ֵ
*
* ����:    
* ��������:
*
* ����ֵ:
*
* �� ��:        
* �޸ļ�¼: 
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
* ������:Calc_TqeMd_Ref
* ��������:���㲻ͬ״̬�µ�ת������ֵ��Ŀ��ת��
*
* ����:    
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 5 �� 30 ��
* �޸ļ�¼: 
*
*===========================================================================
*/  

APP_SECTION void Calc_TqeMd_Ref(void)
{
  int16_t Temp_Tgt_RPM=0;
  //��������µ����ţ�ɲ������      
  if(BRAKEx > 0 || (BRK_SW == BRK_ON) || Neutral_Flag == 1 || TPSx_Output == 0)  //���ƶ���������ʱ
  {
		if(Motor1.Mot_AbsRPM > Min_BRK_RPM && Motor1.Cur_Mode == Rev_Mode && (ATD_Value_u8[Ctrl_1.PA.VAD_CH] > 3 || ATD_Value_u8[Ctrl_1.PB.VAD_CH] > 3))
		{
			if(BRAKEx > 0)
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * BRAKEx * Default_Cal_data[CAL_T_BRK_AD_PERCENT]) / 10000) * (-Ref_Direction));
			}
			else if(BRK_SW == BRK_ON && Default_Cal_data[CAL_T_BRK_SW_PERCENT] != 0) //ɲ���������ȼ����
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * Default_Cal_data[CAL_T_BRK_SW_PERCENT]) / 100) * (-Ref_Direction));
			}
			else if(Neutral_Flag == 1 && Default_Cal_data[CAL_T_NTL_BRK_PERCENT] != 0)  //�յ����ȼ���֮
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * Default_Cal_data[CAL_T_NTL_BRK_PERCENT]) / 100) * (-Ref_Direction));
			}
			else //��̤�����ȼ����
			{
				BRKTorque.Ref = (int16_t)((int16_t)((uint32_t)(OUTMAX_Q14 * (Default_Cal_data[CAL_T_RLSTPS_BRK_PERCENT])) / 100) * (-Ref_Direction));
				if(BRKTorque.Ref == 0)
				{
					if(Ref_Direction == 1)  //���ƶ�����ʱ��Ŀ���ٶ�����Ϊ��󣬷�ֹ�ٶȻ������Ƚϴ���ƶ�����
						Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Fwd_RPM) * Ref_Direction);
					else
						Temp_Tgt_RPM = (int16_t)((int16_t)(Max_Rev_RPM) * Ref_Direction);
				}
			}

			if(BRKTorque.Ref != 0)
				Regen_En = 1;
		}
		else  //�ٶ�̫��ʱ���ͷ��ƶ����أ�Ŀ���ٶ�Ϊ0������ֹͣ
		{
			BRKTorque.Ref = 0;
			Temp_Tgt_RPM = 0;
			Regen_En = 0;
		}

		TPSTorque.Ref = 0; 
    //������з�����ƶ����ط����෴��Ŀ���ٶ���Ϊ0�����ٷ���
    if((Rev_Mode == 0 && Real_OutMax <= 0) || (Rev_Mode == 1 && Real_OutMax >= 0))
    	Temp_Tgt_RPM = 0;
    
    if(Rev_Mode == 0) //��תIq:(-Imax/2)~Imax
    {
    	PIParmQ1.qOutMax = Target_Vq_max;
    	PIParmQ1.qOutMin = 0;
    }
    else    //��תIq:-Imax~(Imax/2)
    {
    	PIParmQ1.qOutMax = 0;
    	PIParmQ1.qOutMin = Target_Vq_min;
    } 
  }
  else 
  {
    BRKTorque.Ref = 0;
    Regen_En = 0;
    if(TPSx_Output > 0)  //�������ƶ����������������������
    {
      TPSTorque.Ref = TPSx_Output * Ref_Direction;
      if(Ref_Direction == 1)//���ƶ�����ʱ��Ŀ���ٶ�����Ϊ��󣬷�ֹ�ٶȻ������Ƚϴ���ƶ�����
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
* ������:Calc_TqeMd_Tgt
* ��������:����Ŀ��ת��
*
* ����:    
* ��������:
*
* ����ֵ:
*
* �� ��:        Icy, 2013 �� 5 �� 29 ��
* �޸ļ�¼: 
*
*===========================================================================
*/
APP_SECTION void Calc_TqeMd_Tgt(void)
{  
  uint16_t T_Acc_Rate_temp=0,T_BRK_Rate_temp=0; 
  int16_t Target_Out=0;
  //��������ֵ��ɲ��״̬����ת�ظ���ֵ
  //����ת�ظ���ֵ�����ת�ص�������ֵ

  if(M_En == 1 || Obser1.Start_Allow == 1)  //������������ʱ���������״̬ʱ����������ת��
  {
		T_Acc_Rate_temp = T_Acc_Rate;
		T_BRK_Rate_temp = T_BRK_Rate; 
  }
  else  //��������������ʱ����������ת�أ���ֹ�ν�ʱ���̫��
  {
    T_Acc_Rate_temp = 0;
    T_BRK_Rate_temp = 0;
  }
    
  TPSTorque.Tgt = Torque_Buffer_Function(&TPSTorque,T_Acc_Rate_temp,T_Acc_Rel_Rate);
  BRKTorque.Tgt = Torque_Buffer_Function(&BRKTorque,T_BRK_Rate_temp,T_BRK_Rel_Rate);
  
  Target_Out = (int16_t)(TPSTorque.Tgt + BRKTorque.Tgt);
  
  if(Target_Out > (int16_t)OUTMAX_Q14) Target_Out = (int16_t)OUTMAX_Q14;
  if(Target_Out < (int16_t)(-(int16_t)OUTMAX_Q14)) Target_Out = (int16_t)(-(int16_t)OUTMAX_Q14);

  if(Rev_Mode == 0) //��תIq:-Regen_Iq_Max~Drive_Iq_Max
  {
    if(Target_Out > (int16_t)Drive_OutMax)
    	Target_Out = (int16_t)Drive_OutMax;
    else if(Target_Out < (int16_t)(-Regen_OutMax))
    	Target_Out = (int16_t)(-Regen_OutMax);
  }
  else    //��תIq:-Drive_Iq_Max~Regen_Iq_Max
  {
    if(Target_Out > (int16_t)Regen_OutMax)
    	Target_Out = (int16_t)Regen_OutMax;
    else if(Target_Out < (int16_t)(-Drive_OutMax))
    	Target_Out = (int16_t)(-Drive_OutMax);
  }
  Real_OutMax = (int16_t)((int32_t)(Target_Out * Current_Max_Q) >> 14);

  //�򿪻�ر����ʹ��
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
