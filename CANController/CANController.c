#include "CANController.h"

#include <stdint.h>
#include <stdbool.h>

void CANController_Init
(
	CANController_t * p_canController,
	bool (*getRxBufferFullFlag)(void),
	bool (*getTxBufferEmptyFlag)(void),
	void (*enableTxBufferEmptyInterrupt)(void),
	void (*enableRxBufferFullInterrupt)(void),
	void (*ensableTxBufferEmptyInterrupt)(void),
	void (*disableRxBufferFullInterrupt)(void),
	void (*clearRxBufferFullFlag)(void),
	void (*readRxMessageBuffer)(CAN_Frame_t * p_rxFrame),
	void (*writeTxMessageBuffer)(CAN_Frame_t * p_rxFrame),
	void (*rxCompleteCallback)(void),
	void (*txCompleteCallback)(void),
	void (*setBaudRate)(uint32_t),
	uint32_t baudRate
)
{
	p_canController->MSCAN_GetRxBufferFullFlag 				= getRxBufferFullFlag;
	p_canController->MSCAN_GetTxBufferEmptyFlag 			= getTxBufferEmptyFlag;
	p_canController->MSCAN_EnableTxBufferEmptyInterrupt 	= enableTxBufferEmptyInterrupt;
	p_canController->MSCAN_EnableRxBufferFullInterrupt 		= enableRxBufferFullInterrupt;
	p_canController->MSCAN_DisableTxBufferEmptyInterrupt 	= ensableTxBufferEmptyInterrupt;
	p_canController->MSCAN_DisableRxBufferFullInterrupt 	= disableRxBufferFullInterrupt;
	p_canController->MSCAN_ClearRxBufferFullFlag 			= clearRxBufferFullFlag;
	p_canController->MSCAN_ReadRxMessageBuffer 				= readRxMessageBuffer;
	p_canController->MSCAN_WriteTxMessageBuffer 			= writeTxMessageBuffer;
	p_canController->MSCAN_RxCompleteCallback 				= rxCompleteCallback;
	p_canController->MSCAN_TxCompleteCallback 				= txCompleteCallback;
	p_canController->MSCAN_SetBaudRate 						= setBaudRate;

	p_canController->MSCAN_SetBaudRate(baudRate);
}

void CANController_TxISR(CANController_t * p_canController)
{
	if(p_canController->MSCAN_GetTxBufferEmptyFlag())
	{
		switch (p_canController->TxState)
		{
			case (uint8_t)CAN_STATE_TX_DATA:
					//MSCAN_TransferAbortSend(base, handle, (uint8_t)kMSCAN_TxEmptyInterruptEnable);
				p_canController->MSCAN_DisableTxBufferEmptyInterrupt();
				p_canController->TxState = CAN_STATE_IDLE;
				break;

			case (uint8_t)CAN_STATE_TX_REMOTE:
					//status            = kStatus_MSCAN_TxSwitchToRx;
				p_canController->MSCAN_DisableTxBufferEmptyInterrupt();
				p_canController->TxState = CAN_STATE_IDLE;
				p_canController->MSCAN_EnableRxBufferFullInterrupt();
				p_canController->RxState = (uint8_t)CAN_STATE_RX_REMOTE;
				break;

			default:
				//status = kStatus_MSCAN_UnHandled;
				break;
		}
		p_canController->IsTxComplete = true;
	}

	if (p_canController->MSCAN_TxCompleteCallback) p_canController->MSCAN_TxCompleteCallback();
}

void CANController_RxISR(CANController_t * p_canController)
{
	if (p_canController->MSCAN_GetRxBufferFullFlag())
	{
		switch (p_canController->RxState)
		{
			case (uint8_t)CAN_STATE_RX_DATA:
				p_canController->MSCAN_ReadRxMessageBuffer(&(p_canController->RxFrame));
				p_canController->MSCAN_DisableRxBufferFullInterrupt;
				p_canController->RxState = CAN_STATE_IDLE;
				break;

			case (uint8_t)CAN_STATE_RX_REMOTE:
				p_canController->MSCAN_ReadRxMessageBuffer(&(p_canController->RxFrame));
				p_canController->MSCAN_DisableRxBufferFullInterrupt;
				p_canController->RxState = CAN_STATE_IDLE;
				break;

			default:
				/* To avoid MISRA-C 2012 rule 16.4 issue. */
				break;
		}
		p_canController->MSCAN_ClearRxBufferFullFlag();
		p_canController->IsRxComplete = true;
	}


	if (p_canController->MSCAN_RxCompleteCallback) p_canController->MSCAN_RxCompleteCallback();
}


void CANController_SetBaudRate(CANController_t * p_canController, uint32_t baudRate)
{
	p_canController->MSCAN_SetBaudRate(baudRate);
}


void CANController_Send(CANController_t * p_canController, uint32_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
{

}

//void CANController_SendByte(CANController_t * p_canController, uint8_t data)
//{
//
//}

void CANController_StartTxFrame(CANController_t * p_canController)
{
//	status_t status;

    //if (p_canController->TxState != CAN_STATE_IDLE) return;

	if (!p_canController->MSCAN_GetTxBufferEmptyFlag()) return;

	if (p_canController->TxFrame.Type = CAN_FRAME_TYPE_REMOTE)
	{
		p_canController->TxState = CAN_STATE_TX_REMOTE;
	}
	else
	{
		p_canController->TxState = CAN_STATE_TX_DATA;
	}
	p_canController->MSCAN_WriteTxMessageBuffer(&(p_canController->TxFrame));
	p_canController->MSCAN_EnableTxBufferEmptyInterrupt();

    p_canController->IsTxComplete = false;
//	return status;
}

//void CANController_Receive(CANController_t * p_canController, uint32_t * id, uint8_t * data)
//{
//
//}

//void CANController_ReceiveByte(CANController_t * p_canController)
//{
//
//}

void CANController_StartRxFrame(CANController_t * p_canController)
{
    if (p_canController->RxState != CAN_STATE_IDLE) return;

	p_canController->RxState = CAN_STATE_RX_DATA;
	p_canController->MSCAN_EnableRxBufferFullInterrupt();
	p_canController->IsRxComplete = false;
}

bool CANController_IsRxComplete(CANController_t * p_canController)
{
    return (p_canController->IsRxComplete);
}

bool CANController_IsTxComplete(CANController_t * p_canController)
{
    return (p_canController->IsTxComplete);
}

//Set all frame information including data
void CANController_SetTxFrame(CANController_t * p_canController, CAN_ID_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
{
    /* Prepare Tx Frame for sending. */
	p_canController->TxFrame.ID 		= id;
	p_canController->TxFrame.Format 	= format;
	p_canController->TxFrame.Type 		= type;
	p_canController->TxFrame.DLR 		= length;

	for (uint8_t i = 0; i < length; i++)
	{
		p_canController->TxFrame.DSR[i]  = p_data[i];
	}
}

//Set all frame information expect data
void CANController_SetTxFrameConfig(CANController_t * p_canController, CAN_ID_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length)
{
	p_canController->TxFrame.ID 		= id;
	p_canController->TxFrame.Format 	= format;
	p_canController->TxFrame.Type 		= type;
	p_canController->TxFrame.DLR 		= length;
}


void CANController_SetTxFrameDataByte(CANController_t * p_canController, uint8_t dataByte, uint8_t dataIndex)
{
	p_canController->TxFrame.DSR[dataIndex]  = dataByte;
}

void CANController_GetRxFrameData(CANController_t * p_canController, uint32_t * p_id, uint8_t * p_data)
{
	&p_id = p_canController->RxFrame.ID;

	for (uint8_t i = 0; i < p_canController->RxFrame.DLR; i++)
	{
		p_data[i] = p_canController->RxFrame.DSR[i];
	}
}

CAN_Frame_t * CANController_GetRxFrame(CANController_t * p_canController)
{
	return (p_canController->RxFrame);
}

CAN_Frame_t * CANController_GetTxFrame(CANController_t * p_canController)
{
	return (p_canController->TxFrame);
}


//void CANController_ProcRxPoll(CANController_t * p_canController)
//{
//	 uint32_t * p_id;
//	 uint8_t * p_data;
//
//	 if (CANController_IsRxComplete(p_canController))
//	 {
//		 CANController_GetRxFrame(p_canController, p_id, p_data);
//		 //parse;
//		 //table function;
//		 CANController_ReceiveFrame(p_canController); //next frame
//	 }
//}

