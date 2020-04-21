/*
**     Comment     :
**         Actually 1 ms period using 40MHz processor clock
**     Settings    :
**          Component name                                 : SysTick
**          Device                                         : SysTick
**          Settings                                       : 
**            Clock source                                 : Processor clock
**            Reload value                                 : 39999
**            Counter period                               : 1 ms
**          Interrupts                                     : 
**            Interrupt                                    : INT_SysTick
**            Interrupt priority                           : 2
**            ISR Name                                     : SysTick_ISR
**            Timer interrupt                              : Enabled
*/

#include "KEA64/SysTick.h"

#include <stdint.h>

#define SYST_CSR 	(*((uint32_t *)0xE000E010))		// SysTick Control and Status Register
#define SYST_RVR 	(*((uint32_t *)0xE000E014))		// SysTick Reload Value Register
#define SYST_CVR 	(*((uint32_t *)0xE000E018))		// SysTick Current Value Register
#define SYST_CALIB 	(*((uint32_t *)0xE000E01C))		// SysTick Calibration Value Register
#define SCB_SHPR3 	(*((uint32_t *)0xE000ED20))		// System Handler Priority Register 3

#define SysTick_CSR_ENABLE_MASK                  0x1u
#define SysTick_CSR_ENABLE_SHIFT                 0
#define SysTick_CSR_ENABLE_WIDTH                 1
#define SysTick_CSR_ENABLE(x)                    (((uint32_t)(((uint32_t)(x))<<SysTick_CSR_ENABLE_SHIFT))&SysTick_CSR_ENABLE_MASK)
#define SysTick_CSR_TICKINT_MASK                 0x2u
#define SysTick_CSR_TICKINT_SHIFT                1
#define SysTick_CSR_TICKINT_WIDTH                1
#define SysTick_CSR_TICKINT(x)                   (((uint32_t)(((uint32_t)(x))<<SysTick_CSR_TICKINT_SHIFT))&SysTick_CSR_TICKINT_MASK)
#define SysTick_CSR_CLKSOURCE_MASK               0x4u
#define SCB_SHPR3_PRI_15_MASK                    0xFF000000u
#define SCB_SHPR3_PRI_15_SHIFT                   24
#define SCB_SHPR3_PRI_15_WIDTH                   8
#define SCB_SHPR3_PRI_15(x)                      (((uint32_t)(((uint32_t)(x))<<SCB_SHPR3_PRI_15_SHIFT))&SCB_SHPR3_PRI_15_MASK)

void SysTick_Init(void)
{
	/* SYST_CSR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,COUNTFLAG=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CLKSOURCE=0,TICKINT=0,ENABLE=0 */
	SYST_CSR = 0x00U;
	/* SYST_RVR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,RELOAD=0x9C3F */
	SYST_RVR = 0x9C3F;
	/* SYST_CVR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CURRENT=0 */
	SYST_CVR = 0x00;
	/* SYST_CSR: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,COUNTFLAG=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,CLKSOURCE=1,TICKINT=1,ENABLE=1 */
	SYST_CSR = SysTick_CSR_CLKSOURCE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_ENABLE_MASK;

	SCB_SHPR3 = (uint32_t)((SCB_SHPR3 & (uint32_t)~(uint32_t)(SCB_SHPR3_PRI_15(0xFF))) | (uint32_t)(SCB_SHPR3_PRI_15(0x80)));
}

//{
//	//SysTick_Init();
//	SYST_RVR = (fCPU / 1000) - 1;
//	SYST_CVR = 0;
//	SYST_CSR = SysTick_CSR_CLKSOURCE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_ENABLE_MASK;
//	// KE/KEA PE priority 2 -> PRI_15 = 0x80
//	SCB_SHPR3 = (uint32_t)((SCB_SHPR3 & (uint32_t)~(uint32_t)(SCB_SHPR3_PRI_15(0xFF))) | (uint32_t)(SCB_SHPR3_PRI_15(priroity)));
//}
