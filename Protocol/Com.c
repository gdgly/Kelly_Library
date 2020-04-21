#include "Protocol/Bluefruit.h"

// protocols
// spi/uart/i2c drivers
// gui input/output
// arduino
// remote control

typedef enum
{
	PROTOCOL_UART_PASSTHROUGH_T,
	PROTOCOL_SHELL_T,
	PROTOCOL_BLUEFRUIT_T,
	PROTOCOL_REMOTE_T,
	PROTOCOL_ETS_T,
	PROTOCOL_TEST_T,
} PROTOCOL_T;

static PROTOCOL_T Protocol = PROTOCOL_BLUEFRUIT_T;



void Com_Process()
{
	switch (Protocol)
	{
		case PROTOCOL_UART_PASSTHROUGH_T:
			break;

		case PROTOCOL_BLUEFRUIT_T:
//			if (Bluefruit_Poll())
//			{
//				if (Bluefruit_ReadButtons().Button1) 		{}
//				if (Bluefruit_ReadButtons().Button2) 		{}
//				if (Bluefruit_ReadButtons().Button3) 		{}
//				if (Bluefruit_ReadButtons().Button4) 		{}
//				if (Bluefruit_ReadButtons().ButtonUp) 		{}
//				if (Bluefruit_ReadButtons().ButtonDown) 	{}
//				if (Bluefruit_ReadButtons().ButtonLeft) 	{}
//				if (Bluefruit_ReadButtons().ButtonRight) 	{}
//
//				Bluefruit_GetColorR();
//				Bluefruit_GetColorG();
//				Bluefruit_GetColorB();
//			}
//			break;

		case PROTOCOL_TEST_T:

			if (Test_RxPacket())
			{

			}

			Test_TxPacket( angleV,  motor_bemf,  motorCmdLim,  headroom);

			break;

		default:
			break;


	}

}

void Com_Init(void)
{

}


//if multiple connections
typedef struct
{
	PROTOCOL_T * Protocol;
	uint8_t * TxBuffer;
	uint8_t * RxBuffer;

	void (*BuildRxPacket)(void);
	void (*ParseRxPacket)(void);
	void (*TxPacket)(void);
	void (*RxPacket)(void);

	//void (* RecvChar)(uint8_t * ch);
} COM_T;

void Com_SetProtocol(COM_T * com, PROTOCOL_T * protocol)
{
	com->Protocol = protocol;
}

void Com_Tx(COM_T * com)
{


}

void Com_Rx(COM_T * com)
{


}



//
//void Com_RecvChar(COM_T * com, uint8_t * ch)
//{
//	com->RecvChar(ch);
//}
//
//void Com_InitSerial(COM_T * com, void (* recvChar)(uint8_t * ch))
//{
//	com->RecvChar = recvChar;
//}


//void Com_Process(COM_T * com)
//{
//
//	switch(com->Mode)
//	{
//
//	}
//}
//
//void Com_IDPacket(COM_T * com)
//{
//
//
//}
//
//
//void UART1_RecvChar(uint8_t * ch)
//{
//
//}
//
