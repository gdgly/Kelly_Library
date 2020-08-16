#ifndef _CAN_CONTROLLER_H_
#define _CAN_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>

/*! @brief CAN frame format. */
typedef enum
{
    CAN_FRAME_FORMAT_STANDARD 	= 0x0U, /*!< Standard frame format attribute. */
    CAN_FRAME_FORMAT_EXTEND 	= 0x1U, /*!< Extend frame format attribute. */
} CAN_FrameFormat_t;

/*! @brief CAN frame type. */
typedef enum
{
    CAN_FRAME_TYPE_DATA   = 0x0U, /*!< Data frame type attribute. */
    CAN_FRAME_TYPE_REMOTE = 0x1U, /*!< Remote frame type attribute. */
} CAN_FrameType_t;

/*! @brief CAN ID filter mode. */
typedef enum
{
    CAN_FILTER_32_BIT = 0x0U, /*!< Two 32-bit acceptance filters. */
    CAN_FILTER_16_BIT = 0x1U, /*!< Four 16-bit acceptance filters. */
    CAN_FILTER_8_BIT  = 0x2U, /*!< Eight 8-bit acceptance filters. */
    CAN_FILTER_CLOSED = 0x3U, /*!< Filter closed. */
} CAN_IDFilterMode_t;

typedef enum
{
    CAN_STATE_IDLE			= 0x0, /*!< MB/RxFIFO idle.*/
    CAN_STATE_RX_DATA   	= 0x1, /*!< MB receiving.*/
    CAN_STATE_RX_REMOTE 	= 0x2, /*!< MB receiving remote reply.*/
    CAN_STATE_TX_DATA   	= 0x3, /*!< MB transmitting.*/
    CAN_STATE_TX_REMOTE 	= 0x4, /*!< MB transmitting remote request.*/
    CAN_STATE_RX_FIFO   	= 0x5, /*!< RxFIFO receiving.*/
} CAN_MessageBufferState_t;

typedef union
{
    MSCAN_StandardID_t StdID; /*!< standard format */
    MSCAN_ExtendID_t ExtID;   /*!< extend format */
    uint32_t ID;                /*!< Identifier with 32 bit format */
} CAN_ID_t;                      /*!< identifier union */

///*! @brief MsCAN frame structure. */
typedef struct
{
	CAN_ID_t ID;
    union
    {
        uint8_t DSR[8]; /*!< data segment */
        struct
        {
            uint32_t dataWord0; /*!< MSCAN Frame payload word0. */
            uint32_t dataWord1; /*!< MSCAN Frame payload word1. */
        };
        struct
        {
            uint8_t dataByte0; /*!< MSCAN Frame payload byte0. */
            uint8_t dataByte1; /*!< MSCAN Frame payload byte1. */
            uint8_t dataByte2; /*!< MSCAN Frame payload byte2. */
            uint8_t dataByte3; /*!< MSCAN Frame payload byte3. */
            uint8_t dataByte4; /*!< MSCAN Frame payload byte4. */
            uint8_t dataByte5; /*!< MSCAN Frame payload byte5. */
            uint8_t dataByte6; /*!< MSCAN Frame payload byte6. */
            uint8_t dataByte7; /*!< MSCAN Frame payload byte7. */
        };
    };
    uint8_t DLR;                 /*!< data length */
    uint8_t BPR;                 /*!< transmit buffer priority */
    CAN_FrameType_t Type;     		/*!< remote frame or data frame */
    CAN_FrameFormat_t Format; 	/*!< extend frame or standard frame */
    uint8_t TSRH;                /*!< time stamp high byte */
    uint8_t TSRL;                /*!< time stamp low byte */
} CAN_Frame_t;

//
//typedef struct
//{
//	const uint8_t Code;
//    const char * Name;
//	void (*Function)(void);
//}
//CAN_Function_t, *CAN_FunctionHandle_t;

typedef struct
{
	volatile CAN_Frame_t TxFrame; //Frame Buffer
	volatile CAN_Frame_t RxFrame; //Frame Buffer
	volatile CAN_MessageBufferState_t TxState;
	volatile CAN_MessageBufferState_t RxState;
	volatile bool IsTxComplete;
	volatile bool IsRxComplete;
	bool (*MSCAN_GetRxBufferFullFlag)(void);
	bool (*MSCAN_GetTxBufferEmptyFlag)(void);
	void (*MSCAN_EnableTxBufferEmptyInterrupt)(void);
	void (*MSCAN_EnableRxBufferFullInterrupt)(void);
	void (*MSCAN_DisableTxBufferEmptyInterrupt)(void);
	void (*MSCAN_DisableRxBufferFullInterrupt)(void);
	void (*MSCAN_ReadRxMessageBuffer)(CAN_Frame_t * p_rxFrame);
	void (*MSCAN_WriteTxMessageBuffer)(CAN_Frame_t * p_rxFrame);
	void (*MSCAN_RxCompleteCallback)(void);
	void (*MSCAN_TxCompleteCallback)(void);
	void (*MSCAN_SetBaudRate)(uint32_t);
} CANController_t;

#endif
