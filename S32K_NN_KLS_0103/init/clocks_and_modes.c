/*
 * clocks_and_modes.c              Copyright NXP 2016
 * Description: Example clock and mode functions
 * 2015 Feb 22 S Mihalik/ O Romero - initial version
 * 2016 Oct 15 S.M. / O.R. - Updated PLL per S32K1XX data sheet rev 1
 *
 */

#include "S32K142.h"           /* include peripheral declarations S32K144 */
#include "clocks_and_modes.h"
#include "s32_core_cm4.h"
#include "typedefine.h"


void disable_WDOG (void)
{
	WDOG->CNT=0xD928C520; 	/*Unlock watchdog*/
	WDOG->TOVAL=0x0000FFFF;	/*Maximum timeout value*/
	WDOG->CS &= ~WDOG_CS_EN(1);  /*Disable watchdog*/
}

void Init_WDOG (void)
{
	WDOG->CNT = 0xD928C520; //unlock watchdog
	while((WDOG->CS & WDOG_CS_ULK_MASK)==0); //wait until registers are unlocked
	WDOG->TOVAL = 0xffff; //set timeout value
	WDOG->CS |= WDOG_CS_EN(1) | WDOG_CS_CLK(2) | WDOG_CS_INT(0) |
			WDOG_CS_WIN(0) | WDOG_CS_UPDATE(1) | WDOG_CS_PRES(0);
	while((WDOG->CS & WDOG_CS_RCS_MASK)==0); //wait until new configuration takes effect
}

void WDOG_Feed (void)
{
	_DI();// disable global interrupt
	WDOG->CNT = 0xB480A602; // refresh watchdog
	_EI(); // enable global interrupt
}
/*==========================================================================
 * 函数名:SOSC_init_8MHz
 * 功能描述:外部晶振初始化。
 * SOSCDIV1 & SOSCDIV2为8M时钟。
 *
 * 参数:
 * 参数解释:
 *
 * 返回值:
 *
 * 作 者:        ted, 2018 年 05 月 24 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void SOSC_init_8MHz(void) {
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
}
/*==========================================================================
 * 函数名:NormalRUNmode_80MHz
 * 功能描述:标准模式80MHz。SPLL_CLK=160MHz。
 * SPLLDIV1 divide by 2=80MHz; SPLLDIV2 divide by 2=80MHz；
 * sys_clk=Core clock = 160/2 MHz = 80 MHz；bus clock = 40 MHz；flash clock= 26.6 MHz；
 * FIRC=48MHz；SIRC=8MHz；
 * 参数:
 * 参数解释:
 *
 * 返回值:
 *
 * 作 者:        ted, 2018 年 05 月 24 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void NormalRUNmode_80MHz (void) {  /* Change to normal RUN mode with 8MHz SOSC, 80 MHz PLL*/
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
}
/*==========================================================================
 * 函数名:HSRUNmode_112MHz
 * 功能描述:高速模式112MHz。SPLL_CLK=112MHz。
 * SPLLDIV1 divide by 2=56MHz; SPLLDIV2 divide by 2=56MHz；
 * sys_clk=Core clock = 112 MHz；bus clock = 56 MHz；flash clock= 28 MHz；
 * FIRC=48MHz；SIRC=8MHz；
 * 参数:
 * 参数解释:
 *
 * 返回值:
 *
 * 作 者:        ted, 2018 年 05 月 24 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void HSRUNmode_112MHz(void){
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000000;  /* SPLLEN=0: SPLL is disabled (default) */
  SCG->RCCR = SCG_RCCR_SCS(3)		 /* FIRC as clock source*/
		|SCG_RCCR_DIVCORE(0b00)      /* DIVCORE= 1, Core clock = 48 MHz*/
		|SCG_RCCR_DIVBUS(0b00)       /* DIVBUS = 1, bus clock = 48 MHz*/
		|SCG_RCCR_DIVSLOW(0b01);     /* DIVSLOW = 2, SCG slow, flash clock= 24 MHz*/
  SCG->HCCR = SCG_HCCR_SCS(3)		 /* FIRC as clock source*/
		|SCG_HCCR_DIVCORE(0b00)      /* DIVCORE= 1, Core clock = 48 MHz*/
		|SCG_HCCR_DIVBUS(0b00)       /* DIVBUS = 1, bus clock = 48 MHz*/
		|SCG_HCCR_DIVSLOW(0b01);     /* DIVSLOW = 2, SCG slow, flash clock= 24 MHz*/
  if ((SCG->CSR & SCG_CSR_SCS_MASK >> SCG_CSR_SCS_SHIFT ) != 3) {}
                                 /* Wait for sys clk src = FIRC */
  SMC->PMPROT = SMC_PMPROT_AHSRUN(1);
  SMC->PMCTRL = SMC_PMCTRL_RUNM(3);
  while(!(SMC->PMSTAT & SMC_PMSTAT_PMSTAT(0X80)));


  //SPLL_init_112MHz
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000000;  /* SPLLEN=0: SPLL is disabled (default) */
  SCG->SPLLDIV = 0x00000202;  /* SPLLDIV1 divide by 2; SPLLDIV2 divide by 2 */
  SCG->SPLLCFG = 0x000C0000;  /* PREDIV=0: Divide SOSC_CLK by 1+1=2 */
                              /* MULT=12:  Multiply sys pll by 16+12=28 */
                              /* SPLL_CLK = 8MHz /1* 28 / 2 = 112 MHz */
  while(SCG->SPLLCSR & SCG_SPLLCSR_LK_MASK); /* Ensure SPLLCSR unlocked */
  SCG->SPLLCSR = 0x00000001; /* LK=0:        SPLLCSR can be written */
                             /* SPLLCMRE=0:  SPLL CLK monitor IRQ if enabled */
                             /* SPLLCM=0:    SPLL CLK monitor disabled */
                             /* SPLLSTEN=0:  SPLL disabled in Stop modes */
                             /* SPLLEN=1:    Enable SPLL */
  while(!(SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK)); /* Wait for SPLL valid */

  //HSRUNmode_112MHz
  SCG->HCCR=SCG_HCCR_SCS(6)      /* PLL as clock source*/
    |SCG_HCCR_DIVCORE(0b00)      /* DIVCORE= 1, Core clock = 112 MHz*/
    |SCG_HCCR_DIVBUS(0b01)       /* DIVBUS = 2, bus clock = 56 MHz*/
    |SCG_HCCR_DIVSLOW(0b11);     /* DIVSLOW = 4, SCG slow, flash clock= 28 MHz*/
  if ((SCG->CSR & SCG_CSR_SCS_MASK >> SCG_CSR_SCS_SHIFT ) != 6) {}
                                 /* Wait for sys clk src = SPLL */
}

/*==========================================================================
 * 函数名:HSRUN_to_RUN
 * 功能描述:高速模式切换到RUN；
 * 参数:
 * 参数解释:
 *
 * 返回值:
 *
 * 作 者:        ted, 2018 年 05 月 24 日
 * 修改记录:
 *
 *===========================================================================
 */
APP_SECTION void HSRUN_to_RUN(void){

  SMC->PMCTRL = SMC_PMCTRL_RUNM(0);
  while(!(SMC->PMSTAT & SMC_PMSTAT_PMSTAT(0X80)));
  NormalRUNmode_80MHz();

}
