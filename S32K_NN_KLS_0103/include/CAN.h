/*
 * CAN.h
 *
 *  Created on: 2019年1月10日
 *      Author: Administrator
 */

#ifndef CAN_H_
#define CAN_H_


#define NUMBER_OF_MB 16

//CAN报文结构位定义
#define	CAN_MSG_IDE_MASK	(0x10000000L)
#define CAN_MSG_IDE_BIT_NO	(28)
#define CAN_MSG_ID_EXTENDED	(CAN_MSG_ID_MASK)
#define CAN_MSG_ID_STANDARD	(0)
#define CAN_MSG_TYPE_MASK 	(0x40000000L)
#define CAN_MSG_TYPE_BIT_NO	(30)
#define CAN_MSG_TYPE_DATA_FRAME	(CAN_MSG_TYPE_MASK)
#define CAN_MSG_TYPE_REMOTE_FRAME	(0)

//FELEX_MB_CS的位定义和宏
#define FLEXCAN_MB_CS_TIMESTAMP(x)    (((x)&0x0000FFFF)<<0)
#define FLEXCAN_MB_CS_TIMESTAMP_MASK  (0x0000FFFFL)
#define FLEXCAN_MB_CS_LENGTH(x)       (((x)&0x0000000F)<<16)
#define FLEXCAN_MB_CS_RTR             (0x00100000)
#define FLEXCAN_MB_CS_IDE             (0x00200000)
#define FLEXCAN_MB_CS_SRR             (0x00400000)
#define FLEXCAN_MB_CS_CODE(x)         (((x)&0x0000000F)<<24)
#define FLEXCAN_MB_CS_CODE_MASK	      (0x0F000000L)
#define FLEXCAN_MB_CS_DLC_MASK	      (0x000F0000L)
#define FLEXCAN_MB_CODE_RX_INACTIVE	(0)
#define FLEXCAN_MB_CODE_RX_EMPTY	(4)
#define FLEXCAN_MB_CODE_RX_FULL		(2)
#define FLEXCAN_MB_CODE_RX_OVERRUN	(6)
#define FLEXCAN_MB_CODE_RX_BUSY		(1)

#define FLEXCAN_MB_CS_IDE_BIT_NO        (21)
#define FLEXCAN_MB_CS_RTR_BIT_NO        (20)
#define FLEXCAN_MB_CS_DLC_BIT_NO        (16)

#define FLEXCAN_MB_CODE_TX_INACTIVE	        (8)
#define FLEXCAN_MB_CODE_TX_ABORT		    (9)
#define FLEXCAN_MB_CODE_TX_ONCE			    (0x0C)
#define FLEXCAN_MB_CODE_TX_RESPONSE		    (0x0A)
#define FLEXCAN_MB_CODE_TX_RESPONSE_TEMPO	(0x0E)
#define FLEXCAN_get_code(cs)				(((cs) & FLEXCAN_MB_CS_CODE_MASK)>>24)
#define FLEXCAN_get_length(cs)      (((cs) & FLEXCAN_MB_CS_DLC_MASK)>>16)


//FLEXCAN_MB_ID的位定义和宏
#define FLEXCAN_MB_ID_STD_MASK		   (0x1FFC0000L)
#define FLEXCAN_MB_ID_EXT_MASK		   (0x1FFFFFFFL)
#define FLEXCAN_MB_ID_IDEXT(x)        (((x)&0x0003FFFF)<<0)
#define FLEXCAN_MB_ID_IDSTD(x)        (((x)&0x000007FF)<<18)
#define FLEXCAN_MB_ID_PRIO(x)         (((x)&0x00000007)<<29)
#define FLEXCAN_MB_ID_PRIO_BIT_NO	  (29)
#define FLEXCAN_MB_ID_STD_BIT_NO	  (18)
#define FLEXCAN_MB_ID_EXT_BIT_NO	  (0)

//FLEXCAN_MB_WORD0的位定义和宏
#define FLEXCAN_MB_WORD0_DATA3(x)     (((x)&0x000000FF)<<0)
#define FLEXCAN_MB_WORD0_DATA2(x)     (((x)&0x000000FF)<<8)
#define FLEXCAN_MB_WORD0_DATA1(x)     (((x)&0x000000FF)<<16)
#define FLEXCAN_MB_WORD0_DATA0(x)     (((x)&0x000000FF)<<24)

//FLEXCAN_MB_WORD1的位定义和宏
#define FLEXCAN_MB_WORD1_DATA7(x)     (((x)&0x000000FF)<<0)
#define FLEXCAN_MB_WORD1_DATA6(x)     (((x)&0x000000FF)<<8)
#define FLEXCAN_MB_WORD1_DATA5(x)     (((x)&0x000000FF)<<16)
#define FLEXCAN_MB_WORD1_DATA4(x)     (((x)&0x000000FF)<<24)

#define CAN_ID_EXT_MASK                          0x3FFFFu
#define CAN_ID_EXT_SHIFT                         0
#define CAN_ID_EXT_WIDTH                         18

#define CAN_ID_STD_MASK                          0x1FFC0000u
#define CAN_ID_STD_SHIFT                         18
#define CAN_ID_STD_WIDTH                         11

typedef	struct
{
			uint32_t	TimeStamp:16;
			uint32_t	DLC:4;
			uint32_t	RTR:1;
			uint32_t	IDE:1;
			uint32_t	SRR:1;
			uint32_t	Reserved0:1;
			uint32_t	CODE:4;
			uint32_t	Reserved1:1;
			uint32_t	ESI:1;
			uint32_t	BRS:1;
			uint32_t	EDL:1;

			uint32_t	ID:29;
			uint32_t	PRIO:3;

			uint8_t		Data[8];

} 	MailBoxType;

typedef	struct
{
			uint32_t	RXIDA:30;
			uint32_t	IDE:1;
			uint32_t	RTR:1;

} 	RxFilterType;

typedef	struct
{
		MailBoxType		RxMBox[6];
		RxFilterType	RxFilter[8];
		MailBoxType		TxMBox[8];

}		MailBox;

const uint32_t ID_table[16];
uint8_t CANInit(uint8_t CANChannel,uint32_t baudrateKHz,uint32_t id_ext,uint32_t *id_table);
uint8_t CANSendData(uint8_t CANChannel, uint32_t id_ext, uint32_t id, uint8_t length,uint8_t Data[]);
uint8_t CANRecData(uint8_t CANChannel, uint32_t *id,uint8_t *Datalenght,uint8_t *Data);




#endif /* CAN_H_ */
