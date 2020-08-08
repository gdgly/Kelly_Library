/*
 * ETS.h
 *
 *  Created on: Jul 26, 2014
 *      Author: Admin
 */

#ifndef ETS_H_
#define ETS_H_

#include "typedefine.h"


#define ETS_MAX_DATA_LEN    16  //limited memory  

#define ETS_RX_DATA			ETS_RX_Buf.buffer.fields.data
#define ETS_TX_DATA			ETS_TX_Buf.buffer.fields.data
#define ETS_RX_BYTES		ETS_RX_Buf.buffer.fields.no_bytes
#define ETS_TX_BYTES		ETS_TX_Buf.buffer.fields.no_bytes
#define ETS_RX_CMD			ETS_RX_Buf.buffer.fields.command
#define ETS_TX_CMD			ETS_TX_Buf.buffer.fields.command
#define ETS_RX_RAW			ETS_RX_Buf.buffer.raw_bytes
#define ETS_TX_RAW			ETS_TX_Buf.buffer.raw_bytes


#define ETS_INVALID_COMMAND   	0xe3
#define ETS_INVALID_PARAMETER 	0xe6
#define ETS_CHECKSUM_ERROR 	  	0xe7

/* Valid command ID's */      

#define  ETS_WATCHDOG_TEST              0x10 //1 byte data ,Not return value,will restart 

#define  ETS_CODE_VERSION               0x11 //No data. up to 2 bytes return.

#define  ETS_COM_ECHO_TEST              0x12 //1 byte data

#define  ETS_A2D_BATCH_READ 			      0x1b //0 byte data,return all channel 16 byte

#define  ETS_A2D_BATCH_READ2         		0x1c //3 byte data (channel,2 bytes duty)

#define  ETS_ETS_MONITOR1							0x20

#define  ETS_ETS_MONITOR2							0x21

#define  ETS_ETS_MONITOR3							0x22

#define  ETS_ETS_MONITOR4							0x23

#define  ETS_MONITOR1                   0x33 //0 byte data ,Return 8 byte 

#define  ETS_MONITOR2                   0x34 //0 byte data ,Return 2 byte 

#define  ETS_GET_PHASE_I_AD             0x35 //0 byte data ,Return 3 byte

#define  ETS_GET_PHASE_VOLTAGE          0x36 //0 byte data ,Return 4 byte

#define  ETS_USER_MONITOR1							0x3A

#define  ETS_USER_MONITOR2							0x3B

#define  ETS_USER_MONITOR3							0x3C



#define  ETS_GET_SEED                   0xE1 //0 byte data ,Return 1 byte 

#define  ETS_VALIDATE_SEED              0xE2 //2 byte data ,Return 1 byte

#define  ETS_FLASH_OPEN                 0xF1 //0 byte data

#define  ETS_FLASH_READ                 0xF2 //1 byte offset,1 byte size,1 byte type

#define  ETS_FLASH_WRITE                0xF3 //1 byte offset,1 byte size,1 byte type,x byte data

#define  ETS_FLASH_CLOSE                0xF4 //0 byte data

#define  ETS_FLASH_INFO_OPEN            0xF5 //0 byte data
                                        
#define  ETS_FLASH_INFO_READ            0xF6 //1 byte offset,1 byte size

#define  ETS_FLASH_INFO_WRITE           0xF7 //1 byte offset,1 byte size,x byte data

#define  ETS_FLASH_INFO_CLOSE           0xF8 //0 byte data

#define  ETS_FLASH_INFO_CONFIG          0xF9 //0 byte data,return two bytes

#define  ETS_FLASH_INFO_VERSION         0xFA //0 byte data,return 15 bytes


//////////BootLoader Command////////////


#define  ETS_ERASE_FLASH                0xB1 //1 data indication whether earse Calibration page, on 1 is erase calibration page.  1 bytes return.

#define  ETS_BURNT_FLASH                0xB2 //2 byte flash offset address,1 byte number, up to 13 bytes write.

#define  ETS_BURNT_CHECKSUM             0xB3 //1 byte mode,1 byte checksum.return 0 is error,1 is OK

#define  ETS_BURNT_RESET                0xB4 //0 byte ,return 0 byte,will reset chip

#define  ETS_CODE_END_ADDRESS						0xB5


typedef struct 
{
    union
    {
        uint8_t raw_bytes[ETS_MAX_DATA_LEN+3];
        struct
        {
           uint8_t  command;
           uint8_t  no_bytes;  //shall be less than ETS_MAX_DATA_LEN. It doesn't include checksum byte.
           uint8_t  data[ETS_MAX_DATA_LEN + 1]; //include checksum
        } fields;
    } buffer;

    /*
     * Reception: The next free byte in the message.
     * Transmission: The next byte to send in the message.
     */
    uint8_t index;
} T_ETS_Buf;

typedef enum
{
	ETS_RECEIVED = 0,
	ETS_RECEIVING
} T_ETS_Rx_State;

typedef enum
{
	ETS_TRANSMITTED = 0,
	ETS_TRANSMITTING
} T_ETS_Tx_State;


void Call_ETS(void);
void ETS_Action(void);



#endif /* ETS_H_ */
