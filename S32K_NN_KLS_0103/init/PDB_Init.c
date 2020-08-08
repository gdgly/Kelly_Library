/*
 * PDB_Init.c
 *
 *  Created on: 2018Äê5ÔÂ26ÈÕ
 *      Author: Administrator
 */
#include "S32K142.h" /* include peripheral declarations S32K144 */
#include "typedefine.h"

/*******************************************************************************
Function Name : PDB_Init
Engineer      : Petr Stancik
Date          : Oct-2-2016
Parameters    : NONE
Modifies      : NONE
Returns       : NONE
Notes         : Initialize PDB for back-to-back mode
Issues        : NONE
*******************************************************************************/
APP_SECTION void Init_PDB(void)
{
  /*PCC->PCCn[PCC_PDB0_INDEX] |= PCC_PCCn_CGC_MASK;

  PDB0->SC |= PDB_SC_TRGSEL(0xF)  |
              PDB_SC_PRESCALER(0) |
              PDB_SC_MULT(0)      |
              PDB_SC_PDBEN_MASK;
  PDB0->CH[0].C1 = 0x00fe00ff;
  PDB0->CH[1].C1 = 0x00000000;

  PDB0->SC |= PDB_SC_LDOK_MASK;

  PDB0->SC |= PDB_SC_SWTRIG_MASK;
*/
	PCC->PCCn[PCC_PDB1_INDEX] |= PCC_PCCn_CGC_MASK;  /* Enable bus clock in PDB0 */

  PDB1->SC |= PDB_SC_TRGSEL(0xF)  |   /* b1111: Software trigger is selected */
              PDB_SC_PRESCALER(0) |   /* Prescaler:  010 = sysclck/(MULT) = 80 / (1) = 80MHz */
              PDB_SC_MULT(0)      |   /* 00: Multiplication factor is 1. */
              PDB_SC_PDBEN_MASK;      /* 1: PDB enabled */
  PDB1->CH[0].C1 = 0x00fe00ff;
  PDB1->CH[1].C1 = 0x00000000;

  PDB1->SC |= PDB_SC_LDOK_MASK;

  //PDB1->SC |= PDB_SC_SWTRIG_MASK;
}
