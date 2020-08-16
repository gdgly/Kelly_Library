#include "FTM1.h"
  /* Including shared modules, which are used in the whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Cpu.h"



/*
 *              Clock source                               : System clock
 *              Prescaler                                  : divide by 64
 *              Modulo counter                             : 65535
 *              Period                                     : 209.715 ms
 *          Channels                                       :
 *            Channel 0                                    : Disabled
 *            Channel 1                                    : Enabled
 *              Channel mode                               : Input capture
 *                Edge control                             : Rising edge
 *                Pin                                      : PTC5/FTM1_CH1/RTCO
 */
void FTM1_Init(void)
{
	/* NVIC_ISER: SETENA|=0x000C0000 */
	NVIC_ISER |= NVIC_ISER_SETENA(0x000C0000);
	/* NVIC_IPR4: PRI_19=0x40,PRI_18=0x80 */
	NVIC_IPR4 = (uint32_t)((NVIC_IPR4 & (uint32_t)~(uint32_t)(
			NVIC_IP_PRI_18(0x7F)
	)) | (uint32_t)(
			NVIC_IP_PRI_18(0x80)
	));

	/* SIM_SCGC: FTM1=1 */
	SIM_SCGC |= SIM_SCGC_FTM1_MASK;
	(void) (FTM1_SC == 0U); /* Dummy read of the FTM1_SC register to clear the interrupt flag */
	/* FTM1_SC: TOF=0,TOIE=0,CPWMS=0,CLKS=0,PS=0 */
	FTM1_SC = (FTM_SC_CLKS(0x00) | FTM_SC_PS(0x00)); /* Stop the counter */

	(void) (FTM1_C1SC == 0U); /* Dummy read of the FTM1_C1SC register to clear the interrupt flag */

	/* FTM1_C0SC: CHF=0,CHIE=0,MSB=0,MSA=0,ELSB=0,ELSA=0 */
	FTM1_C0SC = 0x00U;
	/* FTM1_C1SC: CHF=0,CHIE=0,MSB=0,MSA=0,ELSB=0,ELSA=0 */
	FTM1_C1SC = 0x00U;

	/* FTM1_SC: TOF=0,CPWMS=0 */
	FTM1_SC &= (uint32_t) ~(uint32_t) ((FTM_SC_TOF_MASK | FTM_SC_CPWMS_MASK));

	/* FTM1_C0SC: CHF=0,MSB=0,MSA=0,ELSB=0,ELSA=0 */
	FTM1_C0SC &= (uint32_t)~(uint32_t)(
			FTM_CnSC_CHF_MASK |
			FTM_CnSC_MSB_MASK |
			FTM_CnSC_MSA_MASK |
			FTM_CnSC_ELSB_MASK |
			FTM_CnSC_ELSA_MASK |
			0xFFFFFF02U
	);

	/* FTM1_C1SC: CHF=0,CHIE=1,MSB=0,MSA=0,ELSB=0,ELSA=1 */
	FTM1_C1SC = (uint32_t)((FTM1_C1SC & (uint32_t)~(uint32_t)(
			FTM_CnSC_CHF_MASK |
			FTM_CnSC_MSB_MASK |
			FTM_CnSC_MSA_MASK |
			FTM_CnSC_ELSB_MASK |
			0xFFFFFF02U
	)) | (uint32_t)(
			FTM_CnSC_CHIE_MASK |
			FTM_CnSC_ELSA_MASK
	));
	/* FTM1_C1V: */
	FTM1_C1V &= (uint32_t)~(uint32_t)(0xFFFF0000U);
	/* FTM1_MOD: MOD=0xFFFF */
	FTM1_MOD |= FTM_MOD_MOD(0xFFFF);
	/* FTM1_CNT: COUNT=0 */
	FTM1_CNT &= (uint32_t)~(uint32_t)(FTM_CNT_COUNT(0xFFFF));
	/* FTM1_SC: TOF=0,TOIE=0,CPWMS=0,CLKS=1,PS=6 */
	FTM1_SC = (uint32_t)((FTM1_SC & (uint32_t)~(uint32_t)(
			FTM_SC_TOF_MASK |
			FTM_SC_TOIE_MASK |
			FTM_SC_CPWMS_MASK |
			FTM_SC_CLKS(0x02) |
			FTM_SC_PS(0x01)
	)) | (uint32_t)(
			FTM_SC_CLKS(0x01) |
			FTM_SC_PS(0x06)
	));

}

