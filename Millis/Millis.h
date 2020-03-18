/*
 * Millis.h
 *
 *  Created on: Nov 30, 2019
 *      Author: FireSourcery
 */

#ifndef MILLIS_H_
#define MILLIS_H_

#define SYSTICK_COUNT() (SYST_CVR)

uint32_t Millis(void);
void Delay(uint32_t ms);

void Millis_Init(uint32_t fCPU, uint8_t priroity);

#endif /* MILLIS_H_ */
