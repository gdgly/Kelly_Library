#include <Platform/KE06/MSCAN.h>

#include "CANController/CANController.h"
#include "OS/OS.h"
#include "MKE06Z4.h"

#include <stdbool.h>
#include <stdint.h>


CANController_t CANController1;
LITE_FX_OS_THREAD_T ThreadCANTx;

uint8_t Data[8] 			= {0x44, 0x33, 0x22, 0x11, 0 , 0 , 0, 0};

uint16_t MotorPWM;

uint16_t MotorRPM;

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


void ProcessCANTx(void)
{
	MotorRPM++;
	PacketSpeed_RPMOffset10000 = MotorRPM + 10000;

	BuildTxPacket(CANController_GetTxFrame(&CANController1));
    CANController_StartTxFrame(&CANController1);
}

void ProcessCANRx(void)
{
	if (CANController_IsRxComplete(&CANController1))
	{
		ParseRxPacket(CANController_GetRxFrame(&CANController1));
		MotorPWM = PacketThrottle * 512 / 1000;
		//GetKellyThrottle(void);
		//UpdatePWM();
		CANController_StartRxFrame(&CANController1); //start next frame
	}
}

void mainApp(void)
{
	uint32_t txID 				= 0x1;
	CAN_FrameFormat_t format 	= CAN_FRAME_FORMAT_EXTEND;
	CAN_FrameType_t type 		= CAN_FRAME_TYPE_DATA;
	uint32_t length 			= 8;

	KE06_MSCAN_Init(2400000);

	Millis_Init(4800000, 200);
	LiteFXOS_InitMillis(Millis_GetTickCounter());
	LiteFXOS_InitThreadPeriodicArgPeriod(&ThreadCANTx, ProcessCANTx, 20);


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
	CANController_SetTxFrame(&CANController1, txID, format, type, length, &Data);

	CANController_StartRxFrame(&CANController1);

	while(1)
	{
		//Process Rx when available
		ProcessCANRx();

		//Process Tx every 20ms
		LiteFXOS_ProcThread(&ThreadCANTx);
	}
}
