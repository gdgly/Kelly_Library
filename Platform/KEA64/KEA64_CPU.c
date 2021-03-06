#include "IO_Map.h"

#include <stdint.h>

void CPU_InitClock40MHz(void)
{
	/* System clock initialization */
	if ( *((uint8_t*) 0x03FFU) != 0xFFU) {
	  ICS_C3 = *((uint8_t*) 0x03FFU);
	  ICS_C4 = (ICS_C4 & 0xFEU) | ((*((uint8_t*) 0x03FEU)) & 0x01U);
	}
	/* ICS_C2: BDIV|=1 */
	ICS_C2 |= ICS_C2_BDIV(0x01);         /* Update system prescalers */
	/* SIM_BUSDIV: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,BUSDIV=0 */
	SIM_BUSDIV = 0x00U;                  /* Update system prescalers */
	/* Switch to FEI Mode */
	/* ICS_C1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=1 */
	ICS_C1 = ICS_C1_CLKS(0x00) |
			 ICS_C1_RDIV(0x00) |
			 ICS_C1_IREFS_MASK |
			 ICS_C1_IRCLKEN_MASK |
			 ICS_C1_IREFSTEN_MASK;
	/* ICS_C2: BDIV=1,LP=0 */
	ICS_C2 = (uint8_t)((ICS_C2 & (uint8_t)~(uint8_t)(
			  ICS_C2_BDIV(0x06) |
			  ICS_C2_LP_MASK
			 )) | (uint8_t)(
			  ICS_C2_BDIV(0x01)
			 ));
	/* OSC_CR: OSCEN=0,??=0,OSCSTEN=1,OSCOS=0,??=0,RANGE=0,HGO=0,OSCINIT=0 */
	OSC_CR = OSC_CR_OSCSTEN_MASK;
	while((ICS_S & ICS_S_IREFST_MASK) == 0x00U) { /* Check that the source of the FLL reference clock is the internal reference clock. */
	}
	while((ICS_S & 0x0CU) != 0x00U) {    /* Wait until output of the FLL is selected */
	}
	/*** End of PE initialization code after reset ***/

	/*** User code after PE initialization and before C initialization ***/
	// Change ICS to 40MHz
	// If user wants to use 40 MHz core clock , this bit must be set to 1 before changing ICS_C2[BDIV]=0x000.
	/* SIM_BUSDIV: BUSDIV=1 */
	SIM_BUSDIV = 0x01U;
	// NOTE: If user want to use 40 MHz core clock, this bit need clear to 000b after SIM_BUSDIV[BUSDIV]=1.
	/* ICS_C2: BDIV=0 */
	ICS_C2 = (uint8_t)(ICS_C2 & (uint8_t)~(uint8_t)(ICS_C2_BDIV_MASK));
	/*** End of user code after PE initialization and before C initialization ***/
}
