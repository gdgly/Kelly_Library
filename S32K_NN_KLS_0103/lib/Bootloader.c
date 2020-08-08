/*
 * Bootloader.c
 *
 *  Created on: Jul 31, 2014
 *      Author: Admin
 */
/* Including needed modules to compile this module/procedure */

/* Including shared modules, which are used for whole project */
#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"
#include "Variable.h"
#include "Flash.h"
#include "Function.h"
#include "ETS.h"
#include "Platform.h"


byte Boot_SCI1_RecvChar(byte *Chr)
{
	uint32_t StatReg = LPUART1->STAT;               /* Read status register */

  if (StatReg & (LPUART_STAT_OR_MASK|LPUART_STAT_NF_MASK|LPUART_STAT_FE_MASK|LPUART_STAT_PF_MASK)) { /* Is any error set? */
    LPUART1->STAT |= (LPUART_STAT_OR_MASK|LPUART_STAT_NF_MASK|LPUART_STAT_FE_MASK|LPUART_STAT_PF_MASK);
    return ERR_COMMON;
  } else if ((StatReg & LPUART_STAT_RDRF_MASK) == 0) { /* Is the reciver empty and no error is set? */
    return ERR_RXEMPTY;                /* If yes then error */
  } else {                             /* Intentionally left empty due to compatibility with MISRA rule 60*/
  }
  *Chr = LPUART1->DATA;                        /* Read data from the receiver */
  return ERR_OK;                       /* Return error code */
}

byte Boot_SCI1_SendChar(byte Chr)
{
  if((LPUART1->STAT & LPUART_STAT_TDRE_MASK) == 0) {                   /* Is the transmitter empty? */
    return ERR_TXFULL;                 /* If no then error */
  }
  LPUART1->DATA = (uint8_t)Chr;                   /* Store char to the transmitter register */
  return ERR_OK;                       /* OK */
}


void boot_init(void)
{

	_DI();
	WDOG->CNT=0xD928C520; 	/*Unlock watchdog*/
	WDOG->TOVAL=0x0000FFFF;	/*Maximum timeout value*/
	WDOG->CS &= ~WDOG_CS_EN(1);  /*Disable watchdog*/

	SCG->SOSCDIV=0x00000101;  /* SOSCDIV1 & SOSCDIV2 =1: divide by 1 */
  SCG->SOSCCFG=0x00000024;  /* Range=2: Medium freq (SOSC betw 1MHz-8MHz)*/
                            /* HGO=0:   Config xtal osc for low power */
                            /* EREFS=1: Input is external XTAL */
  while(SCG->SOSCCSR & SCG_SOSCCSR_LK_MASK); /* Ensure SOSCCSR unlocked */
  SCG->SOSCCSR=0x00000001;  /* LK=0:          SOSCCSR can be written */
                            /* SOSCCMRE=0:    OSC CLK monitor IRQ if enabled */
                            /* SOSCCM=0:      OSC CLK monitor disabled */
                            /* SOSCERCLKEN=0: Sys OSC 3V ERCLK output clk disabled */
                            /* SOSCLPEN=0:    Sys OSC disabled in VLP modes */
                            /* SOSCSTEN=0:    Sys OSC disabled in Stop modes */
                            /* SOSCEN=1:      Enable oscillator */
  while(!(SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK)); /* Wait for sys OSC clk valid */

  //SPLL_init_160MHz
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000000;  /* SPLLEN=0: SPLL is disabled (default) */
  SCG->SPLLDIV = 0x00000202;  /* SPLLDIV1 divide by 2; SPLLDIV2 divide by 2 */
  SCG->SPLLCFG = 0x00180000;  /* PREDIV=0: Divide SOSC_CLK by 0+1=1 */
							  /* MULT=24:  Multiply sys pll by 16+24=40 */
							  /* SPLL_CLK = 8MHz/1 * 40 / 2 = 160 MHz */
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000001; /* LK=0:        SPLLCSR can be written */
							 /* SPLLCMRE=0:  SPLL CLK monitor IRQ if enabled */
							 /* SPLLCM=0:    SPLL CLK monitor disabled */
							 /* SPLLSTEN=0:  SPLL disabled in Stop modes */
							 /* SPLLEN=1:    Enable SPLL */
  while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait for SPLL valid */

  //NormalRUNmode_80MHz
  SCG->RCCR=SCG_RCCR_SCS(6)      /* PLL as clock source*/
    |SCG_RCCR_DIVCORE(0b01)      /* DIVCORE= 2, Core clock = 160/2 MHz = 80 MHz*/
    |SCG_RCCR_DIVBUS(0b01)       /* DIVBUS = 2, bus clock = 40 MHz*/
    |SCG_RCCR_DIVSLOW(0b10);     /* DIVSLOW = 3, SCG slow, flash clock= 26.6 MHz*/
  if ((SCG->CSR & SCG_CSR_SCS_MASK >> SCG_CSR_SCS_SHIFT ) != 6) {}
                                 /* Wait for sys clk src = SPLL */


	PCC->PCCn[PCC_PORTA_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTA */
	PCC->PCCn[PCC_PORTB_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTB */
	PCC->PCCn[PCC_PORTC_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTC */
	PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTD */
	PCC->PCCn[PCC_PORTE_INDEX ]|=PCC_PCCn_CGC_MASK; /* Enable clock for PORTE */
	//UART1
	PORTC->PCR[9]=PORT_PCR_MUX(2);
	PTC_PDDR_P9 = 1;
	PORTC->PCR[8]=PORT_PCR_MUX(2);
	PTC_PDDR_P8 = 0;

  PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;    /* Ensure clk disabled for config */
  PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(0b001)    /* Clock Src= 1 (SOSCDIV2_CLK) */
                               |  PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */

  LPUART1->BAUD = 0x0F00001A;  /* Initialize for 19200 baud, 1 stop: */
                               /* SBR=52 (0x34): baud divisor = 8M/19200/16 = ~26 */
                               /* OSR=15: Over sampling ratio = 15+1=16 */
                               /* SBNS=0: One stop bit */
                               /* BOTHEDGE=0: receiver samples only on rising edge */
                               /* M10=0: Rx and Tx use 7 to 9 bit data characters */
                               /* RESYNCDIS=0: Resync during rec'd data word supported */
                               /* LBKDIE, RXEDGIE=0: interrupts disable */
                               /* TDMAE, RDMAE, TDMAE=0: DMA requests disabled */
                               /* MAEN1, MAEN2,  MATCFG=0: Match disabled */

  LPUART1->CTRL=0x000C0000;    /* Enable transmitter & receiver, no parity, 8 bit char: */
                               /* RE=1: Receiver enabled */
                               /* TE=1: Transmitter enabled */
                               /* PE,PT=0: No hw parity generation or checking */
                               /* M7,M,R8T9,R9T8=0: 8-bit data characters*/
                               /* DOZEEN=0: LPUART enabled in Doze mode */
                               /* ORIE,NEIE,FEIE,PEIE,TIE,TCIE,RIE,ILIE,MA1IE,MA2IE=0: no IRQ*/
                               /* TxDIR=0: TxD pin is input if in single-wire mode */
                               /* TXINV=0: TRansmit data not inverted */
                               /* RWU,WAKE=0: normal operation; rcvr not in statndby */
                               /* IDLCFG=0: one idle character */
                               /* ILT=0: Idle char bit count starts after start bit */
                               /* SBK=0: Normal transmitter operation - no break char */
                               /* LOOPS,RSRC=0: no loop back */
	
}


void boot_Init_WDOG (void)
{
	WDOG->CNT = 0xD928C520; //unlock watchdog
	while((WDOG->CS & WDOG_CS_ULK_MASK)==0); //wait until registers are unlocked
	WDOG->TOVAL = 0xffff; //set timeout value
	WDOG->CS |= WDOG_CS_EN(1) | WDOG_CS_CLK(2) | WDOG_CS_INT(0) |
			WDOG_CS_WIN(0) | WDOG_CS_UPDATE(1) | WDOG_CS_PRES(0);
	while((WDOG->CS & WDOG_CS_RCS_MASK)==0); //wait until new configuration takes effect
}

void boot_WDOG_Feed (void)
{
	WDOG->CNT = 0xB480A602; // refresh watchdog
}

void boot(void)
{
  volatile uint32_t i=0,j,Boot_Check_ETS=0;
  uint8_t Return_code=0,Recv_Value=0;
  
	uint8_t command=0,check_sum=0, tx_bytes=0;
	uint32_t par=0,Code_End_Address=0;
	
	T_ETS_Buf ETS_RX_Buf;
	T_ETS_Buf ETS_TX_Buf;

	T_ETS_Tx_State	ETS_Tx_Status=0 ;
	T_ETS_Rx_State	ETS_Rx_Status=0 ;      
  
  boot_init();
	//FLASH_Init();
  memcpy(&__MY_RAM_ADR , &__MY_RAM_ROM_ADR, &__MY_RAM_SIZE);
	boot_WDOG_Feed();

	for(;;)
	{
		boot_WDOG_Feed();
		
	  Boot_Check_ETS++;
		
		if(Boot_Check_ETS>100000)		//ETS运行出错
		{
	    ETS_RX_Buf.index = 0;
			ETS_Rx_Status = ETS_RECEIVED;
		}
		
  	Return_code=Boot_SCI1_RecvChar(&Recv_Value);
  	
  	if(Return_code==ERR_OK)
  	{      
  		Boot_Check_ETS = 0;
  		
  		if (ETS_Rx_Status == ETS_RECEIVED)
  		{	//new command
  			ETS_RX_Buf.index=0;
  			ETS_RX_BYTES=0;
  			ETS_Rx_Status = ETS_RECEIVING;
  		}
  		
  		if(ETS_RX_Buf.index<ETS_MAX_DATA_LEN+3)
  		{
  		  ETS_RX_RAW[ETS_RX_Buf.index]=Recv_Value;
  		  ETS_RX_Buf.index++;
  		  
  		}
  			
  		
  		if (ETS_RX_BYTES > ETS_MAX_DATA_LEN) ETS_RX_BYTES = ETS_MAX_DATA_LEN;   	
  		
  		i=ETS_RX_BYTES+3;
  		if(ETS_RX_Buf.index >= i) //all bytes received
  		{
  			ETS_Rx_Status = ETS_RECEIVED;
  			
  			
  			///// ETS action /////
      	tx_bytes = 1;			 			       	
      	check_sum = 0;
      	j=ETS_RX_BYTES+2;
      	for (i=0; i<j; i++)
      		check_sum += ETS_RX_RAW[i];
      	
      	if(check_sum != ETS_RX_DATA[ETS_RX_BYTES])
      	{
      		ETS_TX_CMD = ETS_CHECKSUM_ERROR;
      		command=0;//error command
      	}
      	else
      	{       	  
      	  ETS_TX_CMD = command = ETS_RX_CMD;
      	}
      	
      	
      	if (command==ETS_CODE_VERSION)
      	{
        		tx_bytes = 3;
			  		ETS_TX_DATA[0] = SOFT_VER_H;
			  		ETS_TX_DATA[1] = SOFT_VER_L;
			  		ETS_TX_DATA[2] = 0x02;	//01 代表软件处在Bootloader区
      	}
        else if (command==ETS_ERASE_FLASH)
        {          
            if(ETS_RX_DATA[0]==1)//earse Calibration page
            {     
              par=CAL_DATA_FLASH_ADDRESS;
              for(;par<CODE_START_ADDRESS;par+=2048) //Erase Code and Data flash,Bootloader area no erase
              { 
              	j=FLASH_EraseSector(par);
              }

            }
            else if(ETS_RX_DATA[0] == 2)				//只擦除代码区
            {
              par=CODE_START_ADDRESS;
              for(;par<CODE_END_ADDRESS;par+=2048) //Erase Code and Data flash,Bootloader area no erase
              { 
              	j=FLASH_EraseSector(par);
              }
            }
            else if(ETS_RX_DATA[0] == 3) 				//擦除标定区和代码区
            {
            	par=CAL_DATA_FLASH_ADDRESS;
              for(;par<CODE_END_ADDRESS;par+=2048) //Erase Code and Data flash,Bootloader area no erase
              { 
              	j=FLASH_EraseSector(par);
              }
            }

            if(j > 0)
            	ETS_TX_DATA[0]=0;//erase flash succeed or error,0 is error,1 is succeed
            else
            	ETS_TX_DATA[0]=1;
        }
        else if (command==ETS_BURNT_FLASH)
        {
            if(ETS_RX_DATA[12] == 1)
            {
              par=CAL_DATA_FLASH_ADDRESS+((ETS_RX_DATA[0]<<24) | (ETS_RX_DATA[1]<<16) | (ETS_RX_DATA[2]<<8) | (ETS_RX_DATA[3]));//flash address
            }
            else
            {
              par=CODE_START_ADDRESS+((ETS_RX_DATA[0]<<24) | (ETS_RX_DATA[1]<<16) | (ETS_RX_DATA[2]<<8) | (ETS_RX_DATA[3]));//flash address
            }
        		

          for(j=0;j<8;j++)
          {
          	Cal_data[j] = ETS_RX_DATA[4 + j];
          }
          
          j = FLASH_Program(par,&Cal_data[0],8 );
          
         if(j > 0)
          	ETS_TX_DATA[0]=0;//erase flash succeed or error,0 is error,1 is succeed
          else
          	ETS_TX_DATA[0]=1;    
        }  
        else if (command==ETS_CODE_END_ADDRESS)
				{
					Code_End_Address = (uint32_t)(ETS_RX_DATA[0]<<24)
														|(uint32_t)(ETS_RX_DATA[1]<<16)
														|(uint32_t)(ETS_RX_DATA[2]<<8)
														|(uint32_t)ETS_RX_DATA[3];//算出.mot文件中代码结束地址
				}
      	else if (command==ETS_BURNT_CHECKSUM)
      	{
        	check_sum=0;
        	if(ETS_RX_DATA[0]==1)	//with calibration data checksum
        	{
        	  par=CAL_DATA_FLASH_ADDRESS;      
          	for(;par<CODE_START_ADDRESS;par++)
          	{
          	  check_sum+= *(uint8_t *)(par);
          	}
        	}
        	else if(ETS_RX_DATA[0]==2)
        	{
        	  par=CODE_START_ADDRESS;    
          	for(;par<Code_End_Address;par++)
          	{
          	  check_sum+= *(uint8_t *)(par);
          	}
        	}
        	else if(ETS_RX_DATA[0]==3)
        	{
        		par=CAL_DATA_FLASH_ADDRESS;
          	for(;par<Code_End_Address;par++)
          	{
          	  check_sum+= *(uint8_t *)(par);
          	}
        	}
        	

        	if(check_sum==ETS_RX_DATA[1])
        	  ETS_TX_DATA[0] = 1;//OK
        	else
        	  ETS_TX_DATA[0] = 0;//error
        	
      	}   
      	else if (command==ETS_BURNT_RESET)
      	{
        	OpenFlash();
        	Cal_data[INFO_BOOTLOADER]=0x0;//next into normally mode

        	_DI();
        	CloseFlash();	
      	  for(;;);//wait to watch dog reset
      	}
        else
        { 
          tx_bytes = 0;      

      		ETS_TX_CMD = ETS_INVALID_COMMAND;
          
        }
        			                           
      	ETS_TX_BYTES = tx_bytes;
      	
      	//Create check sum
      	check_sum = 0;
      	j=tx_bytes+2;
      	for (i=0; i<j; i++)
      		check_sum += ETS_TX_RAW[i];
      	
      	ETS_TX_DATA[tx_bytes] = check_sum;  //load checksum
      	
      	ETS_TX_Buf.index = 0;
      	ETS_Tx_Status = ETS_TRANSMITTING; //send
  		}
  	}
  	boot_WDOG_Feed();
  	//Send
  	if (ETS_Tx_Status != ETS_TRANSMITTED)
  	{
  		Return_code=Boot_SCI1_SendChar(ETS_TX_RAW[ETS_TX_Buf.index]);
  		if(Return_code==ERR_OK)
  		{                                 
  			if(ETS_TX_Buf.index >= (uint8_t)(ETS_TX_BYTES+2))
  				ETS_Tx_Status = ETS_TRANSMITTED;
  			ETS_TX_Buf.index++;
  		}
  	}

	}
}


