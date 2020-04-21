/*
 * Blinky.c
 *
 *  Created on: Dec 22, 2019
 *      Author: FireSourcery
 */
#include "Blinky.h"

void Blinky_Toggle(BLINKY_T * blinky)
{
	if (blinky->IsOn)
	{
		blinky->IsOn = 0;
		blinky->Off();
	}
	else
	{
		blinky->IsOn = 1;
		blinky->On();
	}
}
/*
 * @brief toggle blink switch
 *
 * @param[in] blinky- original blink value
 */
void Blinky_Init(BLINKY_T * blinky, void (*on)(void), void (*off)(void))
{
	blinky->Off = off;
	blinky->On = on;
}
/*
 * @brief intialize blink switch
 *
 * @param[in] (*on)- physical switch status
 * @param[in] (*off)- physical switch status
 */
