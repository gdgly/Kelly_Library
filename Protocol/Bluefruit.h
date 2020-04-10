#ifndef _PROTOCOL_BLUEFRUIT_H_
#define _PROTOCOL_BLUEFRUIT_H_

#include <stdint.h>
//#include <stdbool.h>

typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
} BLUEFRUIT_COLOR_T;

typedef struct
{
	union
	{
		struct
		{
			uint8_t Button1		: 1;
			uint8_t Button2		: 1;
			uint8_t Button3		: 1;
			uint8_t Button4		: 1;
			uint8_t ButtonUp 	: 1;
			uint8_t ButtonDown 	: 1;
			uint8_t ButtonLeft	: 1;
			uint8_t ButtonRight	: 1;
		};
		uint8_t Buttons;
	};
} BUTTON_PAD_T;

BUTTON_PAD_T Bluefruit_GetButtons();
BUTTON_PAD_T Bluefruit_ReadButtons();

#endif
