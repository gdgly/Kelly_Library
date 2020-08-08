/*
 * Err_Sts.c
 *
 *  Created on: 2018年11月7日
 *      Author: Administrator
 */
#include "Function.h"
#include "Variable.h"
#include "Platform.h"




//############################################################################
//函数功能：LED灯提示控制器当前状态
//函数名：LED_Process()
//
//描述：控制器正常时绿灯常亮
//      报错时根据报错码红灯相应闪烁
//
//作者：
//时间：
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

	  if(Error_Struct.str!=0 || LED_Status != LED_START)//have error 绿灯灭、红灯报错误代码
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
	  else //no error  红灯灭、绿灯亮
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


	  if(Error_Struct.str!=0 || LED_Status != LED_START)//have error 绿灯灭、红灯报错误代码
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
	  else //no error  红灯灭、绿灯亮
	  {
	    RED_CTL=LIGHT_OFF;
	    GREEN_CTL=LIGHT_ON;
	  }

	  ALM_OUT = RED_CTL;

	}
#endif
}*/

//############################################################################
//函数功能：蜂鸣器鸣叫指出当前控制器错误状态
//函数名：Alarm_Process()
//
//描述：控制器正常时蜂鸣器处于睡眠状态
//      报错时根据报错码蜂鸣器开始工作
//
//作者：Dwane
//时间：2012-03-06
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
			ALM_OUT = 1;     //高踏板错误,蜂鸣器长鸣
		else if( Alarm_Err.str || (Alarm_Status!=ALARM_START) )
		{
			if(Short_Sound == 0)      //开始检测新错误
			{
				for(i=Error_Number ; i<16; Error_Number++,i++)  //蜂鸣器结束指示一个错误后,继续往后检测是否有新错误
				{
					if(((Alarm_Err.str>>i) & (uint16_t)0x0001) == 1)  //检测到错误源
					{
						Short_Sound = Error_Number; //蜂鸣器低端响声次数
						Error_Code = Error_Number;
						i=15;                                           //检测有错误,停止检测
					}

					if(Error_Number >= 15)  Error_Number = 0;       //超出所能检测错误范围后,再次重新检测
				}

				ALM_OUT = 0;              //关闭
				Alarm_Status = ALARM_START;

				Long_Sound = 3;   //高端响1.5s
				Shut_Sound = 6;   //出现错误后,控制器停顿3s后再工作
			}

			switch(Alarm_Status)
			{
				case ALARM_START:
				{
					if(Shut_Sound > 0)  //蜂鸣器维持3s的关闭
					{
						Shut_Sound--;
						ALM_OUT = 0;        //关闭
					}
					else  Alarm_Status = ALARM_HIGH_BLINKING;
				}break;

				case  ALARM_HIGH_BLINKING:
				{
					if(Long_Sound > 0)  //蜂鸣器响声维持1.5s
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

				case ALARM_MIDDLE:  //高端响声结束后，休息1.5秒
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
					if( (ALM_OUT == 0) && (Short_Sound > 0) )  //低端响声次数为 Short_Sound
					{
						ALM_OUT = 1;
						Short_Sound--;
					}
					else if( (ALM_OUT == 1) && (Short_Sound > 0) )
					{
						ALM_OUT = 0;
					}
					else    //报错结束
					{
						ALM_OUT=0;  //关闭蜂鸣器
						Short_Sound=0;//继续下一次的错误检测
					}
				}break;

				default: break;
			}
		}
		else
		{
			ALM_OUT = 0;   //控制器正常则关闭蜂鸣器
			Error_Number = 0;
		}
	}
}*/
