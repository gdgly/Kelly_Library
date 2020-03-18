/*
 * Blinky.h
 *
 *  Created on: Dec 22, 2019
 *      Author: FireSourcery
 */

#ifndef BLINKY_H_
#define BLINKY_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	bool IsOn;
	uint8_t Time;
	void (*On)(void);
	void (*Off)(void);
} BLINKY_T;


#endif /* UTILITY_BLINKY_BLINKY_H_ */
