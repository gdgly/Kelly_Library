#ifndef _MSCAN_H_
#define _MSCAN_H_

#include <stdbool.h>
#include <stdint.h>

#include "CANController/CANController.h"

bool KE06_MSCAN_GetTxBufferEmptyFlag(void);
bool KE06_MSCAN_GetRxBufferFullFlag(void);
void KE06_MSCAN_EnableTxBufferEmptyInterrupt(void);
void KE06_MSCAN_EnableRxBufferFullInterrupt(void);
void KE06_MSCAN_DisableTxBufferEmptyInterrupt(void);
void KE06_MSCAN_DisableRxBufferFullInterrupt(void);
void KE06_MSCAN_ClearRxBufferFullFlag(void);
void KE06_MSCAN_SetBaudRate(uint32_t baudRate);
void KE06_MSCAN_WriteTxMessageBuffer(CAN_Frame_t * p_txFrame);
void KE06_MSCAN_ReadRxMessageBuffer(CAN_Frame_t * p_rxFrame);
/*! @brief Initialize KE06 MSCAN module. Default settings, interrupt driven for send and receive */
void KE06_MSCAN_Init(uint32_t busFreq);

#endif
