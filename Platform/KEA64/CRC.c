/*
 * CRC.c
 *
 *  Created on: Jan 15, 2020
 *      Author: FireSourcery
 */

#include "CRC1.h"

void CRC_Reset(void)
{
	CRC1_ResetCRC(CRC1_DeviceData);
}

void CRC_AddByte(uint8_t data)
{
	CRC1_GetCRC8(CRC1_DeviceData, data);
}

uint16_t CRC_GetResult(void)
{
	return (uint16_t)CRC1_GetCRCResult(CRC1_DeviceData);
}

void CRC_Init(void)
{
	(void)CRC1_Init(NULL);
}
