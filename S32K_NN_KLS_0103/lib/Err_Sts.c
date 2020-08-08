/*
 * Err_Sts.c
 *
 *  Created on: 2018��11��7��
 *      Author: Administrator
 */
#include "Function.h"
#include "Variable.h"
#include "Platform.h"




//############################################################################
//�������ܣ�LED����ʾ��������ǰ״̬
//��������LED_Process()
//
//����������������ʱ�̵Ƴ���
//      ����ʱ���ݱ���������Ӧ��˸
//
//���ߣ�
//ʱ�䣺
//############################################################################
/*APP_SECTION void LED_Process(void)
{
  static uint8_t Shine_Timer,HighShine,LowShine,CurError,LED_Status,Shut_Timer;
  uint8_t i=0,j=0;

#if KELLY_CAN == 0
	Shine_Timer++;
	if(Shine_Timer>=5)//500ms task
	{
	  Shine_Timer=0;

	  if(Error_Struct.str!=0 || LED_Status != LED_START)//have error �̵��𡢺�Ʊ��������
	  {
	    GREEN_CTL=LIGHT_OFF;

	    j=CurError;

	    if(LowShine==0)//have indicated a complete error code,detect next error
  	  {
  		  for(i=0;i<16;i++) //first suppose it has 16 faults
  		  {
  		    j++;

  		    if(j>15)
  		      j=0;

  		    if(((Error_Struct.str>>j) & (uint16_t)0x0001) == 1)//detect where fault
  		    {
  		      if( CurError != j)
  		      {
  		        CurError = j;
    		      HighShine=Led_Blinking[CurError]>>4;//reserved high 4 bits
    		      LowShine=Led_Blinking[CurError]&0xf;//reserved low 4 bits
    		      RED_CTL=LIGHT_OFF;  //shut led_red
    		      LED_Status=LED_START;
    		      Shut_Timer=4;
    		      break;
  		      }
  		    }
  		  }
	    }

		  switch(LED_Status)
		  {
		    case LED_START:
		    {
		      if(Shut_Timer>0)
		      {
		        Shut_Timer--;//Shut_Timer minus oneself 4 times,used 4*500ms=2s
		        RED_CTL=LIGHT_OFF;//shut led_red
		        break;
		      }
		      else
		      {
		        LED_Status=LED_HIGH_BLINKING;
		      }
		    }

		    case LED_HIGH_BLINKING:
		    {
		      if(RED_CTL==LIGHT_OFF && HighShine>0)
		      {
		        RED_CTL=LIGHT_ON; //high shine
		        HighShine--;
		        break;
		      }
		      else if(HighShine>0)
		      {
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
		        Shut_Timer=2;//1s
		        LED_Status=LED_MIDDLE;
		      }
		    }

		    case LED_MIDDLE:
		    {
		      if(Shut_Timer>0)
		      {
		        Shut_Timer--;//Shut_Timer minus oneself 2 times,used 2*500ms=1s
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
		        LED_Status=LED_LOW_BLINKING;
		      }
		    }

		    case LED_LOW_BLINKING:
		    {
		      if(RED_CTL==LIGHT_OFF && LowShine>0)
		      {
		        RED_CTL=LIGHT_ON;
		        LowShine--;
		        break;
		      }
		      else if(LowShine>0)
		      {
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
  		      RED_CTL=LIGHT_OFF;  //shut led_red
  		      HighShine=Led_Blinking[CurError]>>4;//reserved high 4 bits
  		      LowShine=Led_Blinking[CurError]&0xf;//reserved low 4 bits
  		      LED_Status=LED_START;
  		      Shut_Timer=4;
		        break;
		      }
		    }
		    default:
		    {
		      break;
		    }
		  }
	  }
	  else //no error  ������̵���
	  {
	    RED_CTL=LIGHT_OFF;
	    GREEN_CTL=LIGHT_ON;
	  }

	}
#else
  Shine_Timer++;
	if(Shine_Timer>=5)//500ms task
	{
	  Shine_Timer=0;


	  if(Error_Struct.str!=0 || LED_Status != LED_START)//have error �̵��𡢺�Ʊ��������
	  {
	    GREEN_CTL=LIGHT_OFF;

	    j=CurError;

	    if(LowShine==0)//have indicated a complete error code,detect next error
  	  {
  		  for(i=0;i<16;i++) //first suppose it has 16 faults
  		  {
  		    j++;

  		    if(j>15)
  		      j=0;

  		    if(((Error_Struct.str>>j) & (uint16_t)0x0001) == 1)//detect where fault
  		    {
  		      if( CurError != j)
  		      {
  		        CurError = j;
    		      HighShine=Led_Blinking[CurError]>>4;//reserved high 4 bits
    		      LowShine=Led_Blinking[CurError]&0xf;//reserved low 4 bits
    		      RED_CTL=LIGHT_OFF;  //shut led_red
    		      LED_Status=LED_START;
    		      Shut_Timer=4;
    		      break;
  		      }
  		    }
  		  }
	    }

		  switch(LED_Status)
		  {
		    case LED_START:
		    {
		      if(Shut_Timer>0)
		      {
		        Shut_Timer--;//Shut_Timer minus oneself 4 times,used 4*500ms=2s
		        RED_CTL=LIGHT_OFF;//shut led_red
		        break;
		      }
		      else
		      {
		        LED_Status=LED_HIGH_BLINKING;
		      }
		    }

		    case LED_HIGH_BLINKING:
		    {
		      if(RED_CTL==LIGHT_OFF && HighShine>0)
		      {
		        RED_CTL=LIGHT_ON; //high shine
		        HighShine--;
		        break;
		      }
		      else if(HighShine>0)
		      {
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
		        Shut_Timer=2;//1s
		        LED_Status=LED_MIDDLE;
		      }
		    }

		    case LED_MIDDLE:
		    {
		      if(Shut_Timer>0)
		      {
		        Shut_Timer--;//Shut_Timer minus oneself 2 times,used 2*500ms=1s
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
		        LED_Status=LED_LOW_BLINKING;
		      }
		    }

		    case LED_LOW_BLINKING:
		    {
		      if(RED_CTL==LIGHT_OFF && LowShine>0)
		      {
		        RED_CTL=LIGHT_ON;
		        LowShine--;
		        break;
		      }
		      else if(LowShine>0)
		      {
		        RED_CTL=LIGHT_OFF;
		        break;
		      }
		      else
		      {
  		      RED_CTL=LIGHT_OFF;  //shut led_red
  		      HighShine=Led_Blinking[CurError]>>4;//reserved high 4 bits
  		      LowShine=Led_Blinking[CurError]&0xf;//reserved low 4 bits
  		      LED_Status=LED_START;
  		      Shut_Timer=4;
		        break;
		      }
		    }
		    default:
		    {
		      break;
		    }
		  }
	  }
	  else //no error  ������̵���
	  {
	    RED_CTL=LIGHT_OFF;
	    GREEN_CTL=LIGHT_ON;
	  }

	  ALM_OUT = RED_CTL;

	}
#endif
}*/

//############################################################################
//�������ܣ�����������ָ����ǰ����������״̬
//��������Alarm_Process()
//
//����������������ʱ����������˯��״̬
//      ����ʱ���ݱ������������ʼ����
//
//���ߣ�Dwane
//ʱ�䣺2012-03-06
//############################################################################
/*APP_SECTION void Alarm_Process(void)
{
  static uint8_t Long_Sound=0,Short_Sound=0,Shut_Sound=0,Alarm_Timer=0,Error_Number=0;
  static uint8_t Alarm_Status=ALARM_START;
  volatile uint8_t i=0;

  Alarm_Timer++;

  if(Alarm_Timer == 2)
  {
  	Alarm_Startup_TPS_Err  = Startup_TPS_Err;
		Alarm_ILOP_Error       = ILOP_Error;
		Alarm_OverTemp         = OverTemp;
		Alarm_Cur_Sensor_Err   = Cur_Sensor_Err;
		Alarm_Encoder_Err 		 = Pos_Sensor_Err;
		Alarm_LowVolt_Err      = LowVolt_Err;
		Alarm_OverVolt_Err     = OverVolt_Err;
		Alarm_Motor_Temp_Err   = Motor_Temp_Err;
		Alarm_TPS_Error        = TPS_Error;
  }
  else if(Alarm_Timer>=5)//500ms task
	{
		Alarm_Timer=0;

		if(Startup_TPS_Err)
			ALM_OUT = 1;     //��̤�����,����������
		else if( Alarm_Err.str || (Alarm_Status!=ALARM_START) )
		{
			if(Short_Sound == 0)      //��ʼ����´���
			{
				for(i=Error_Number ; i<16; Error_Number++,i++)  //����������ָʾһ�������,�����������Ƿ����´���
				{
					if(((Alarm_Err.str>>i) & (uint16_t)0x0001) == 1)  //��⵽����Դ
					{
						Short_Sound = Error_Number; //�������Ͷ���������
						Error_Code = Error_Number;
						i=15;                                           //����д���,ֹͣ���
					}

					if(Error_Number >= 15)  Error_Number = 0;       //�������ܼ�����Χ��,�ٴ����¼��
				}

				ALM_OUT = 0;              //�ر�
				Alarm_Status = ALARM_START;

				Long_Sound = 3;   //�߶���1.5s
				Shut_Sound = 6;   //���ִ����,������ͣ��3s���ٹ���
			}

			switch(Alarm_Status)
			{
				case ALARM_START:
				{
					if(Shut_Sound > 0)  //������ά��3s�Ĺر�
					{
						Shut_Sound--;
						ALM_OUT = 0;        //�ر�
					}
					else  Alarm_Status = ALARM_HIGH_BLINKING;
				}break;

				case  ALARM_HIGH_BLINKING:
				{
					if(Long_Sound > 0)  //����������ά��1.5s
					{
						ALM_OUT = 1;
						Long_Sound--;
					}
					else
					{
						ALM_OUT = 0;
						Shut_Sound = 3;
						Alarm_Status = ALARM_MIDDLE;
					}
				}break;

				case ALARM_MIDDLE:  //�߶�������������Ϣ1.5��
				{
					if(Shut_Sound > 0)
					{
						Shut_Sound--;
						ALM_OUT = 0;
					}
					else  Alarm_Status = ALARM_LOW_BLINKING;
				}break;

				case ALARM_LOW_BLINKING:
				{
					if( (ALM_OUT == 0) && (Short_Sound > 0) )  //�Ͷ���������Ϊ Short_Sound
					{
						ALM_OUT = 1;
						Short_Sound--;
					}
					else if( (ALM_OUT == 1) && (Short_Sound > 0) )
					{
						ALM_OUT = 0;
					}
					else    //�������
					{
						ALM_OUT=0;  //�رշ�����
						Short_Sound=0;//������һ�εĴ�����
					}
				}break;

				default: break;
			}
		}
		else
		{
			ALM_OUT = 0;   //������������رշ�����
			Error_Number = 0;
		}
	}
}*/
