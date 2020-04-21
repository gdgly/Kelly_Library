/*
 * Wrapper for Serial LDD. Can fully implement later.
 */

#include "Serial.h"

#include "AS1.h"

void Serial_Init(void)
{
	AS1_Init();
}


void Serial_RecvChar(uint8_t * ch)
{
	AS1_RecvChar(ch);
}

uint8_t Serial_GetChar(void)
{
	uint8_t ch;
	AS1_RecvChar(&ch);
	return ch;
}

void Serial_SendChar(uint8_t ch)
{
	AS1_SendChar(ch);
}

void Serial_ClearRxBuffer(void)
{
	AS1_ClearRxBuf();
}

uint16_t Serial_GetCharsInRxBuffer(void)
{
	return AS1_GetCharsInRxBuf();
}
