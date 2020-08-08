/*
 * LPIT_Init.c
 *
 *  Created on: 2017Äê8ÔÂ21ÈÕ
 *      Author: ted
 */
#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "typedefine.h"



APP_SECTION void Init_LPIT0 (void) {
  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK),40MHz*/
  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs */
  LPIT0->MCR = 0x00000001;    /* DBG_EN-0: Timer chans stop in Debug mode */
                              /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
                              /* SW_RST=0: SW reset does not reset timer chans, regs */
                              /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */
  LPIT0->MIER = 0x00000001;   /* TIE0=1: Timer Interrupt Enabled fot Chan 0 */
  LPIT0->TMR[0].TVAL = 8000;    //100us/* Chan 0 Timeout period: 80M clocks */
  LPIT0->TMR[0].TCTRL = 0x00000000; /* T_EN=1: Timer channel is enabled */
                              /* CHAIN=0: channel chaining is disabled */
                              /* MODE=0: 32 periodic counter mode */
                              /* TSOT=0: Timer decrements immediately based on restart */
                              /* TSOI=0: Timer does not stop after timeout */
                              /* TROT=0 Timer will not reload on trigger */
                              /* TRG_SRC=0: External trigger soruce */
                              /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}
