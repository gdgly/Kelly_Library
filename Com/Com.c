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

typedef enum
{
	COM_MODE_UART_PASSTHROUGH_T,
	COM_MODE_SHELL_T,

}COM_MODE_T;

//for multiple serial objects
typedef struct
{
	COM_MODE_T Mode;
	uint8_t * Buffer;
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




void Com_Process(COM_T * com)
{

	switch(com->Mode)
	{
		case COM_MODE_SHELL_T:

		case REMOTE_T:
	}

}



void Com_IDPacket(COM_T * com)
{


}


void UART1_RecvChar(uint8_t * ch)
{

}

