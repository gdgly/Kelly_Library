#include <Platform/KE06/MSCAN.h>

#include "CANController.h"
#include "MKE06Z4.h"

#include <stdbool.h>
#include <stdint.h>

CANController_t CANController1;

void CAN_Rx_ISR(void) // MSCAN_1_IRQHandler,      // 46: MSCAN Rx Interrupt
{
//    if (MSCAN_GetRxBufferFullFlag(MSCAN))
//    {
		CANController_RxISR(&CANController1);
//    }
//
//	MSCAN_ClearRxBufferFullFlag(MSCAN);
}

void CAN_Tx_ISR(void) //MSCAN_2_IRQHandler,      // 47: MSCAN Tx, Err and Wake-up interrupt
{
	CANController_TxISR(&CANController1);
}


void main(void)
{
	uint32_t txID 				= 0x1;
	CAN_FrameFormat_t format 	= CAN_FRAME_FORMAT_EXTEND;
	CAN_FrameType_t type 		= CAN_FRAME_TYPE_DATA;
	uint32_t length 			= 8;
	uint32_t data[2] 			= {0x44332211, 0x88776655};

	KE06_MSCAN_Init(2400000);

	CANController_Init
	(
			KE06_MSCAN_GetRxBufferFullFlag,
			KE06_MSCAN_GetTxBufferEmptyFlag ,
			KE06_MSCAN_EnableTxBufferEmptyInterrupt,
			KE06_MSCAN_EnableRxBufferFullInterrupt,
			KE06_MSCAN_DisableTxBufferEmptyInterrupt,
			KE06_MSCAN_DisableRxBufferFullInterrupt,
			KE06_MSCAN_ClearRxBufferFullFlag,
			KE06_MSCAN_ReadRxMessageBuffer,
			KE06_MSCAN_WriteTxMessageBuffer,
			0,
			0,
			KE06_MSCAN_SetBaudRate,
			500000
	);

	//CANController_SetTxFrame(&CANController1, uint32_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
	CANController_SetTxFrame(&CANController1, txID, format, type, length, &data);
    CANController_SendFrame(&CANController1);
	CANController_ReceiveFrame(&CANController1);

	while(1)
	{
		CANController_ProcRxPoll(&CANController1);
	}
}
