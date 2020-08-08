/*
 * ETS.c
 *
 *  Created on: Jul 26, 2014
 *      Author: Admin
 */
/* Including needed modules to compile this module/procedure */

#include "Flash.h"
/* Including shared modules, which are used for whole project */
#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"

#include "Variable.h"
#include "Function.h"
#include "Platform.h"
#include "ETS.h"
     

 /* Global variable definition here */

volatile T_ETS_Buf ETS_RX_Buf;
volatile T_ETS_Buf ETS_TX_Buf;

volatile T_ETS_Tx_State	ETS_Tx_Status ;
volatile T_ETS_Rx_State	ETS_Rx_Status ;


APP_SECTION void Call_ETS(void)
{                          
	uint8_t Return_code2,Recv_Value2;
	static volatile uint16_t Check_ETS=0;
	
	Check_ETS++;
	
	if(Check_ETS>10000)//ETS run Error
	{
	    ETS_RX_Buf.index=0;
			ETS_Rx_Status = ETS_RECEIVED;
	}
	
	Return_code2=LPUART1_receive_char(&Recv_Value2);
 
  if(Return_code2==ERR_OK)
	{
	  Check_ETS=0;//ETS run OK
	  
		if (ETS_Rx_Status == ETS_RECEIVED)
		{
			ETS_RX_Buf.index=0;
			ETS_Rx_Status = ETS_RECEIVING;
		}
		
		if(ETS_RX_Buf.index<ETS_MAX_DATA_LEN+3)
			ETS_RX_RAW[ETS_RX_Buf.index++]=Recv_Value2;
		
		if (ETS_RX_BYTES > ETS_MAX_DATA_LEN) ETS_RX_BYTES = ETS_MAX_DATA_LEN;   	
		
		if(ETS_RX_Buf.index >= ETS_RX_BYTES+3) //all bytes received
		{
			ETS_Rx_Status = ETS_RECEIVED;
			ETS_Action();
		}	    
	} 
	
	if (ETS_Tx_Status != ETS_TRANSMITTED)
	{
		Return_code2=LPUART1_transmit_char(ETS_TX_RAW[ETS_TX_Buf.index]);
		
	  if(Return_code2==ERR_OK)
		{
			if(ETS_TX_Buf.index++ >= (uint8_t)(ETS_TX_BYTES+2))
				ETS_Tx_Status = ETS_TRANSMITTED;
		}
	}
}


APP_SECTION void ETS_Action(void)
{ 
	static uint8_t Seed=0;
	uint8_t command=0, i=0,check_sum=0, tx_bytes=0;
	uint16_t par=0;
	//check checksum of received data
	
	check_sum = 0;
	for (i=0; i<(uint8_t)(ETS_RX_BYTES+2); i++)
		check_sum += ETS_RX_RAW[i];
	
	if(check_sum != ETS_RX_DATA[ETS_RX_BYTES])
	{
		ETS_TX_CMD = ETS_CHECKSUM_ERROR;
		goto prepare_to_transmit;
	}
	
	ETS_TX_CMD = command = ETS_RX_CMD;
	
	switch (command)
	{
		
		case  ETS_CODE_VERSION: //判断指令，软件版本指令
					{
						tx_bytes = 3;
						ETS_TX_DATA[0] = SOFT_VER_H;
						ETS_TX_DATA[1] = SOFT_VER_L;
						ETS_TX_DATA[2] = 1;  //01 代表软件处在应用程序
					}break;
			

		case  ETS_A2D_BATCH_READ:  
		  		{
		  			tx_bytes = 16;
		  			
		  			ETS_TX_DATA[0]=ATD_Value_u8[ADC_TPS1_AD];
	      		ETS_TX_DATA[1]=0;
	      		ETS_TX_DATA[2]=ATD_Value_u8[ADC_BRK1_AD];
	      		ETS_TX_DATA[3]=ATD_Value_u8[ADC_B_AD];//ATD_Value_u8[ADC_BRK2_AD];
	      		ETS_TX_DATA[4]=ATD_Value_u8[ADC_VS_AD];
	          ETS_TX_DATA[5]=ATD_Value_u8[ADC_V_AD];
	          ETS_TX_DATA[6]=(uint8_t)(B_Voltage>> 8);
	          ETS_TX_DATA[7]=(uint8_t)B_Voltage;
	      		ETS_TX_DATA[8]=ATD_Value_u8[ADC_VA1_AD];
	      		ETS_TX_DATA[9]=ATD_Value_u8[ADC_VB1_AD];
	      		ETS_TX_DATA[10]=ATD_Value_u8[ADC_VC1_AD];
	      		ETS_TX_DATA[11]=0;
	      		ETS_TX_DATA[12]=0;
	      		ETS_TX_DATA[13]=0;
	      		ETS_TX_DATA[14]=ATD_Value_u8[ADC_IA1_AD];
	      		ETS_TX_DATA[15]=ATD_Value_u8[ADC_IB1_AD];
		  		}break;

		case  ETS_A2D_BATCH_READ2:
		  		{
		  			tx_bytes = 16;

		  			ETS_TX_DATA[0]=0;
	      		ETS_TX_DATA[1]=0;
	      		ETS_TX_DATA[2]=Motor_Temp1;
	      		ETS_TX_DATA[3]=0;
	      		ETS_TX_DATA[4]=HL_Temp;
	          ETS_TX_DATA[5]=0;
	          ETS_TX_DATA[6]=0;
	          ETS_TX_DATA[7]=0;
	      		ETS_TX_DATA[8]=PCB_Temp;
	      		ETS_TX_DATA[9]=0;
	      		ETS_TX_DATA[10]=(uint8_t)((FOOT_SW)<<1) + BRK_SW;
	      		ETS_TX_DATA[11]=(uint8_t)((FWD_SW)<<1) + REV_SW;
	      		ETS_TX_DATA[12]=(uint8_t)((ECO_IN)<<1) + 0;
	      		ETS_TX_DATA[13]=(uint8_t)((Rev_Mode)<<1) + Motor1.Cur_Mode;
	      		ETS_TX_DATA[14]=0;
	      		ETS_TX_DATA[15]=HALL_PORT1;

		  		}break;
			          		
		case  ETS_FLASH_OPEN:
		  		{
		  			WDOG_Feed();
		  			OpenFlash();
		  		}break;	  

	  case ETS_FLASH_CLOSE:
      
      CloseAll();
	  	Cpu_Delay100US();
	  	_DI();
      CloseFlash();
      _EI();
  		
  		break;  
		
    case ETS_WATCHDOG_TEST:
      for(;;);//Not return value,will restart   	
      break;


		case  ETS_FLASH_READ:
				{
					par = (uint16_t)(ETS_RX_DATA[2] << 8) + ETS_RX_DATA[0];
					tx_bytes=ETS_RX_DATA[1];
					
					for(i=0;i<tx_bytes;i++)
					{
						ETS_TX_DATA[i]=Cal_data[par];
						par++;
					}  
				}break;
		
		case  ETS_FLASH_WRITE:
				{
					par = (uint16_t)(ETS_RX_DATA[2] << 8) + ETS_RX_DATA[0];
	
					tx_bytes=1;	    
					ETS_TX_DATA[0]=0;
					for(i=0;i<ETS_RX_DATA[1];i++)
					{
						Cal_data[par]=ETS_RX_DATA[(uint8_t)(i+3)];//Write to RAM
						par++;
					}    
				}break;	
          
    case ETS_GET_PHASE_I_AD://测项线默认I_AD

  		tx_bytes=12;

  		_DI();

  		PWM_PORT = PWM1_DI;

  		SET_32(Ctrl_1.PA.PWM_CH, 0);
  		SET_32(Ctrl_1.PB.PWM_CH, 0);
  		SET_32(Ctrl_1.PC.PWM_CH, 0);

    	PWM_PORT = PWM1_EN;

    	for(i=0;i<50;i++)//2ms
    	{
    	  Cpu_Delay100US();
    	  WDOG->CNT = 0xB480A602;
    	}
    	 // refresh watchdog

    	for(i=0;i<50;i++)//2ms
    	{
    	  Cpu_Delay100US();
    	  Get_ATD_Data(0);
    	  WDOG->CNT = 0xB480A602; // refresh watchdog
    	}

      ETS_TX_DATA[0]=ATD_Value_u16[ADC_IA1_AD] >> 8;//Save conversion value to temp variable
      ETS_TX_DATA[1]=ATD_Value_u16[ADC_IA1_AD];
      ETS_TX_DATA[2]=ATD_Value_u16[ADC_IB1_AD] >> 8;//Save conversion value to temp variable
      ETS_TX_DATA[3]=ATD_Value_u16[ADC_IB1_AD];
      ETS_TX_DATA[4]=ATD_Value_u16[ADC_IC1_AD] >> 8;//Save conversion value to temp variable
      ETS_TX_DATA[5]=ATD_Value_u16[ADC_IC1_AD];

      ETS_TX_DATA[6]=(uint8_t)(512 >> 8);//Save conversion value to temp variable
      ETS_TX_DATA[7]=(uint8_t)512;
      ETS_TX_DATA[8]=(uint8_t)(512 >> 8);//Save conversion value to temp variable
      ETS_TX_DATA[9]=(uint8_t)512;
      ETS_TX_DATA[10]=(uint8_t)(512 >> 8);//Save conversion value to temp variable
      ETS_TX_DATA[11]=(uint8_t)512;

      PWM_PORT = PWM1_DI;

      _EI();

  		break;

		case  ETS_MONITOR1:
					{
						tx_bytes=16;    

					  ETS_TX_DATA[0] = 0;
					  ETS_TX_DATA[1] = 0;
					  ETS_TX_DATA[2] = Error_Struct1.str;
					  ETS_TX_DATA[3] = Error_Struct0.str;
					  ETS_TX_DATA[4] = (uint8_t)((uint16_t)(Motor1.Tgt_RPM+10000)>>8);
					  ETS_TX_DATA[5] = (uint8_t)((uint16_t)(Motor1.Tgt_RPM+10000));

					  ETS_TX_DATA[6] = (uint8_t)((uint16_t)(Motor1.Mot_RPM+10000)>>8);
					  ETS_TX_DATA[7] = (uint8_t)((uint16_t)(Motor1.Mot_RPM+10000));
					  
			      ETS_TX_DATA[8] = (uint8_t)((uint16_t)(ParkParm_I1.IqRef+15000)>>8);
					  ETS_TX_DATA[9] = (uint8_t)((uint16_t)(ParkParm_I1.IqRef+15000));
					  ETS_TX_DATA[10] = (uint8_t)((uint16_t)(ParkParm_I1.Iq+15000)>>8);
					  ETS_TX_DATA[11] = (uint8_t)((uint16_t)(ParkParm_I1.Iq+15000));
					  
					  ETS_TX_DATA[12] = (uint8_t)((uint16_t)(ParkParm_I1.IdRef+15000)>>8);
					  ETS_TX_DATA[13] = (uint8_t)((uint16_t)(ParkParm_I1.IdRef+15000));

					  ETS_TX_DATA[14] = (uint8_t)((uint16_t)(ParkParm_I1.Id+15000)>>8);
					  ETS_TX_DATA[15] = (uint8_t)((uint16_t)(ParkParm_I1.Id+15000));
	
		  		}break;			              

		case  ETS_MONITOR2:
					{
						tx_bytes=16; 
						   
					  ETS_TX_DATA[0]= (uint8_t)((int16_t)(ParkParm_V1.Vd>>8) + 128);
					  ETS_TX_DATA[1]= (uint8_t)((int16_t)(ParkParm_V1.Vq>>8) + 128);
					    
					  ETS_TX_DATA[2] = 0;
					  ETS_TX_DATA[3] = 0;
					  
					  ETS_TX_DATA[4] = 0;
					  ETS_TX_DATA[5] = 0;
					  
			      ETS_TX_DATA[6] = 0;
					  ETS_TX_DATA[7] = 0;
					  ETS_TX_DATA[8] = 0;
					  ETS_TX_DATA[9] = 0;
					  
					  ETS_TX_DATA[10] = 0;
					  ETS_TX_DATA[11] = 0;
					                                                                 
					  ETS_TX_DATA[12] = 0;
					  ETS_TX_DATA[13] = 0;
						
					  ETS_TX_DATA[14]= 0;
					  ETS_TX_DATA[15]= 0;
		  		}break;		

		case  ETS_ETS_MONITOR1:
					{
						tx_bytes=16; 
						   
		  			ETS_TX_DATA[0]=ATD_Value_u8[ADC_TPS1_AD];
	      		ETS_TX_DATA[1]=0;
	      		ETS_TX_DATA[2]=ATD_Value_u8[ADC_BRK1_AD];
	      		ETS_TX_DATA[3]=0;
	      		ETS_TX_DATA[4]=ATD_Value_u8[ADC_VS_AD];
	          ETS_TX_DATA[5]=ATD_Value_u8[ADC_V_AD];
	          ETS_TX_DATA[6]=0;
	          ETS_TX_DATA[7]=ATD_Value_u8[ADC_B_AD];
	          ETS_TX_DATA[8]=ATD_Value_u8[ADC_MOT1_AD];
	      		ETS_TX_DATA[9]=0;
	      		ETS_TX_DATA[10]=ATD_Value_u8[ADC_HS_TEMP_AD];
	          ETS_TX_DATA[11]=0;
	          ETS_TX_DATA[12]=0;
	          ETS_TX_DATA[13]=0;
	      		ETS_TX_DATA[14]=ATD_Value_u8[ADC_TEMP_AD];
	      		ETS_TX_DATA[15]=0;
		  		}break;	
		  		
		case  ETS_ETS_MONITOR2:
					{
						tx_bytes=16; 
						   
			      ETS_TX_DATA[0] = HALL_SA1;
					  ETS_TX_DATA[1] = HALL_SB1;
					  ETS_TX_DATA[2] = HALL_SC1;
					  ETS_TX_DATA[3] = 0;
					  ETS_TX_DATA[4]= 0;
					  ETS_TX_DATA[5]= 0;
					  ETS_TX_DATA[6] = FOOT_SW;
					  ETS_TX_DATA[7] = BRK_SW;
					  ETS_TX_DATA[8] = FWD_SW;
					  ETS_TX_DATA[9] = REV_SW;
			      ETS_TX_DATA[10] = ECO_IN;
					  ETS_TX_DATA[11] = 0;
					  ETS_TX_DATA[12] = Rev_Mode;
					  ETS_TX_DATA[13] = Motor1.Cur_Mode;
					  ETS_TX_DATA[14] = 0;
					  ETS_TX_DATA[15] = 0;
		  		}break;		
		  		
		case  ETS_ETS_MONITOR3:
					{
						tx_bytes=16; 
						   
	          ETS_TX_DATA[0]=(uint8_t)(B_Voltage>> 8);
	          ETS_TX_DATA[1]=(uint8_t)B_Voltage;
					  ETS_TX_DATA[2] = (uint8_t)((uint16_t)(Motor1.Mot_RPM+10000)>>8);
					  ETS_TX_DATA[3] = (uint8_t)((uint16_t)(Motor1.Mot_RPM+10000));
					  ETS_TX_DATA[4] = 0;
					  ETS_TX_DATA[5] = 0;
					  ETS_TX_DATA[6] = 0;
					  ETS_TX_DATA[7] = 0;
					  ETS_TX_DATA[8] = 0;
					  ETS_TX_DATA[9] = 0;
			      ETS_TX_DATA[10] = 0;
					  ETS_TX_DATA[11] = 0;
					  ETS_TX_DATA[12] = 0;
					  ETS_TX_DATA[13] = 0;
						ETS_TX_DATA[14] = 0;
						ETS_TX_DATA[15] = 0;
		  		}break;
		  		
	  case ETS_GET_SEED:
  		tx_bytes=2;  		
  		Seed=FTM0->CNT >> 8;
  		Seed=FTM0->CNT;
  		ETS_TX_DATA[0]=Seed;	
  		break;   	   	
  			             
   case ETS_VALIDATE_SEED: 
  		tx_bytes=1;   
      ETS_TX_DATA[0]=0;//indicate error

      
      if(Seed!=0)
      {
      	Seed ^= 0xEB;
        
        if(ETS_RX_DATA[0]==Seed)
        {
          #if DEBUG
          if(ETS_RX_DATA[1]==0x1)//into Test Mode
          {
            TestMode=1;
            ETS_TX_DATA[0]=1;//return successfully into test mode
          }
          else if(ETS_RX_DATA[1]==0x2)//exit Test Mode
          {
            TestMode=0;
            ETS_TX_DATA[0]=2;//return successfully into test mode
          }       
          else 
          #endif
          if(ETS_RX_DATA[1]==0x3)//into administrator Mode
          {
            ETS_TX_DATA[0]=3;//return successfully into admin mode
          }
          
          else if(ETS_RX_DATA[1]==0x4)//into  Burst flash mode
          {
            ////////////  Bootload Mode  ////////////
        		//Read the Calibration data from Flash then Write to RAM
          	WDOG_Feed();
            OpenFlash();
        		Cal_data[INFO_BOOTLOADER]=0xFF;//next into Burst flash mode
            Cpu_Delay100US();

						_DI();
        		CloseFlash();
        		for(;;);//Not return value,will into Burst flash mode on restart   	
          } 
        }
      }

  		break;   	   	
  			       
  	default:    	
		  ETS_TX_CMD = ETS_INVALID_COMMAND;
	}
	
prepare_to_transmit:
	ETS_TX_BYTES = tx_bytes;
	
	//Create check sum
	check_sum = 0;
	for (i=0; i<(uint8_t)(tx_bytes+2); i++)
		check_sum += ETS_TX_RAW[i];
	
	ETS_TX_DATA[tx_bytes] = check_sum;  //load checksum
	
	ETS_TX_Buf.index = 0;
	ETS_Tx_Status = ETS_TRANSMITTING; //send
	
	return;
	
}
