#include "CANController.h"

#include <stdint.h>
#include <stdbool.h>

void CANController_Init
(
	CANController_t * canController,
	void (*rxCompleteCallback)(void),
	void (*txCompleteCallback)(void),
	bool (*getRxBufferFullFlag)(void),
	bool (*getTxBufferEmptyFlag)(void),
	void (*setBaudRate)(uint32_t),
)
{
	canController->MSCAN_RxCompleteCallback 		= rxCompleteCallback;
	canController->MSCAN_TxCompleteCallback 		= txCompleteCallback;
	canController->MSCAN_GetRxBufferFullFlag 		= getRxBufferFullFlag;
	canController->MSCAN_GetTxBufferEmptyFlag 		= getTxBufferEmptyFlag;
	canController->MSCAN_SetBaudRate 				= setBaudRate;

	canController->MSCAN_SetBaudRate(baudRate);
}

void CANController_TxISR(CANController_t * canController)
{
	//status_t status = kStatus_MSCAN_UnHandled;
	if(canController->MSCAN_GetTxBufferEmptyFlag())
	{
		switch (canController->TxState)
		{
			case (uint8_t)CAN_STATE_TX_DATA:
					//status = kStatus_MSCAN_TxIdle;
					//MSCAN_TransferAbortSend(base, handle, (uint8_t)kMSCAN_TxEmptyInterruptEnable);
				canController->MSCAN_DisableTxBufferEmptyInterrupt();
				canController->TxState = CAN_STATE_IDLE;
				break;

			case (uint8_t)CAN_STATE_TX_REMOTE:
					//status            = kStatus_MSCAN_TxSwitchToRx;
				canController->MSCAN_DisableTxBufferEmptyInterrupt(); // SLi- Should this be here?
				canController->RxState = (uint8_t)CAN_STATE_RX_REMOTE;
				canController->TxState = CAN_STATE_IDLE;
				break;

			default:
				//status = kStatus_MSCAN_UnHandled;
				break;
		}
	}

	canController->IsTxComplete = true;
	if (canController->MSCAN_TxCompleteCallback) canController->MSCAN_TxCompleteCallback();
}

void CANController_RxISR(CANController_t * canController)
{
	/* Get current State of Message Buffer. */
	if (canController->MSCAN_GetRxBufferFullFlag())
	{
		switch (canController->RxState)
		{
			case (uint8_t)CAN_STATE_RX_DATA:
					//                status = MSCAN_ReadRxMb(base, handle->mbFrameBuf);
					//                if (kStatus_Success == status)
					//                {
					//                    status = kStatus_MSCAN_RxIdle;
					//                }
					//MSCAN_TransferAbortReceive(base, handle, (uint8_t)kMSCAN_RxFullInterruptEnable);
				canController->MSCAN_ReadRxMessageBuffer(&(canController->RxFrame));
				canController->MSCAN_DisableRxBufferFullInterrupt;
				canController->RxState = CAN_STATE_IDLE;
				break;

			case (uint8_t)CAN_STATE_RX_REMOTE:
					//                status = MSCAN_ReadRxMb(base, handle->mbFrameBuf);
					//                if (kStatus_Success == status)
					//                {
					//                    status = kStatus_MSCAN_RxIdle;
					//                }
					// MSCAN_TransferAbortReceive(base, handle, (uint8_t)kMSCAN_RxFullInterruptEnable);
				canController->MSCAN_ReadRxMessageBuffer(&(canController->RxFrame));
				canController->MSCAN_DisableRxBufferFullInterrupt;
				canController->RxState = CAN_STATE_IDLE;
				break;

			default:
				/* To avoid MISRA-C 2012 rule 16.4 issue. */
				break;
		}
		//MSCAN_ClearRxBufferFullFlag(base);
	}

	canController->IsRxComplete= true;
	if (canController->MSCAN_RxCompleteCallback) canController->MSCAN_RxCompleteCallback();
}


void CANController_SetBaudRate(CANController_t * canController, uint32_t baudRate)
{
	canController->MSCAN_SetBaudRate(baudRate);
}


void CANController_Send(CANController_t * canController, uint32_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
{

}

//void CANController_SendByte(CANController_t * canController, uint8_t data)
//{
//
//}

void CANController_SendFrame(CANController_t * canController)
{
//	status_t status;

	if (!canController->MSCAN_GetTxBufferEmptyFlag()) return;

	if (canController->TxFrame.Type = CAN_FRAME_TYPE_REMOTE)
	{
		canController->TxState = CAN_STATE_TX_REMOTE;
	}
	else
	{
		canController->TxState = CAN_STATE_TX_DATA;
	}
	canController->MSCAN_WriteTxMessageBuffer(&(canController->TxFrame));
	canController->MSCAN_EnableTxBufferEmptyInterrupt();

    canController->IsTxComplete = false;
//	return status;
}

//void CANController_Receive(CANController_t * canController, uint32_t * id, uint8_t * data)
//{
//
//}

//void CANController_ReceiveByte(CANController_t * canController)
//{
//
//}

void CANController_ReceiveFrame(CANController_t * canController)
{
    if (canController->RxState == CAN_STATE_IDLE)
    {
    	canController->RxState = CAN_STATE_RX_DATA;
    	canController->MSCAN_EnableRxBufferFullInterrupt();
    	canController->IsRxComplete = false;
    }
}

bool CANController_IsRxComplete(CANController_t * canController)
{
    return canController->IsRxComplete;
}

bool CANController_IsTxComplete(CANController_t * canController)
{
    return canController->IsTxComplete;
}


void CANController_SetTxFrame(CANController_t * canController, uint32_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
{

}

void CANController_GetRxFrame(CANController_t * canController, uint32_t * id, uint8_t * data)
{

}
