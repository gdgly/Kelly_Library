/*
 * FTM_Init.c
 *
 *  Created on: 2018年5月28日
 *      Author: Administrator
 */

#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "typedefine.h"

APP_SECTION void Init_Cap_Period (FTM_Type * FTM,uint32_t PCC_INDEX,uint32_t CHn)
{
	PCC->PCCn[PCC_INDEX] &= ~PCC_PCCn_CGC_MASK; 	/* Ensure clk disabled for config */
  PCC->PCCn[PCC_INDEX] |= PCC_PCCn_PCS(0x06)	/* Clock Src=6, 40MHZ SPLLDIV1_CLK */
                           |  PCC_PCCn_CGC_MASK;   /* Enable clock for FTM regs */

  FTM->MODE |= FTM_MODE_WPDIS_MASK;  /* Write protect to registers disabled (default) */
  FTM->SC = 0x00000105;     /* Disable PWM channel 0~2 output*/
                             /* Disable PWM channel 4~6 output*/
                             /* TOIE (Timer Overflow Interrupt Ena) = 1  */
                             /* CPWMS (Center aligned PWM Select) = 0 (default, up-down count) */
                             /* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
                             /* PS (Prescaler factor) = 5. Prescaler = 32 */
  FTM->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0  */
  FTM->POL = 0x00000000;    /* Polarity for all channels is active high (default) */
  FTM->MOD = 0;     /* FTM1 counter final value (used for PWM mode) */

  FTM->CONTROLS[CHn].CnSC = 0x00000044;//SA1捕捉上升沿，捕捉事件发生不复位CNT计数器

  FTM->SC |= FTM_SC_CLKS(1);//选择FTM系统时钟（80MHz）作为时钟源，启动FTM2
}

APP_SECTION void Init_QD_Mode (FTM_Type * FTM,uint32_t PCC_INDEX,uint32_t Pulses,uint32_t Swap)
{
	PCC->PCCn[PCC_INDEX] &= ~PCC_PCCn_CGC_MASK; 	/* Ensure clk disabled for config */
  PCC->PCCn[PCC_INDEX] |= PCC_PCCn_PCS(0x06)	/* Clock Src=6, 40MHZ SPLLDIV1_CLK */
                      	   |  PCC_PCCn_CGC_MASK;   /* Enable clock for FTM regs */

  FTM->MODE |= FTM_MODE_WPDIS_MASK;  /* Write protect to registers disabled (default) */
  FTM->SC = 0x00000000;     /* Disable PWM channel 0~2 output*/
                             /* Disable PWM channel 4~6 output*/
                             /* TOIE (Timer Overflow Interrupt Ena) = 1  */
                             /* CPWMS (Center aligned PWM Select) = 0 (default, up-down count) */
                             /* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
                             /* PS (Prescaler factor) = 5. Prescaler = 32 */
  FTM->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0  */
  FTM->POL = 0x00000000;    /* Polarity for all channels is active high (default) */

  FTM->MOD = (Pulses <<2) -1 ;     /* FTM1 counter final value (used for PWM mode) */
  FTM->CNTIN = 0;
  if(Swap == 0)
  	FTM->QDCTRL = 0x01;//不交换编码器AB
	else
		FTM->QDCTRL = 0x11;//交换编码器AB

  FTM->SC |= FTM_SC_CLKS(1);//选择FTM系统时钟（80MHz）作为时钟源，启动FTM2
}

APP_SECTION void Init_Hall_Cap (FTM_Type * FTM,uint32_t PCC_INDEX)//
{
	PCC->PCCn[PCC_INDEX] &= ~PCC_PCCn_CGC_MASK; 	/* Ensure clk disabled for config */
  PCC->PCCn[PCC_INDEX] |= PCC_PCCn_PCS(0x06)	/* Clock Src=6, 40MHZ SPLLDIV1_CLK */
                           |  PCC_PCCn_CGC_MASK;   /* Enable clock for FTM regs */

  FTM->MODE |= FTM_MODE_WPDIS_MASK;  /* Write protect to registers disabled (default) */
  FTM->SC = 0x00000105;     /* Disable PWM channel 0~2 output*/
                             /* Disable PWM channel 4~6 output*/
                             /* TOIE (Timer Overflow Interrupt Ena) = 1  */
                             /* CPWMS (Center aligned PWM Select) = 0 (default, up-down count) */
                             /* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
                             /* PS (Prescaler factor) = 5. Prescaler = 32 */
  FTM->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0  */
  FTM->POL = 0x00000000;    /* Polarity for all channels is active high (default) */
  FTM->MOD = 0 ;     /* FTM1 counter final value (used for PWM mode) */

  FTM->CONTROLS[2].CnSC = 0x0000004C;//SA捕捉
  FTM->CONTROLS[3].CnSC = 0x0000004C;//SB捕捉
  FTM->CONTROLS[4].CnSC = 0x0000004E;//SC捕捉，捕捉事件发生复位CNT计数器
  //FTM->CONTROLS[5].CnSC = 0x0000000C;//SD仅捕捉
  FTM->SC |= FTM_SC_CLKS(1);//选择FTM系统时钟（80MHz）作为时钟源，启动FTM2
}

APP_SECTION void Init_PWM (FTM_Type * FTM, uint32_t PCC_INDEX,uint32_t Modul_Hz)
{
	PCC->PCCn[PCC_INDEX] &= ~PCC_PCCn_CGC_MASK; 	/* Ensure clk disabled for config */
  PCC->PCCn[PCC_INDEX] |= PCC_PCCn_PCS(0x06)	/* Clock Src=6, 40MHZ SPLLDIV1_CLK */
                           |  PCC_PCCn_CGC_MASK;   /* Enable clock for FTM regs */
  FTM->MODE |= FTM_MODE_WPDIS_MASK;  /* Write protect to registers disabled (default) */
  FTM->SC = 0x00E00120;     /* Disable PWM channel 0~4 output*/
                             /* Enable PWM channel 5~7 output*/
                             /* TOIE (Timer Overflow Interrupt Ena) = 0 (default) */
                             /* CPWMS (Center aligned PWM Select) = 1 (default, up-down count) */
                             /* CLKS (Clock source) = 0 (default, no clock; FTM disabled) */
                             /* PS (Prescaler factor) = 0. Prescaler = 1 */
  FTM->COMBINE = 0x00000000;/* FTM mode settings used: DECAPENx, MCOMBINEx, COMBINEx=0  */
  FTM->POL = 0x00000000;    /* Polarity for all channels is active high (default) */

  if(Modul_Hz == 10)
  	FTM->MOD = 4000 ;     /* FTM1 counter final value (used for PWM mode) */
  else
  	FTM->MOD = 6400 ;


  FTM->CONTROLS[5].CnSC = 0x00000008;  // PWM1 FTM ch0: Center-Aligned PWM, High true pulses ，CHIE (Chan Interrupt Ena) = 0 (default)
	FTM->CONTROLS[6].CnSC = 0x00000008;
	FTM->CONTROLS[7].CnSC = 0x00000008;
}

APP_SECTION void Start_PWM (FTM_Type * FTM)
{
	FTM->SC |= FTM_SC_CLKS(1);//选择FTM系统时钟（80MHz）作为时钟源，启动FTM
}
