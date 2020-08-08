/*
 * LUART_Init.c
 *
 *  Created on: 2017Äê8ÔÂ22ÈÕ
 *      Author: ted
 */

#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "typedefine.h"
#include "PE_Error.h"

APP_SECTION void LPUART0_init(void)  /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
  PCC->PCCn[PCC_LPUART0_INDEX] &= ~PCC_PCCn_CGC_MASK;    /* Ensure clk disabled for config */
  PCC->PCCn[PCC_LPUART0_INDEX] |= PCC_PCCn_PCS(0b001)    /* Clock Src= 1 (SOSCDIV2_CLK) */
                               |  PCC_PCCn_CGC_MASK;     /* Enable clock for LPUART1 regs */

  LPUART0->BAUD = 0x0F00001A;  /* Initialize for 19200 baud, 1 stop: */
                               /* SBR=52 (0x34): baud divisor = 8M/19200/16 = ~26 */
                               /* OSR=15: Over sampling ratio = 15+1=16 */
                               /* SBNS=0: One stop bit */
                               /* BOTHEDGE=0: receiver samples only on rising edge */
                               /* M10=0: Rx and Tx use 7 to 9 bit data characters */
                               /* RESYNCDIS=0: Resync during rec'd data word supported */
                               /* LBKDIE, RXEDGIE=0: interrupts disable */
                               /* TDMAE, RDMAE, TDMAE=0: DMA requests disabled */
                               /* MAEN1, MAEN2,  MATCFG=0: Match disabled */

  LPUART0->CTRL=0x000C0000;    /* Enable transmitter & receiver, no parity, 8 bit char: */
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

APP_SECTION uint8_t LPUART0_transmit_char(uint8_t Chr) {    /* Function to Transmit single Char */
  if((LPUART0->STAT & LPUART_STAT_TDRE_MASK) == 0) {                   /* Is the transmitter empty? */
    return ERR_TXFULL;                 /* If no then error */
  }
  LPUART0->DATA = (uint8_t)Chr;                   /* Store char to the transmitter register */
  return ERR_OK;                       /* OK */
}


APP_SECTION uint8_t LPUART0_receive_char(uint8_t *Chr) {    /* Function to Receive single Char */
	uint32_t StatReg = LPUART0->STAT;               /* Read status register */

  if (StatReg & (LPUART_STAT_OR_MASK|LPUART_STAT_NF_MASK|LPUART_STAT_FE_MASK|LPUART_STAT_PF_MASK)) { /* Is any error set? */
    LPUART0->STAT |= (LPUART_STAT_OR_MASK|LPUART_STAT_NF_MASK|LPUART_STAT_FE_MASK|LPUART_STAT_PF_MASK);
    return ERR_COMMON;
  } else if ((StatReg & LPUART_STAT_RDRF_MASK) == 0) { /* Is the reciver empty and no error is set? */
    return ERR_RXEMPTY;                /* If yes then error */
  } else {                             /* Intentionally left empty due to compatibility with MISRA rule 60*/
  }
  *Chr = LPUART0->DATA;                        /* Read data from the receiver */
  return ERR_OK;                       /* Return error code */
}


APP_SECTION void LPUART1_init(void)  /* Init. summary: 9600 baud, 1 stop bit, 8 bit format, no parity */
{
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

APP_SECTION uint8_t LPUART1_transmit_char(uint8_t Chr) {    /* Function to Transmit single Char */

  if((LPUART1->STAT & LPUART_STAT_TDRE_MASK) == 0) {                   /* Is the transmitter empty? */
    return ERR_TXFULL;                 /* If no then error */
  }
  LPUART1->DATA = (uint8_t)Chr;                   /* Store char to the transmitter register */
  return ERR_OK;                       /* OK */
}


APP_SECTION uint8_t LPUART1_receive_char(uint8_t *Chr) {    /* Function to Receive single Char */
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

