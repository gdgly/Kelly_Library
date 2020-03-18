//Handles communication with the Adafruit Bluefruit Android/iOS App
extern uint16_t Bluefruit_GetCharsInRxBuffer();
extern uint8_t Bluefruit_RecvChar(char *);

#define RX_BUFFER_SIZE                    (20)
uint8_t PacketBuffer[READ_BUFSIZE+1];

// ----------------------------------------------------------------------------------------------
// App Data
// ----------------------------------------------------------------------------------------------
bool UpdateFlag = 0; // Shared Update Flag //new data avaible
bool ColorUpdateFlag = 0;
bool ButtonUpdateFlag = 0;

uint8_t ColorR;
uint8_t ColorG;
uint8_t ColorB;

typedef struct
{
	uint8_t Button1		: 1;
	uint8_t Button2		: 1;
	uint8_t Button3		: 1;
	uint8_t Button4		: 1;
	uint8_t ButtonUp 	: 1;
	uint8_t ButtonDown 	: 1;
	uint8_t ButtonLeft	: 1;
	uint8_t ButtonRight	: 1;
} BUTTON_PAD_T;

BUTTON_PAD_T ButtonStates;
//BUTTON_PAD_T ButtonChanged;

#define PACKET_ACC_LEN                  (15)
#define PACKET_GYRO_LEN                 (15)
#define PACKET_MAG_LEN                  (15)
#define PACKET_QUAT_LEN                 (19)
#define PACKET_BUTTON_LEN               (5)
#define PACKET_COLOR_LEN                (6)
#define PACKET_LOCATION_LEN             (15)
#define BUILD_PACKET_TIME_OUT			500   // Timeout in ms waiting to read a response

uint8_t Bluefruit_RxPacket(void)
{
	static uint16_t packetIndex = 0;
	static bool packetComplete;
	static uint32_t startTime;// = Millis();

	if (packetComplete)
	{
		packetComplete = 0;
		packetIndex = 0;
		startTime = Millis();
	}
	else if (Millis() - startTime > BUILD_PACKET_TIME_OUT) //start new packet if timed out
	{
		packetIndex = 0;
		return 0;
	}

    if
	(
		(PacketBuffer[1] == 'B') && (replyidx == PACKET_BUTTON_LEN) ||
    	(PacketBuffer[1] == 'C') && (replyidx == PACKET_COLOR_LEN)
	)
    {
    	packetComplete = 1;
    	return 1;
    }

	while (Bluefruit_GetCharsInRxBuffer())
	{
		if (packetIndex < RX_BUFFER_SIZE)
		{
			Bluefruit_RecvChar(&Packet[packetIndex]);
			if (Packet[packetIndex] == '!')
			{
				packetIndex = 0;
				Packet[packetIndex] = '!';
			}
			packetIndex++;
		}
		else
		{
			packetIndex = 0;
			return 0;
		}
		startTime = Millis();
	}


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

  return 0;
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void Bluefruit_ParsePacket()
{
//	/* Wait for new data to arrive */
//	uint8_t len = buildPacket(&ble, BLE_READPACKET_TIMEOUT);
//	if (len == 0)
//		return;
	//UpdateFlag = 1;

	switch (packetbuffer[1])
	{
	case 'C':
		ColorUpdateFlag = 1;
		ColorR = packetbuffer[2];
		ColorG = packetbuffer[3];
		ColorB = packetbuffer[4];
		break;

	case 'B':
		ButtonUpdateFlag = 1;
		if (packetbuffer[3] - '0')
			ButtonStates |= 1 << ((packetbuffer[2] - '0') - 1);
		else
			ButtonStates &= ~(1 << ((packetbuffer[2] - '0') - 1));
		break;

	}

  //  // Color
//  if (packetbuffer[1] == 'C')
//  {
//	ColorUpdateFlag = 1;
//	ColorR = packetbuffer[2];
//	ColorG = packetbuffer[3];
//	ColorB = packetbuffer[4];
//  }

//  // Buttons
//  if (packetbuffer[1] == 'B')
//  {
//	  if (packetbuffer[3] - '0')
//		  ButtonStates |= 1 << ((packetbuffer[2] - '0') - 1);
//	  else
//		  ButtonStates &= ~(1 << ((packetbuffer[2] - '0') - 1));

//    uint8_t buttnum = packetbuffer[2] - '0';
//    boolean pressed = packetbuffer[3] - '0';
//    switch (buttnum)
//    {
//    	case 1:
//    		Bluefruit_Button1 = pressed;
//    		break;
//    	case 2:
//    		Bluefruit_Button2 = pressed;
//    		break;
//    	case 3:
//    		Bluefruit_Button3 = pressed;
//    		break;
//    	case 4:
//    		Bluefruit_Button4 = pressed;
//    		break;
//    	case 5:
//    		Bluefruit_ButtonUp = pressed;
//    		break;
//    	case 6:
//    		Bluefruit_ButtonDown = pressed;
//    		break;
//    	case 7:
//    		Bluefruit_ButtonLeft = pressed;
//    		break;
//    	case 8:
//    		Bluefruit_ButtonRight = pressed;
//    		break;
//    	default:
//    		break;
//    }
//  }

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

void Bluefruit_Poll(void)
{
	if(Bluefruit_BuildPacket())
	{
		UpdateFlag = true;
		Bluefruit_ParsePacket();
	}
}

//bool Bluefruit_ButtonUp;
//bool Bluefruit_ButtonDown;
//bool Bluefruit_ButtonLeft;
//bool Bluefruit_ButtonRight;
//bool Bluefruit_Button1;
//bool Bluefruit_Button2;
//bool Bluefruit_Button3;
//bool Bluefruit_Button4;
////bool Bluefruit_GetButton1()			{return Bluefruit_Button1;}
////bool Bluefruit_GetButton2()			{return Bluefruit_Button2;}
////bool Bluefruit_GetButton3()			{return Bluefruit_Button3;}
////bool Bluefruit_GetButton4()			{return Bluefruit_Button4;}
////bool Bluefruit_GetButtonUp()			{return Bluefruit_ButtonUp;}
////bool Bluefruit_GetButtonDown()		{return Bluefruit_ButtonDown;}
////bool Bluefruit_GetButtonLeft()		{return Bluefruit_ButtonLeft;}
////bool Bluefruit_GetButtonRight()		{return Bluefruit_ButtonRight;}

char Bluefruit_GetPacketType()
{
	return Packet[1];
}

bool Bluefruit_GetUpdateFlag()			{return UpdateFlag;}
void Bluefruit_ResetUpdateFlag()		{UpdateFlag = 0;}

bool Bluefruit_GetColorUpdateFlag()		{return ColorUpdateFlag;}
void Bluefruit_ResetColorUpdateFlag()	{ColorUpdateFlag = 0;}

uint8_t Bluefruit_GetColorR() 		{return ColorR;}
uint8_t Bluefruit_GetColorG() 		{return ColorG;}
uint8_t Bluefruit_GetColorB() 		{return ColorB;}

bool Bluefruit_GetButton1()			{return ButtonStates.Button1;}
bool Bluefruit_GetButton2()			{return ButtonStates.Button2;}
bool Bluefruit_GetButton3()			{return ButtonStates.Button3;}
bool Bluefruit_GetButton4()			{return ButtonStates.Button4;}
bool Bluefruit_GetButtonUp()		{return ButtonStates.ButtonUp;}
bool Bluefruit_GetButtonDown()		{return ButtonStates.ButtonDown;}
bool Bluefruit_GetButtonLeft()		{return ButtonStates.ButtonLeft;}
bool Bluefruit_GetButtonRight()		{return ButtonStates.ButtonRight;}

// ----------------------------------------------------------------------------------------------
// COMMON SETTINGS
// ----------------------------------------------------------------------------------------------
//#define BUFSIZE                        256   // Size of the read buffer for incoming data
#define FACTORYRESET_ENABLE				0
#define VERBOSE_MODE                   true  // If set to 'true' enables debug output

Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);
// ----------------------------------------------------------------------------------------------
// HARDWARE UART SETTINGS
// ----------------------------------------------------------------------------------------------
#define BLUEFRUIT_HWSERIAL_NAME      Serial1
#define BLUEFRUIT_UART_MODE_PIN        6    // Set to -1 if unused
#define BLUEFRUIT_UART_CTS_PIN         11   // Required for software serial!
#define BLUEFRUIT_UART_RTS_PIN         -1   // Optional, set to -1 if unused


void Bluefruit_Restart(void)
{
	ble.begin(0, 0);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void Bluefruit_Init(void)
{
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE, 0) )
  {
    //error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
	  Serial.println(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  else
  {
	  Serial.println( F("OK!") );

	  if ( FACTORYRESET_ENABLE )
	  {
		/* Perform a factory reset to make sure everything is in a known state */
		Serial.println(F("Performing a factory reset: "));
		if ( ! ble.factoryReset() ){
		  error(F("Couldn't factory reset"));
		}
	  }

	  /* Disable command echo from Bluefruit */
	  ble.echo(false);

	  Serial.println("Requesting Bluefruit info:");
	  /* Print Bluefruit information */
	  ble.info();

	  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
	  Serial.println();

	  //ble.verbose(false);  // debug info is a little annoying after this point!

	  Serial.println(F("***********************"));

	  // Set Bluefruit to DATA mode
	  Serial.println( F("Switching to DATA mode!") );
	  ble.setMode(BLUEFRUIT_MODE_DATA);

	  Serial.println(F("***********************"));
  }
}


