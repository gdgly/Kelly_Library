/*
 * Millis.c
 *
 * Set up millisecond counter, using SysTick timer.
 *
 */

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

static volatile uint32_t MillisTickCount = 0;
	
static void (*OnTick)(void);
static void (*Yield)(void);

void SysTick_ISR(void)
{
	MillisTickCount++;
	if (OnTick) OnTick();
}

uint32_t Millis(void)
{
	return MillisTickCount;
}

uint32_t Micros(void)
{
	#define F_CPU 40000000
	uint32_t micros = (F_CPU / 1000 - 1) - (SYST_CVR / (F_CPU / 1000000));
	return MillisTickCount * 1000 + micros;
}

void Delay(uint32_t ms)
{
	uint32_t start = MillisTickCount;
	uint32_t burn = 1;

	while (MillisTickCount - start < ms)
	{
		if (Yield) Yield();
		while (MillisTickCount - start < burn);
		burn++;
	}
}

volatile uint32_t * Millis_GetTickCounter(void)
{
	return &MillisTickCount;
}

void Millis_SetYield(void (*fp)(void))
{
	Yield = fp;
}

void Millis_SetOnTick(void (*fp)(void))
{
	OnTick = fp;
}


void Millis_Init(uint32_t fCPU, uint8_t priroity)
{
	//SysTick_Init();
	SYST_RVR = (fCPU / 1000) - 1;
	SYST_CVR = 0;
	SYST_CSR = SysTick_CSR_CLKSOURCE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_ENABLE_MASK;
	// KE/KEA PE priority 2 -> PRI_15 = 0x80
	SCB_SHPR3 = (uint32_t)((SCB_SHPR3 & (uint32_t)~(uint32_t)(SCB_SHPR3_PRI_15(0xFF))) | (uint32_t)(SCB_SHPR3_PRI_15(priroity)));
}
