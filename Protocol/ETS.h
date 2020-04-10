/*
 * ETS.h
 *
 *  Created on: Dec 5, 2019
 *      Author: FireSourcery
 */

#ifndef ETS_H_
#define ETS_H_

typedef struct
{
	union
	{
		uint8_t * Index[2];
	};
	uint8_t * hallAB;
	uint16_t * RPM;
	
}
ETS_MONITOR_T;


void ETX_TxNextData(void)
{

	static i = 0;
	
	//send (*Index[i])
	i++;
	
}

#endif /* ETS_H_ */
