#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#define TX_BUFFER_SIZE                    (18)
#define RX_BUFFER_SIZE                    20//(6)

#define PACKET_RX_START_CHAR	'\0'
#define PACKET_RX_END_CHAR		'\n'
#define PACKET_RX_TIMEOUT		500
#define PACKET_TX_INTERVAL		20

//Inherit from Protocol Later
static uint8_t * RxPacketBuffer;
static uint8_t * TxPacketBuffer;

static void (*RecvChar) (uint8_t * c);
static void (*SendChar) (uint8_t val);
static uint16_t (*GetCharsInRxBuffer) (void);

//Protocol config, const fro this module
//uint8_t PacketStartChar;
//uint8_t PacketEndChar;
//uint8_t PacketTimeOut;


//one set of data is fine. only 1 connection using protocol.

//data communicated, data in
static float Kp, Kd, Kbemf, Ku, Ki, DirectV, DesiredV;
static bool first_time = false;
//static float * p_Kp;
//static void (* OnRecvKp)(float Kp)

//data out
float * p_TxAngleV, p_TxBemf;
uint32_t * p_TxCmdLim, p_TxHeadroom;


const char * config_message_30_bytes = "&A~DesiredAngV~5&C&S~Direct~O~0~5.0~0.1&S~DesiredAngV~A~-1~1~0.1&T~AngleV~F4~-2.5~2.5&T~BackEMF~F4~0~5&T~MCmd~F4~0~5&H~4&";

//typedef struct
//{
//	PROTOCOL_T ProtocolCommon;
//	//data communicated
//	static float Kp, Kd, Kbemf, Ku, Ki, DirectV, DesiredV;
//	static bool first_time;
//	//static float * p_Kp;
//	//static void (* OnRecvKp)(float Kp)
//
//	float * p_TxAngleV, p_TxBemf;
//	uint32_t * p_TxCmdLim, p_TxHeadroom;
//
//} TEST_PROTCOL_T;


//typedef struct
//{
//	uint8_t KpUpdated	:1;
//	uint8_t KiUpdated	:1;
//	uint8_t KdUpdated	:1;
//} TEST_VAR_UPATED;

void SendStartMessage(void)
{
	uint8_t i = 0;

	while(config_message_30_bytes[i] != 0)
	{
		SendChar(config_message_30_bytes[i]);
		i++;
	}
}

static uint8_t BuildRxPacket(void)
{
	static uint16_t packetIndex = 0;

    if (RxPacketBuffer[packetIndex] == '\n')
    {
    	packetIndex = 0;
    	return 1;
    }

	while (GetCharsInRxBuffer())
	{
		if (packetIndex < RX_BUFFER_SIZE)
		{
			RecvChar(&RxPacketBuffer[packetIndex]);
			printf("packetIndex is %d  ", packetIndex);
			printf("RxPacketValue is %d \n", RxPacketBuffer[packetIndex]);

			if (RxPacketBuffer[packetIndex] == '\n') break;
			packetIndex++;
		}
		else
		{
			packetIndex = 0;
		}
	}

	return 0;
}

static void ParseRxPacket(void)
{

	switch (RxPacketBuffer[0])
	{
	case 'P':
		Kp = *(float*)&RxPacketBuffer[1];
		//KpEvent(Kp);
		//*p_Kp = *(float*)&RxPacketBuffer[1];
		break;
	case 'D':
		Kd = *(float*)&RxPacketBuffer[1];
		break;
	case 'E':
		Kbemf = *(float*)&RxPacketBuffer[1];
		break;
	case 'U':
		Ku = *(float*)&RxPacketBuffer[1];
		break;
	case 'I':
		Ki = *(float*)&RxPacketBuffer[1];
		break;
	case 'O':
		DirectV = *(float*)&RxPacketBuffer[1];
		break;
	case 'A':
		DesiredV = *(float*)&RxPacketBuffer[1];
		break;
	case '~':
		first_time = true;

//		  p_LoopCounter = 0;
//		  p_Headroom = deltaT;

		break;
	default:
		break;
	}

	RxPacketBuffer[5] = 0;

	//Test_Event();
}

bool Test_RxPacket(void)
{
	if(BuildRxPacket())
	{
		ParseRxPacket();
		return true;
	}
	return false;
}

bool StartUpComplete = false;

void Test_TxPacketValue(float angleV, float motor_bemf, float motorCmdLim, float headroom)
{
	uint8_t i;


	if (StartUpComplete)
	{
		TxPacketBuffer[0] = 0;

		memcpy(&TxPacketBuffer[1], 	&angleV, 		sizeof(angleV));
		memcpy(&TxPacketBuffer[5], 	&motor_bemf, 	sizeof(motor_bemf));
		memcpy(&TxPacketBuffer[9], 	&motorCmdLim, 	sizeof(motorCmdLim));
		memcpy(&TxPacketBuffer[13],	&headroom, 		sizeof(headroom));

		TxPacketBuffer[17] = 255;

		for (i = 0; i < 18; i++)
		{
			SendChar(TxPacketBuffer[i]);
			printf("TxPacketValue is %d \n", TxPacketBuffer[i]);
			//printf("TxPacketValue is %f \n", *(float*)&TxPacketBuffer[i]);
		}
	}
}


void Test_StartUp(void)
{
	if (first_time)
	{

		while (GetCharsInRxBuffer()) RecvChar(&RxPacketBuffer[0]);

		SendStartMessage();

		while (!GetCharsInRxBuffer());

		while (GetCharsInRxBuffer())
		{
			RecvChar(&RxPacketBuffer[0]);
		}
		first_time = false;

		StartUpComplete = true;
	}

}



void Test_TxPacket(void)
{
	uint8_t i;

	TxPacketBuffer[0] = 0;

	memcpy(&TxPacketBuffer[1], 	p_TxAngleV, 	4);
	memcpy(&TxPacketBuffer[5], 	p_TxBemf, 		4);
	memcpy(&TxPacketBuffer[9], 	p_TxCmdLim, 	4);
	memcpy(&TxPacketBuffer[13],	p_TxHeadroom, 	4);

	TxPacketBuffer[17] = 255;

	for (i = 0; i < 18; i++)
		SendChar(TxPacketBuffer[i]);
}

float Test_GetKp() 			{return Kp;}
float Test_GetKi() 			{return Ki;}
float Test_GetKd() 			{return Kd;}
float Test_GetKbemf() 		{return Kbemf;}
float Test_GetKu() 			{return Ku;}
float Test_GetDirectV() 	{return DirectV;}
float Test_GetDesiredV() 	{return DesiredV;}


//void Test_InitProtocol
//(
//		PROTOCOL_T * protocol,
////	uint8_t * txBuffer,
////	uint8_t * rxBuffer,
////	uint16_t (*getCharInRxBuffer)(void),
////	void (*recvChar)(uint8_t * dest),
////	void (*sendChar)(uint8_t ch),
//
//		//uint8_t PacketStartChar;
//		//uint8_t PacketEndChar;
//		//uint8_t PacketTimeOut;
//	const float * txAngleV, float * txBEMF, float * txDesired, float * txHeadroom,
//	float * rxKp, float * rxKd, float * rxKbemf, float * rxKu, float * rxKi, float * rxDirectV, float * rxDesiredV
//
//)
//{
//#define PACKET_RX_START_CHAR	'\0'
//#define PACKET_RX_END_CHAR		'\n'
//#define PACKET_RX_TIMEOUT		500
//#define PACKET_TX_INTERVAL		20
//
//	//Protocol_Init();
//	protocol->TxPacket = Test_TxPacket;
//	protocol->RxPacket = Test_RxPacket;
//	protocol->PacketStartChar = PACKET_RX_START_CHAR;
//	protocol->PacketEndChar = PACKET_RX_END_CHAR;
//	protocol->PacketTimeOut = PACKET_RX_TIMEOUT;
//
//	//Protocol config, const fro this module
//
//	RxPacketBuffer = rxBuffer;
//	TxPacketBuffer = txBuffer;
//	RecvChar = recvChar;
//	SendChar = sendChar;
//	GetCharsInRxBuffer = getCharInRxBuffer;
//}


void Test_Init
(
	uint8_t * txBuffer,
	uint8_t * rxBuffer,
	uint16_t (*getCharInRxBuffer)(void),
	void (*recvChar)(uint8_t * dest),
	void (*sendChar)(uint8_t ch)
)
{
	RxPacketBuffer = rxBuffer;
	TxPacketBuffer = txBuffer;
	RecvChar = recvChar;
	SendChar = sendChar;
	GetCharsInRxBuffer = getCharInRxBuffer;
}

//void Test_InitVars
//(
//	float * txAngleV, float * txBEMF, float * txDesired, float * txHeadroom,
//	float * rxKp, float * rxKd, float * rxKbemf, float * rxKu, float * rxKi, float * rxDirectV, float * rxDesiredV
//)
//{
//
//}

