#ifndef BLINKY_H_
#define BLINKY_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	volatile bool IsOn;
	uint8_t Time;
	void (*On)(void);
	void (*Off)(void);
} BLINKY_T;


#endif
