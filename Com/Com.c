/*
 * Com.c
 *
 *  Created on: Dec 20, 2019
 *      Author: FireSourcery
 */

//handle protocols //pass spi/uart/i2c drivers

//handing gui input/output
//handle arduino
//handle remote control

//spi/uart/i2c
//


//for multiple serial objects
typedef struct
{
	void (* RecvChar)(uint8_t * ch);
} COM_T;

void Com_RecvChar(COM_T * com, uint8_t * ch)
{
	com->RecvChar(ch);
}

void Com_InitSerial(COM_T * com, void (* recvChar)(uint8_t * ch))
{
	com->RecvChar = recvChar;
}

void UART1_RecvChar(uint8_t * ch)
{

}

