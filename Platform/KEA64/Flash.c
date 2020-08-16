/*
 * Flash.c
 *
 *  Created on: Jan 15, 2020
 *      Author: FireSourcery
 */
#include "IFsh1.h"



uint8_t Flash_EraseSector(uint8_t * targetAddress) //returns error
{
	IFsh1_EraseSector(targetAddress);
}

void Flash_Init(void)
{
	IFsh1_Init();
}

uint8_t Flash_WriteBlock(uint8_t * targetAddress, uint8_t * sourceAddress, uint16_t sizeBytes) //return error code
{
	return IFsh1_SetBlockFlash(sourceAddress, targetAddress, sizeBytes);
}

