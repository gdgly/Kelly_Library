/*
 * ADC_Init.c
 *
 *  Created on: 2018年5月23日
 *      Author: Administrator
 */
#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "typedefine.h"



APP_SECTION void Init_ADC_SwTrig (void)
{

	SIM->CHIPCTL &= ~(uint32_t)(SIM_CHIPCTL_ADC_INTERLEAVE_EN_MASK /*Disable ADC interleave channel enable*/
  								| SIM_CHIPCTL_ADC_SUPPLY_MASK									 /*5 V input VDD supply (VDD)*/
									| SIM_CHIPCTL_ADC_SUPPLYEN_MASK);              /* Disable internal supply monitoring*/

	//ADC0使用软件触发
	PCC->PCCn[PCC_ADC0_INDEX] &= ~ PCC_PCCn_CGC_MASK; /* Disable clock to change PCS */
	PCC->PCCn[PCC_ADC0_INDEX] = PCC_PCCn_PCS(6); /* PCS=3: Select SPLL 80MHz*/
	PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to ADC0 regs */

  ADC0->CFG1 = ADC_CFG1_ADICLK(0) |     /* Only ALTCLK1 is available */
		  	  				ADC_CFG1_ADIV(0) |		 /* the clock rate is (input clock)/1=80MHz */
									ADC_CFG1_MODE(2);        /* 0b00: 8-bit, 0b01: 12-bit, 0b10: 10-bit */
	ADC0->CFG2 = 0x00000000C; /* SMPLTS=12(default): sample time is 13 ADC clks */
  ADC0->SC2  = ADC_SC2_ADTRG(0);         /* Conversion Trigger:  0b0= SW , 0b1 = HW */
  ADC0->SC3  = 0x00000000;         /* CAL=0: Do not start calibration sequence */
                                  /* ADCO=0: One conversion performed */
                                  /* AVGE,AVGS=0: HW average function disabled */
  //ADC1使用软件触发
	PCC->PCCn[PCC_ADC1_INDEX] &=~ PCC_PCCn_CGC_MASK;  /* Disable clock to change PCS */
	PCC->PCCn[PCC_ADC1_INDEX] = PCC_PCCn_PCS(6);    /* PCS=3: Select SPLL 80MHz*/
  PCC->PCCn[PCC_ADC1_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to ADC0 regs */

  ADC1->CFG1 |= ADC_CFG1_ADICLK(0) |     /* Only ALTCLK1 is available */
		  	  				ADC_CFG1_ADIV(0) |		 /* the clock rate is (input clock)/1=80MHz */
									ADC_CFG1_MODE(2);        /* 0b00: 8-bit, 0b01: 12-bit, 0b10: 10-bit */
  ADC1->CFG2 = 0x00000000C;       /* SMPLTS=12(default): sample time is 13 ADC clks */
  ADC1->SC2  = ADC_SC2_ADTRG(0);         /* Conversion Trigger:  0b0= SW , 0b1 = HW */
  ADC1->SC3  = 0x00000000;         /* CAL=0: Do not start calibration sequence */
                                  /* ADCO=0: One conversion performed */
                                  /* AVGE,AVGS=0: HW average function disabled */
}

APP_SECTION void Init_ADC0_HwTrig (void)
{
	int i=0;
	ADC0->SC2  = ADC_SC2_ADTRG(1);         /* Conversion Trigger:  0b0= SW , 0b1 = HW */
  for(i=0;i<8;i++)
	  ADC0->SC1[i] = ADC_SC1_ADCH(i);
}
APP_SECTION void Init_ADC1_HwTrig (void)
{
	int i=0;
	ADC1->SC2  = ADC_SC2_ADTRG(1);         /* Conversion Trigger:  0b0= SW , 0b1 = HW */
  for(i=0;i<8;i++)
	  ADC1->SC1[i] = ADC_SC1_ADCH(i);
}
APP_SECTION uint32_t ADC0_SwTrigConvert (uint32_t Ch) {
	ADC0->SC1[0] &= ~ADC_SC1_ADCH_MASK;	      /* Clear prior ADCH bits */
	ADC0->SC1[0] = ADC_SC1_ADCH(Ch);
	while((ADC0->SC1[0] & ADC_SC1_COCO_MASK)==0){}
	return (ADC0->R[0]);

}
APP_SECTION uint32_t ADC1_SwTrigConvert (uint32_t Ch) {
	ADC1->SC1[0] &= ~ADC_SC1_ADCH_MASK;	      /* Clear prior ADCH bits */
	ADC1->SC1[0] = ADC_SC1_ADCH(Ch);
	while((ADC1->SC1[0] & ADC_SC1_COCO_MASK)==0){}
	return (ADC1->R[0]);

}
