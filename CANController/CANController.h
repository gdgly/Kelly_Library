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
    CAN_STATE_COMPLETE,
} CAN_MessageBufferState_t;

/*! @brief MSCAN IDR1 struct. */
typedef struct
{
    uint8_t EID17_15 : 3;           /*!< Extended Format Identifier 17-15*/
    uint8_t R_TEIDE : 1;            /*!< ID Extended */
    uint8_t R_TSRR : 1;             /*!< Substitute Remote Request */
    uint8_t EID20_18_OR_SID2_0 : 3; /*!< Extended Format Identifier 18-20 or standard format bit 0-2*/
} MSCAN_IDR1Type;

/*! @brief MSCAN IDR3 struct. */
typedef struct
{
    uint8_t ERTR : 1;   /*!< Remote Transmission Request*/
    uint8_t EID6_0 : 7; /*!< Extended Format Identifier 6-0*/
} MSCAN_IDR3Type;

/*! @brief MSCAN idr1 and idr3 union. */
typedef union
{
    MSCAN_IDR1Type IDR1; /*!< structure for identifier 1 */
    MSCAN_IDR3Type IDR3; /*!< structure for identifier 3 */
    uint8_t Bytes;       /*!< bytes */
} IDR1_3_UNION;

/*! @brief MSCAN extend ID struct. */
typedef struct
{
    uint32_t EID6_0 : 7;   /*!< ID[0:6] */
    uint32_t EID14_7 : 8;  /*!< ID[14:7] */
    uint32_t EID17_15 : 3; /*!< ID[17:15] */
    uint32_t EID20_18 : 3; /*!< ID[20:18] */
    uint32_t EID28_21 : 8; /*!< ID[28:21] */
    uint32_t rsvd : 3;
} MSCAN_ExtendID_t;

/*! @brief MSCAN standard ID struct. */
typedef struct
{
    uint32_t EID2_0 : 3;  /*!< ID[0:2] */
    uint32_t EID10_3 : 8; /*!< ID[10:3] */
    uint32_t rsvd : 21;
} MSCAN_StandardID_t;

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
	void (*MSCAN_ClearRxBufferFullFlag)(void);
	void (*MSCAN_ReadRxMessageBuffer)(CAN_Frame_t * p_rxFrame);
	void (*MSCAN_WriteTxMessageBuffer)(CAN_Frame_t * p_rxFrame);
	void (*MSCAN_RxCompleteCallback)(void);
	void (*MSCAN_TxCompleteCallback)(void);
	void (*MSCAN_SetBaudRate)(uint32_t);
} CANController_t;

#endif
