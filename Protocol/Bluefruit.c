//Handles communication with the Adafruit Bluefruit Android/iOS App

#include "Bluefruit.h"

#include <stdint.h>
#include <stdbool.h>

#define RX_BUFFER_SIZE                    (20)
#define TX_BUFFER_SIZE                    (1)

static uint8_t * TxPacketBuffer;
static uint8_t * RxPacketBuffer;

static void (*RecvChar) (uint8_t * c);
static void (*SendChar) (uint8_t val);
static uint16_t (*GetCharsInRxBuffer) (void);

/******************************************************************************/
/*!
 * App Data
 */
/******************************************************************************/
bool UpdateFlag = 0; // Shared Update Flag, new data available
bool ColorUpdateFlag = 0;
bool ButtonUpdateFlag = 0;

uint8_t ColorR;
uint8_t ColorG;
uint8_t ColorB;

BUTTON_PAD_T ButtonStates;
//BUTTON_PAD_T ButtonChanged;

uint8_t BuildRxPacket(void)
{
	#define PACKET_ACC_LEN                  (15)
	#define PACKET_GYRO_LEN                 (15)
	#define PACKET_MAG_LEN                  (15)
	#define PACKET_QUAT_LEN                 (19)
	#define PACKET_BUTTON_LEN               (5)
	#define PACKET_COLOR_LEN                (6)
	#define PACKET_LOCATION_LEN             (15)
	#define BUILD_PACKET_TIME_OUT			500   // Timeout in ms waiting to read a response

	static uint16_t packetIndex = 0;
	static uint32_t startTime;
	//static bool packetComplete;

    // Check for completion
    if
	(
		(RxPacketBuffer[1] == 'B') && (packetIndex == PACKET_BUTTON_LEN) ||
    	(RxPacketBuffer[1] == 'C') && (packetIndex == PACKET_COLOR_LEN)
	)
    {
    	packetIndex = 0;
    	//  // check checksum!
    	//  uint8_t xsum = 0;
    	//  uint8_t checksum = packetbuffer[replyidx-1];
    	//
    	//  for (uint8_t i=0; i<replyidx-1; i++) {
    	//    xsum += packetbuffer[i];
    	//  }
    	//  xsum = ~xsum;
    	//
    	//  // Throw an error message if the checksum's don't match
    	//  if (xsum != checksum)
    	//  {
    	//    return 0;
    	//  }
    	return 1;
    }

    // Check for timeout
    if (Millis() - startTime < BUILD_PACKET_TIME_OUT)
    {
    	if  (GetCharsInRxBuffer())
    	{
			startTime = Millis();

			while (GetCharsInRxBuffer())
			{
				if (packetIndex < RX_BUFFER_SIZE)
				{
					RecvChar(&RxPacketBuffer[packetIndex]);
					if (RxPacketBuffer[packetIndex] == '!')
					{
						packetIndex = 0;
						RxPacketBuffer[packetIndex] = '!';
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


void ParseRxPacket()
{
	switch (RxPacketBuffer[1])
	{
		case 'C':
			//ColorUpdateFlag = 1;
			ColorR = RxPacketBuffer[2];
			ColorG = RxPacketBuffer[3];
			ColorB = RxPacketBuffer[4];
			break;

		case 'B':
			//ButtonUpdateFlag = 1;
			//    uint8_t buttnum = packetbuffer[2] - '0';
			//    boolean pressed = packetbuffer[3] - '0';
			if (RxPacketBuffer[3] - '0') // is pressed
				ButtonStates.Buttons |= 1 << ((RxPacketBuffer[2] - '0') - 1); // button num
			else
				ButtonStates.Buttons &= ~(1 << ((RxPacketBuffer[2] - '0') - 1));
			break;
	}

	RxPacketBuffer[1] = 0; //clear buffer

//  // GPS Location
//  if (packetbuffer[1] == 'L') {
//    float lat, lon, alt;
//    lat = parsefloat(packetbuffer+2);
//    lon = parsefloat(packetbuffer+6);
//    alt = parsefloat(packetbuffer+10);
//  }
//
//  // Accelerometer
//  if (packetbuffer[1] == 'A') {
//    float x, y, z;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//
//  }
//
//  // Magnetometer
//  if (packetbuffer[1] == 'M') {
//    float x, y, z;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//
//  }
//
//  // Gyroscope
//  if (packetbuffer[1] == 'G') {
//    float x, y, z;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//
//  }
//
//  // Quaternions
//  if (packetbuffer[1] == 'Q') {
//    float x, y, z, w;
//    x = parsefloat(packetbuffer+2);
//    y = parsefloat(packetbuffer+6);
//    z = parsefloat(packetbuffer+10);
//    w = parsefloat(packetbuffer+14);
//  }
}

/**************************************************************************/
/*!
    @brief  Poll regularly for new data
*/
/**************************************************************************/
bool Bluefruit_Poll(void)
{
	if (BuildRxPacket())
	{
		UpdateFlag = true;
		ParseRxPacket();
		return true;
	}
	return false;
}



uint8_t Bluefruit_GetColorR() 		{return ColorR;}
uint8_t Bluefruit_GetColorG() 		{return ColorG;}
uint8_t Bluefruit_GetColorB() 		{return ColorB;}

BUTTON_PAD_T Bluefruit_GetButtons()		{return ButtonStates;}
BUTTON_PAD_T Bluefruit_ReadButtons()	{UpdateFlag = 0; return ButtonStates;}

//char Bluefruit_GetPacketType() 	{return Packet[1];}

bool Bluefruit_GetUpdateFlag()		{return UpdateFlag;}
void Bluefruit_ResetUpdateFlag()	{UpdateFlag = 0;}

//bool Bluefruit_GetColorUpdateFlag()		{return ColorUpdateFlag;}
//void Bluefruit_ResetColorUpdateFlag()		{ColorUpdateFlag = 0;}

//bool Bluefruit_ReadButton1()		{UpdateFlag = 0; return ButtonStates.Button1;}
//bool Bluefruit_ReadButton2()		{UpdateFlag = 0; return ButtonStates.Button2;}
//bool Bluefruit_ReadButton3()		{UpdateFlag = 0; return ButtonStates.Button3;}
//bool Bluefruit_ReadtButton4()		{UpdateFlag = 0; return ButtonStates.Button4;}
//bool Bluefruit_ReadtButtonUp()		{UpdateFlag = 0; return ButtonStates.ButtonUp;}
//bool Bluefruit_ReadButtonDown()		{UpdateFlag = 0; return ButtonStates.ButtonDown;}
//bool Bluefruit_ReadButtonLeft()		{UpdateFlag = 0; return ButtonStates.ButtonLeft;}
//bool Bluefruit_ReadButtonRight()	{UpdateFlag = 0; return ButtonStates.ButtonRight;}
//
//bool Bluefruit_GetButton1()			{return ButtonStates.Button1;}
//bool Bluefruit_GetButton2()			{return ButtonStates.Button2;}
//bool Bluefruit_GetButton3()			{return ButtonStates.Button3;}
//bool Bluefruit_GetButton4()			{return ButtonStates.Button4;}
//bool Bluefruit_GetButtonUp()		{return ButtonStates.ButtonUp;}
//bool Bluefruit_GetButtonDown()		{return ButtonStates.ButtonDown;}
//bool Bluefruit_GetButtonLeft()		{return ButtonStates.ButtonLeft;}
//bool Bluefruit_GetButtonRight()		{return ButtonStates.ButtonRight;}

//// ----------------------------------------------------------------------------------------------
//// COMMON SETTINGS
//// ----------------------------------------------------------------------------------------------
////#define BUFSIZE                        256   // Size of the read buffer for incoming data
//#define FACTORYRESET_ENABLE				0
//#define VERBOSE_MODE                   true  // If set to 'true' enables debug output
//
//Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);
//// ----------------------------------------------------------------------------------------------
//// HARDWARE UART SETTINGS
//// ----------------------------------------------------------------------------------------------
//#define BLUEFRUIT_HWSERIAL_NAME      Serial1
//#define BLUEFRUIT_UART_MODE_PIN        6    // Set to -1 if unused
//#define BLUEFRUIT_UART_CTS_PIN         11   // Required for software serial!
//#define BLUEFRUIT_UART_RTS_PIN         -1   // Optional, set to -1 if unused
//
//
//void Bluefruit_Restart(void)
//{
//	ble.begin(0, 0);
//}
//
///**************************************************************************/
///*!
//    @brief  Sets up the HW an the BLE module (this function is called
//            automatically on startup)
//*/
///**************************************************************************/
//void Bluefruit_Init(void)
//{
//  /* Initialise the module */
//  Serial.print(F("Initialising the Bluefruit LE module: "));
//
//  if ( !ble.begin(VERBOSE_MODE, 0) )
//  {
//    //error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
//	  Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
//  }
//  else
//  {
//	  Serial.println( F("OK!") );
//
//	  if ( FACTORYRESET_ENABLE )
//	  {
//		/* Perform a factory reset to make sure everything is in a known state */
//		Serial.println(F("Performing a factory reset: "));
//		if ( ! ble.factoryReset() ){
//		  error(F("Couldn't factory reset"));
//		}
//	  }
//
//	  /* Disable command echo from Bluefruit */
//	  ble.echo(false);
//
//	  Serial.println("Requesting Bluefruit info:");
//	  /* Print Bluefruit information */
//	  ble.info();
//
//	  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
//	  Serial.println();
//
//	  //ble.verbose(false);  // debug info is a little annoying after this point!
//
//	  Serial.println(F("***********************"));
//
//	  // Set Bluefruit to DATA mode
//	  Serial.println( F("Switching to DATA mode!") );
//	  ble.setMode(BLUEFRUIT_MODE_DATA);
//
//	  Serial.println(F("***********************"));
//  }
//}
//
//
