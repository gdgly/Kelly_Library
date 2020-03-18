/*
 * Serial.h
 *
 *  Created on: Jan 15, 2020
 *      Author: FireSourcery
 */

#ifndef KELLY_LIBRARY_KEA64_SERIAL_H_
#define KELLY_LIBRARY_KEA64_SERIAL_H_

#include <stdint.h>

extern void Serial_RecvChar(uint8_t * ch);
extern void Serial_SendChar(uint8_t ch);
extern void Serial_ClearRxBuffer(void);
extern uint16_t Serial_GetCharsInRxBuffer(void);


#endif /* KELLY_LIBRARY_KEA64_SERIAL_H_ */
