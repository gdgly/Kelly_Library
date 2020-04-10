#include <stdint.h>
#include <stdbool.h>

#define NO_START_CHAR '\n' //uses \n as no start char. this way start char of \0 is supported. protocols using \n as start char is unsupported

typedef struct
{
	//hardware driver uart/spi
	void (*RecvChar)(uint8_t * c);
	void (*SendChar)(uint8_t val);
	uint16_t (*GetCharsInRxBuffer)(void);

	uint8_t * RxPacketBuffer;
	uint8_t * TxPacketBuffer;

	//Protocol config, inherit
	uint8_t PacketStartChar;
	uint8_t PacketEndChar;
	uint8_t PacketTimeOut;

	void (*ParsePacket)(void);
	void (*ParsePacket)(void);

} PROTOCOL_T;



void ParsePacket(PROTOCOL_T * prot)
{
	prot->ParsePacket();
}


static uint8_t BuildRxPacket(PROTOCOL_T * prot)
{
	static uint16_t packetIndex = 0;

    if (prot->PacketComplete())
    {
    	packetIndex = 0;
    	return 1;
    }

    // Check for timeout
    if (Millis() - startTime < prot->BUILD_PACKET_TIME_OUT)
    {
    	if  (GetCharsInRxBuffer())
    	{
			startTime = Millis();

			while (GetCharsInRxBuffer())
			{
				if (packetIndex < RX_BUFFER_SIZE)
				{
					RecvChar(&RxPacketBuffer[packetIndex]);

					if (RxPacketBuffer[packetIndex] == prot->PacketStartChar)
					{
						packetIndex = 0;
						RxPacketBuffer[packetIndex] = PacketStartChar;
					}
					else if (RxPacketBuffer[packetIndex] == PacketEndChar)
					{
						break;
					}

					packetIndex++;
				}
				else
				{
					packetIndex = 0;
				}
			}
    	}
	}
	else //start new packet if timed out
	{
		packetIndex = 0;
		startTime = Millis();
	}

	return 0;
}


static bool Protocol_RxPacket(PROTOCOL_T * prot)
{
	if (Protocol_BuildRxPacket(prot))
	{
		//UpdateFlag = true;
		prot->ParseRxPacket();
		return true;
	}
	return false;
}

static void Protocol_TxPacket(PROTOCOL_T * prot)
{

}
//bool Protocol_CheckWithTimeOut(void) //blocking
//{
//	char charBuffer;
//	uint32_t startTime = Millis();
//	uint32_t timeOutCounter = 0;
//
//	while (Millis() - startTime < FC_START_UP_TIME_OUT + 1)
//	{
//		Serial_SendChar(FC_CMD_ACK);
//		timeOutCounter += FC_START_UP_TIME_OUT_PER_ACK;
//
//		while (Millis() - startTime < timeOutCounter) // wait 250ms for Rx Char
//		{
//			if (Serial_GetCharsInRxBuf())
//			{
//
//				Serial_RecvChar(&charBuffer);
//					fp(charBuffer);
//			}
//		}
//	}
//	return false;
//}
