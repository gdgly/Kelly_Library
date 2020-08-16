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
	CANController_Init
	(
		&CANController1,

		MSCAN1_GetRxBufferFullFlag,
		MSCAN1_GetTxBufferEmptyFlag,
		MSCAN1_SetBaudRate
	);

    /* Prepare Tx Frame for sending. */
    txFrame.ID_Type.ID = NODE_ID1;
    txFrame.format     = kMSCAN_FrameFormatExtend;
    txFrame.type       = kMSCAN_FrameTypeData;
    txFrame.DLR        = 8;
    txFrame.dataWord0  = 0x44332211;
    txFrame.dataWord1  = 0x88776655;

    PRINTF("Send message!\r\n");
    PRINTF("tx word0 = 0x%x\r\n", txFrame.dataWord0);
    PRINTF("tx word1 = 0x%x\r\n", txFrame.dataWord1);

	CAN_SendFrame(&CANController1);

	while(1);
}
