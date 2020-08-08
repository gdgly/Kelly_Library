/*
 * Platform.c
 *
 *  Created on: 2018��8��1��
 *      Author: Administrator
 */


#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "s32_core_cm4.h"
#include "typedefine.h"
#include "Function.h"
#include "Platform.h"
#include "Variable.h"
#include "Motor.h"

/*==========================================================================
* ������:Memory_Init
* ��������:�ڴ��ʼ��
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Ted, 2014 �� 7 �� 28 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Memory_Init(void)
{
	uint32_t i=0;

	for(i=0;i<(RAM_SIZE/4)-1;i++)
	{
			(*(uint32_t *)(RAM_START_ADDRESS+ i*4)) = 0;
	}
	for(i=0;i<CAL_RAM_SIZE;i++)
	{
		Cal_data[i] = (*(uint32_t *)(CAL_DATA_FLASH_ADDRESS+ i));
	}

  if(Default_Cal_data[CAL_PWM_FREQ] == 10)
	{
		PWM1.dloop= 10000; //�Ƕȼ���ִ������100us, 1s/100us=10000
		PWM1.Mod  = 4000;
		PWM1.PE70 = 3540;
		PWM1.Max  = 3920; //98%
		PWM1.Min  = 80;  //2%
		Timer_500us=5;
	}
  else
	{
		PWM1.dloop= 6250; //�Ƕȼ���ִ������160us, 1s/160us=6250
		PWM1.Mod  = 6400;
		PWM1.PE70 = 5664;
		PWM1.Max  = 6272; //98%
		PWM1.Min  = 128;  //2%
		Timer_500us=3;
	}

	if((Default_Cal_data[CAL_APPLICATION_CONFIG+2]&0x40)!=0 && (Default_Cal_data[INFO_IDENTIFY] != 0xAA))//��1�ֽڵĵ�7λ
	{
		Ctrl_1.PA.EN_CH=EN1_CHB;
		Ctrl_1.PB.EN_CH=EN1_CHA;
		Ctrl_1.PC.EN_CH=EN1_CHC;

		Ctrl_1.PA.PWM_CH=PWM1_B;
		Ctrl_1.PB.PWM_CH=PWM1_A;
		Ctrl_1.PC.PWM_CH=PWM1_C;

		Ctrl_1.PA.IAD_CH =ADC_IB1_AD;
		Ctrl_1.PB.IAD_CH =ADC_IA1_AD;
		Ctrl_1.PC.VAD_CH =ADC_IC1_AD;

		Ctrl_1.PA.VAD_CH =ADC_VB1_AD;
		Ctrl_1.PB.VAD_CH =ADC_VA1_AD;
		Ctrl_1.PC.VAD_CH =ADC_VC1_AD;

		Ctrl_1.PA.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
		Ctrl_1.PB.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
		Ctrl_1.PC.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_L]);

	}
	else
	{
		Ctrl_1.PA.EN_CH=EN1_CHA;
		Ctrl_1.PB.EN_CH=EN1_CHB;
		Ctrl_1.PC.EN_CH=EN1_CHC;

		Ctrl_1.PA.PWM_CH=PWM1_A;
		Ctrl_1.PB.PWM_CH=PWM1_B;
		Ctrl_1.PC.PWM_CH=PWM1_C;

		Ctrl_1.PA.IAD_CH =ADC_IA1_AD;
		Ctrl_1.PB.IAD_CH =ADC_IB1_AD;
		Ctrl_1.PC.IAD_CH =ADC_IC1_AD;

		Ctrl_1.PA.VAD_CH =ADC_VA1_AD;
		Ctrl_1.PB.VAD_CH =ADC_VB1_AD;
		Ctrl_1.PC.VAD_CH =ADC_VC1_AD;

		Ctrl_1.PA.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
		Ctrl_1.PB.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
		Ctrl_1.PC.IAD_ZERO = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_C1_ZERO_CURRENT_L]);
	}
}

/*==========================================================================
* ������:MCU_Init
* ��������:MCU��ʼ��
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Ted, 2018 �� 7 �� 28 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void MCU_Init(void)
{
	Init_IRQs(FTM0_Ovf_Reload_IRQn,1);
	Init_IRQs(FTM2_Ovf_Reload_IRQn,2);
	Init_IRQs(FTM2_Ch2_Ch3_IRQn,3);
	Init_IRQs(FTM2_Ch4_Ch5_IRQn,3);

	Init_IO();
	LPUART0_init();
	LPUART1_init();
	Init_PWM(FTM0,PCC_FTM0_INDEX,Default_Cal_data[CAL_PWM_FREQ]);
	Init_ADC_SwTrig();
	Init_ADC1_HwTrig();
	Init_PDB();
	Start_PWM (FTM0);//ѡ��FTMϵͳʱ�ӣ�80MHz����Ϊʱ��Դ������FTM3

	Init_WDOG();
	WDOG_Feed();

	//j = SCI1_RecvChar(&i);//����һ�Σ���ֹ�����������õ��²�����
	//j = SCI1_SendChar(0);

	//CAN1_Init();
}

/*==========================================================================
* ������:IO_Init
* ��������:�˿ڳ�ʼ��
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Ted, 2014 �� 7 �� 28 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Init_IO(void)
{
	PCC->PCCn[PCC_PORTA_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTA */
	PCC->PCCn[PCC_PORTB_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PCC->PCCn[PCC_PORTE_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */

	//SDCBA
	PORTE->PCR[11]=PORT_PCR_MUX(1);
	PTE_PDDR_P11 = 0;
	PORTE->PCR[10]=PORT_PCR_MUX(4);
	PTE_PDDR_P10 = 0;
	PORTE->PCR[5]=PORT_PCR_MUX(4);
	PTE_PDDR_P5 = 0;
	PORTE->PCR[4]=PORT_PCR_MUX(4);
	PTE_PDDR_P4 = 0;

	//PWMCBA
	PORTE->PCR[9]=PORT_PCR_MUX(2);
	PTE_PDDR_P9 = 1;
	PORTE->PCR[8]=PORT_PCR_MUX(2);
	PTE_PDDR_P8 = 1;
	PORTB->PCR[5]=PORT_PCR_MUX(2);
	PTB_PDDR_P5 = 1;

	//EN-CBA
	PORTE->PCR[2]=PORT_PCR_MUX(1);
	PTE_PDDR_P2 = 1;
	PORTE->PCR[1]=PORT_PCR_MUX(1);
	PTE_PDDR_P1 = 1;
	PORTE->PCR[0]=PORT_PCR_MUX(1);
	PTE_PDDR_P0 = 1;

	//UART0 TXRX
	PORTC->PCR[3]=PORT_PCR_MUX(4);
	PTC_PDDR_P3 = 1;
	PORTC->PCR[2]=PORT_PCR_MUX(4);
	PTC_PDDR_P2 = 0;

	//UART1 TXRX
	PORTC->PCR[9]=PORT_PCR_MUX(2);
	PTC_PDDR_P9 = 1;
	PORTC->PCR[8]=PORT_PCR_MUX(2);
	PTC_PDDR_P8 = 0;

	//CAN1 TXRX
	PORTA->PCR[13]=PORT_PCR_MUX(3);
	PTA_PDDR_P13 = 1;
	PORTA->PCR[12]=PORT_PCR_MUX(3);
	PTA_PDDR_P12 = 0;

	PORTD->PCR[7]=PORT_PCR_MUX(1);//ECO_IN
	PTD_PDDR_P7 = 0;
	PORTD->PCR[6]=PORT_PCR_MUX(1);//MTR_OUT
	PTD_PDDR_P6 = 1;
	PORTD->PCR[5]=PORT_PCR_MUX(1);//COIL_OUT
	PTD_PDDR_P5 = 1;
	PORTC->PCR[1]=PORT_PCR_MUX(1);//ALM_OUT
	PTC_PDDR_P1 = 1;
	PORTC->PCR[0]=PORT_PCR_MUX(1);//FWD_IN
	PTC_PDDR_P0 = 0;
	PORTC->PCR[17]=PORT_PCR_MUX(1);//FOOT_IN
	PTC_PDDR_P17 = 0;
	PORTC->PCR[15]=PORT_PCR_MUX(1);//REV_IN
	PTC_PDDR_P15 = 0;
	PORTC->PCR[14]=PORT_PCR_MUX(1);//BRK_IN
	PTC_PDDR_P14 = 0;

	COIL_OUT=TRUE;                //���Ӵ�������
}
/*==========================================================================
* ������:Function_Init
* ��������:���궨�������ÿ���������
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
APP_SECTION void Function_Init(void)
{
  if((Default_Cal_data[CAL_APPLICATION_CONFIG] & 0x02) != 0)//��0�ֽڵĵ�01λ
  {
    Releas_BRK_DisHP = 1;
  }
  if((Default_Cal_data[CAL_APPLICATION_CONFIG] & 0x04) != 0)//��0�ֽڵĵ�01λ
  {
    Neutral_DisHP = 1;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1]&0x01)!=0)//��1�ֽڵĵ�0λ//usage Foot SW
  {
    WithFoot=1;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1]&0x02)==0)//��1�ֽڵĵ�1λ//�����߼��궨
  {
  //�͵�ƽ��Ч
    SW_ON  = 0;
    SW_OFF = 1;
  }
  else  //�ߵ�ƽ��Ч
  {
    SW_ON  = 1;
    SW_OFF = 0;
  }

  if((Default_Cal_data[CAL_APPLICATION_CONFIG+1] & 0x04) != 0)//��1�ֽڵĵ�2λ//�궨ģʽѡ��0-��������ģʽ��MTPA����ֵ��1-�궨����ģʽ�����ڱ궨�±��ǲ�ֵ�㷨
  {
  	PCB_En = 1;
  }

  return;
}

/*==========================================================================
* ������:Reset_Handle
* ��������:��λ����봦��
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Ted, 2014 �� 7 �� 28 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void Reset_Handle(void)
{
  uint8_t Reset_Status=0;

	if((RCM->SRS & 0x82) !=0 )//If Power on reset
  {
    RESET_COUNT=0;
    if((Default_Cal_data[CAL_APPLICATION_CONFIG]&0x01)==1)
    {
      StartupStatus=STARTUP_POWERON; //High Pedal Disable
    }
    else
    {
      StartupStatus=STARTUP_NORMALLY;//High Pedal Enable
    }
    Reset_Status = 0x33;
  }
  else if(((RCM->SRS & 0x2EE4)!=0))//other cause reset
  {
    if(RESET_COUNT<200)
      RESET_COUNT++;
    ILOP_Error = 1;
    Reset_Status = 0x99;
  }

	if(RESET_COUNT>=10)
	{
	  Reset_Err=1;
	  ILOP_Error = 1;
  }

	if(Reset_Status == 0x99)//���ϵ縴λ
	{
		PCB_Temp = PCB_TEMP_SV;
	}
	else
	{
		PCB_Temp = Get_Temp((uint8_t)(ATD_Value_u16[ADC_TEMP_AD] >> ADx_AD8_Coeff));
		PCB_TEMP_SV = PCB_Temp;
	}
}

/*==========================================================================
* ������:POR_Status_Detect
* ��������:�ϵ����ٶȣ����غ͵�ѹ״̬
*
* ����:
* ��������:
*
* ����ֵ:
*
* �� ��:        Ted, 2014 �� 7 �� 28 ��
* �޸ļ�¼:
*
*===========================================================================
*/
APP_SECTION void POR_Status_Detect(void)
{
  /*int16_t Cal_I_Zero=0,ZeroErr=0;

	Cal_I_Zero = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_A1_ZERO_CURRENT_L]);
	ZeroErr = (int16_t)(Cal_I_Zero - ATD_Value_u16[ADC_IA1_AD]);
	if(ZeroErr > 20 || ZeroErr < -20)
		Cur_Sensor_Err1 = 1;

	Cal_I_Zero = (uint16_t)(((uint16_t)(Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_H])<<8)+Default_Cal_data[INFO_PHASE_B1_ZERO_CURRENT_L]);
	ZeroErr = (int16_t)(Cal_I_Zero - ATD_Value_u16[ADC_IB1_AD]);
	if(ZeroErr > 20 || ZeroErr < -20)
		Cur_Sensor_Err1 = 1;
*/
	//��ⷴ�򿪹�״̬
	if(FWD_SW == SW_OFF && REV_SW == SW_ON)
		Rev_Mode = 1;                          //����
	else
		Rev_Mode = 0;                          //ǰ��

	if(FWD_SW != REV_SW)
		NTL_Error = 1;

  Calculate_Speed(Hall_Str1.CurSpeed,&Motor1,&Obser1);    //����ת��ֵ

	//��AD����ʵ�ʵ�ѹֵ
	B_Voltage_64 = Get_Voltage(ATD_Value_u16[ADC_B_AD]);
	B_Voltage = B_Voltage_64 >> 6;

	//�ϵ����������ص�ѹ������100ms�������м��
	if((ATD_Value_u16[ADC_B_AD] >> ADx_AD8_Coeff) < LowVoltageAD)
	{
		Global_En = 0;LowVolt_Err = 1;Wait_Timer = 1;
	}
	else if((ATD_Value_u16[ADC_B_AD] >> ADx_AD8_Coeff) > OverVoltageAD)
	{
		Global_En = 0;B_OverVolt_Err = 1;OverVolt_Err = 1;
	}
}

//�ӳ�100us�������̣���������1+1+1+133*10*3+1+2+4(nop)=4000;ÿ�����ڵ�ʱ�䣺1/40M=0.025us��100us=4000*0.025us
APP_SECTION void Cpu_Delay100US(void)
{
	uint32_t j=0;

	//for(i=0;i<us;i++)
		for(j=0;j<1000;j++)
			{}

}

//############################################################################
//�������ܣ�PWM���
//��������PWM_Output(uint8_t Channel,uint16_t Duty)
//
//������
//
//���ߣ�Ted
//ʱ�䣺2013-10-11
//############################################################################
APP_SECTION void PWM_Output(uint32_t Channel,uint32_t Duty)
{
	SET_32(Channel, Duty);
  return;
}

//############################################################################
//�������ܣ��رտ��������
//��������CloseAll()
//
//������
//
//���ߣ�Dwane
//ʱ�䣺2013-02-23
//############################################################################
APP_SECTION void CloseAll(void)
{
	M_En = 0;
	Global_En = 0;
	PWM_PORT = PWM1_DI;
  return;
}
