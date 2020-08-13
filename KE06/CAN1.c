
#include "KE06/CAN1.h"

//#include "Cpu.h"
//#include "PE_Types.h"

#include "IO_Map.h"
#include "MSCAN_PDD.h"

#include <stdint.h>
#include <stdbool.h>


/* {Default RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static CAN1_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;
/* {Default RTOS Adapter} Global variable used for passing a parameter into ISR */
static CAN1_TDeviceDataPtr INT_MSCAN_TX__DEFAULT_RTOS_ISRPARAM;
/* {Default RTOS Adapter} Global variable used for passing a parameter into ISR */
static CAN1_TDeviceDataPtr INT_MSCAN_RX__DEFAULT_RTOS_ISRPARAM;

#define AVAILABLE_EVENTS_MASK (LDD_CAN_ON_FULL_RXBUFFER | LDD_CAN_ON_FREE_TXBUFFER)
#define CAN1_CAN_MBUFFERS 0x02U        /* Number of message buffers */
static const uint8_t RemapBuffersIdxToHw[2] = {0x00U, 0x00U}; /* Remapping message buffers index to Hw */
static const uint8_t RemapHwToTxBuffersIdx[1] = {0x01U}; /* Remapping transmit Hw buffer to component message buffers index */
static const uint8_t RemapHwToRxBuffersIdx[1] = {0x00U}; /* Remapping receive Hw buffer to component message buffers index */



/*
** ===================================================================
**     Method      :  CAN1_Init (component CAN_LDD)
*/
/*!
**     @brief
**         Initializes the device. Allocates memory for the device data
**         structure, allocates interrupt vectors and sets interrupt
**         priority, sets pin routing, sets timing, etc. If the "Enable
**         in init. code" is set to "yes" value then the device is also
**         enabled(see the description of the Enable() method). In this
**         case the Enable() method is not necessary and needn't to be
**         generated.
**     @param
**         UserDataPtr     - Pointer to the user or
**                           RTOS specific data. This pointer will be
**                           passed as an event or callback parameter.
**     @return
**                         - Pointer to the dynamically allocated private
**                           structure or NULL if there was an error.
*/
/* ===================================================================*/
LDD_TDeviceData* CAN1_Init(LDD_TUserData *UserDataPtr)
{
  /* Allocate LDD device structure */
  CAN1_TDeviceDataPtr DeviceDataPrv;
  /* {Default RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;

  DeviceDataPrv->BaseAddr = MSCAN_BASE_PTR; /* Device base address*/
  DeviceDataPrv->UserData = UserDataPtr; /* Store the RTOS device structure */
  DeviceDataPrv->MaxDataLen = 0x08U;   /* Max number of data to be sent in one frame */
  DeviceDataPrv->MaxBufferIndex = (LDD_CAN_TMBIndex)(CAN1_CAN_MBUFFERS-1U); /* Number of max. message buffer index */
  DeviceDataPrv->BuffersNumber = (LDD_CAN_TMBIndex)CAN1_CAN_MBUFFERS; /* Number of message buffers */
  DeviceDataPrv->RxBufferMask = 0x01U; /* Bit mask for message buffers configured as receive */
  DeviceDataPrv->TxBufferMask = 0x02U; /* Bit mask for message buffers configured as transmit */
  DeviceDataPrv->BuffersIdxToHwPtr = (LDD_CAN_TMBIndex *)RemapBuffersIdxToHw; /* Set pointer to BuffersIdxToHwPtr remap table */
  DeviceDataPrv->HwToRxBuffersIdxPtr = (LDD_CAN_TMBIndex *)RemapHwToRxBuffersIdx; /* Set pointer to HwToRxBuffersIdx remap table */
  DeviceDataPrv->HwToTxBuffersIdxPtr = (LDD_CAN_TMBIndex *)RemapHwToTxBuffersIdx; /* Set pointer to HwToTxBuffersIdx remap table */
  DeviceDataPrv->TxBuffersPendingMask = 0x00U; /* Initialize used Tx buffers mask variable */
  DeviceDataPrv->Index = 0x00U;        /* Set the component instance index */
  /* Clock Gating initialization */
  /* SIM_SCGC: MSCAN=1 */
  SIM_SCGC |= SIM_SCGC_MSCAN_MASK;
  /* Allocate interrupt vectors */
  /* {Default RTOS Adapter} Set interrupt vector: IVT is static, ISR parameter is passed by the global variable */
  INT_MSCAN_TX__DEFAULT_RTOS_ISRPARAM = DeviceDataPrv;
  /* {Default RTOS Adapter} Set interrupt vector: IVT is static, ISR parameter is passed by the global variable */
  INT_MSCAN_RX__DEFAULT_RTOS_ISRPARAM = DeviceDataPrv;
  /* SIM_PINSEL1: MSCANPS=1 */
  SIM_PINSEL1 |= SIM_PINSEL1_MSCANPS_MASK;
  /* Interrupt priorities */
  /* NVIC_IPR7: PRI_31=1 */
  NVIC_IPR7 = (uint32_t)((NVIC_IPR7 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_31(0x02)
              )) | (uint32_t)(
               NVIC_IP_PRI_31(0x01)
              ));
  /* NVIC_ISER: SETENA31=1,SETENA30=0,SETENA29=0,SETENA28=0,SETENA27=0,SETENA26=0,SETENA25=0,SETENA24=0,SETENA23=0,SETENA22=0,SETENA21=0,SETENA20=0,SETENA19=0,SETENA18=0,SETENA17=0,SETENA16=0,SETENA15=0,SETENA14=0,SETENA13=0,SETENA12=0,SETENA11=0,SETENA10=0,SETENA9=0,SETENA8=0,SETENA7=0,SETENA6=0,SETENA5=0,SETENA4=0,SETENA3=0,SETENA2=0,SETENA1=0,SETENA0=0 */
  NVIC_ISER = NVIC_ISER_SETENA31_MASK;
  /* NVIC_ICER: CLRENA31=0,CLRENA30=0,CLRENA29=0,CLRENA28=0,CLRENA27=0,CLRENA26=0,CLRENA25=0,CLRENA24=0,CLRENA23=0,CLRENA22=0,CLRENA21=0,CLRENA20=0,CLRENA19=0,CLRENA18=0,CLRENA17=0,CLRENA16=0,CLRENA15=0,CLRENA14=0,CLRENA13=0,CLRENA12=0,CLRENA11=0,CLRENA10=0,CLRENA9=0,CLRENA8=0,CLRENA7=0,CLRENA6=0,CLRENA5=0,CLRENA4=0,CLRENA3=0,CLRENA2=0,CLRENA1=0,CLRENA0=0 */
  NVIC_ICER = 0x00U;
  /* NVIC_IPR7: PRI_30=1 */
  NVIC_IPR7 = (uint32_t)((NVIC_IPR7 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_30(0x02)
              )) | (uint32_t)(
               NVIC_IP_PRI_30(0x01)
              ));
  /* NVIC_ISER: SETENA31=0,SETENA30=1,SETENA29=0,SETENA28=0,SETENA27=0,SETENA26=0,SETENA25=0,SETENA24=0,SETENA23=0,SETENA22=0,SETENA21=0,SETENA20=0,SETENA19=0,SETENA18=0,SETENA17=0,SETENA16=0,SETENA15=0,SETENA14=0,SETENA13=0,SETENA12=0,SETENA11=0,SETENA10=0,SETENA9=0,SETENA8=0,SETENA7=0,SETENA6=0,SETENA5=0,SETENA4=0,SETENA3=0,SETENA2=0,SETENA1=0,SETENA0=0 */
  NVIC_ISER = NVIC_ISER_SETENA30_MASK;
  /* NVIC_ICER: CLRENA31=0,CLRENA30=0,CLRENA29=0,CLRENA28=0,CLRENA27=0,CLRENA26=0,CLRENA25=0,CLRENA24=0,CLRENA23=0,CLRENA22=0,CLRENA21=0,CLRENA20=0,CLRENA19=0,CLRENA18=0,CLRENA17=0,CLRENA16=0,CLRENA15=0,CLRENA14=0,CLRENA13=0,CLRENA12=0,CLRENA11=0,CLRENA10=0,CLRENA9=0,CLRENA8=0,CLRENA7=0,CLRENA6=0,CLRENA5=0,CLRENA4=0,CLRENA3=0,CLRENA2=0,CLRENA1=0,CLRENA0=0 */
  NVIC_ICER = 0x00U;
  /* MSCAN_CANCTL1: CANE=1,CLKSRC=1,LOOPB=0,LISTEN=0,BORM=0,WUPM=0,SLPAK=0,INITAK=0 */
  MSCAN_CANCTL1 = (MSCAN_CANCTL1_CANE_MASK | MSCAN_CANCTL1_CLKSRC_MASK); /* Set control 1 register */
  /* MSCAN_CANCTL0: RXFRM=0,RXACT=0,CSWAI=0,SYNCH=0,TIME=0,WUPE=0,SLPRQ=0,INITRQ=1 */
  MSCAN_CANCTL0 = MSCAN_CANCTL0_INITRQ_MASK; /* Set control 0 register */
  /* MSCAN_CANIDAC: ??=0,??=0,IDAM=0,??=0,IDHIT=0 */
  MSCAN_CANIDAC = (MSCAN_CANIDAC_IDAM(0x00) | MSCAN_CANIDAC_IDHIT(0x00)); /* Set identifier acceptance control register */
  /* MSCAN_CANIDMR0: AM=0xFF */
  MSCAN_CANIDMR0 = MSCAN_CANIDMR_BANK_1_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR0 */
  /* MSCAN_CANIDMR1: AM=0xFF */
  MSCAN_CANIDMR1 = MSCAN_CANIDMR_BANK_1_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR1 */
  /* MSCAN_CANIDMR2: AM=0xFF */
  MSCAN_CANIDMR2 = MSCAN_CANIDMR_BANK_1_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR2 */
  /* MSCAN_CANIDMR3: AM=0xFF */
  MSCAN_CANIDMR3 = MSCAN_CANIDMR_BANK_1_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR3 */
  /* MSCAN_CANIDMR4: AM=0xFF */
  MSCAN_CANIDMR4 = MSCAN_CANIDMR_BANK_2_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR4 */
  /* MSCAN_CANIDMR5: AM=0xFF */
  MSCAN_CANIDMR5 = MSCAN_CANIDMR_BANK_2_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR5 */
  /* MSCAN_CANIDMR6: AM=0xFF */
  MSCAN_CANIDMR6 = MSCAN_CANIDMR_BANK_2_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR6 */
  /* MSCAN_CANIDMR7: AM=0xFF */
  MSCAN_CANIDMR7 = MSCAN_CANIDMR_BANK_2_AM(0xFF); /* Set the acceptance mask - register MSCAN_CANIDMR7 */
  /* MSCAN_CANIDAR0: AC=0xFF */
  MSCAN_CANIDAR0 = MSCAN_CANIDAR_BANK_1_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR0 */
  /* MSCAN_CANIDAR1: AC=0xFF */
  MSCAN_CANIDAR1 = MSCAN_CANIDAR_BANK_1_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR1 */
  /* MSCAN_CANIDAR2: AC=0xFF */
  MSCAN_CANIDAR2 = MSCAN_CANIDAR_BANK_1_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR2 */
  /* MSCAN_CANIDAR3: AC=0xFF */
  MSCAN_CANIDAR3 = MSCAN_CANIDAR_BANK_1_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR3 */
  /* MSCAN_CANIDAR4: AC=0xFF */
  MSCAN_CANIDAR4 = MSCAN_CANIDAR_BANK_2_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR4 */
  /* MSCAN_CANIDAR5: AC=0xFF */
  MSCAN_CANIDAR5 = MSCAN_CANIDAR_BANK_2_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR5 */
  /* MSCAN_CANIDAR6: AC=0xFF */
  MSCAN_CANIDAR6 = MSCAN_CANIDAR_BANK_2_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR6 */
  /* MSCAN_CANIDAR7: AC=0xFF */
  MSCAN_CANIDAR7 = MSCAN_CANIDAR_BANK_2_AC(0xFF); /* Set the acceptance code - register MSCAN_CANIDAR7 */
  /* MSCAN_CANBTR0: SJW=0,BRP=0x3F */
  MSCAN_CANBTR0 = (MSCAN_CANBTR0_SJW(0x00) | MSCAN_CANBTR0_BRP(0x3F)); /* Set the timing register 0 */
  /* MSCAN_CANBTR1: SAMP=0,TSEG2=1,TSEG1=3 */
  MSCAN_CANBTR1 = (MSCAN_CANBTR1_TSEG2(0x01) | MSCAN_CANBTR1_TSEG1(0x03)); /* Set the timing register 1 */
  DeviceDataPrv->TxBuffersPendingMask = 0x00U; /* Clear Tx request pending message buffer mask */
  MSCAN_PDD_EnableInitializationMode(MSCAN_BASE_PTR, PDD_DISABLE); /* Exit from initialization mode */
  while(MSCAN_PDD_GetInitializationModeAcknowledgeFlag(MSCAN_BASE_PTR) != 0U) {} /* Wait for enable */
  MSCAN_PDD_ClearRxStatusInterruptFlags(MSCAN_BASE_PTR, (MSCAN_PDD_WAKEUP_REQUESTED_FLAG | MSCAN_PDD_ERROR_COUNTER_CHANGE_FLAG | MSCAN_PDD_RX_BUFFER_OVERRUN_FLAG)); /* Reset interrupt flags */
  MSCAN_PDD_EnableRxInterruptsMask(MSCAN_BASE_PTR, MSCAN_PDD_RX_BUFFER_FULL_INT); /* Enable/configure interrupt */
  /* Registration of the device structure */
  PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_CAN1_ID,DeviceDataPrv);
  return ((LDD_TDeviceDataPtr)DeviceDataPrv);
}

/*
** ===================================================================
**     Method      :  CAN1_SetRxIDFilterTable (component CAN_LDD)
*/
/*!
**     @brief
**         Sets the receive ID filter table element specified by the
**         parameter ElementIdx. This method is available only if
**         receiver FIFO mode is enabled.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         ElementIdx      - Index of the ID filter table
**                           element.
**     @param
**         AccCode         - 11-bit Mask could be selected for
**                           a standard frame or 29-bit for an extended
**                           frame. 
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_PARAM_RANGE - Value of receive ID
**                           filter element index is out of range.
**                           ERR_NOTAVAIL - Receive ID filter table
**                           isn't possible change when CAN component is
**                           running, component must be disabled or
**                           switched to freeze mode.
*/
/* ===================================================================*/
LDD_TError CAN1_SetRxIDFilterTable(LDD_TDeviceData *DeviceDataPtr, LDD_CAN_TElementIndex ElementIdx, LDD_CAN_TAccCode AccCode)
{
  (void)DeviceDataPtr;                 /* Parameter is not used, suppress unused argument warning */

  if (ElementIdx > 0x01U) {            /* Is ElementIdx greater than MaxBuffers */
    return ERR_PARAM_RANGE;            /* If yes then error */
  }
  if (MSCAN_PDD_GetInitializationModeAcknowledgeFlag(MSCAN_BASE_PTR) == 0U) { /* Is CAN in running mode? */
    return ERR_NOTAVAIL;               /* If yes then error */
  }
  if (ElementIdx == 0U) {
    MSCAN_PDD_SetIdAcceptanceCode1stBank(MSCAN_BASE_PTR, AccCode); /* Set the individual acceptance code for first bank */
  } else {
    MSCAN_PDD_SetIdAcceptanceCode2ndBank(MSCAN_BASE_PTR, AccCode); /* Set the individual acceptance code for second bank */
  }
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  CAN1_SendFrame (component CAN_LDD)
*/
/*!
**     @brief
**         Sends a frame via the CAN device. This method allow to
**         specify CAN buffer number, message ID, data to be sent,
**         frame type and whether the message will be sent after the
**         request comes. 
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         BufferIdx       - Index of the Tx message buffer.
**     @param
**         Frame           - Pointer to the CAN frame to send.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_DISABLED - This component is disabled
**                           by user
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_PARAM_RANGE - Value of buffer index is
**                           out of range.
**                           ERR_PARAM_INDEX - Index of message buffer
**                           is not for transmit.
**                           ERR_PARAM_LENGTH - Number of data in the
**                           frame is greater than MaxDataLength.
**                           ERR_PARAM_ATTRIBUTE_SET - Frame type isn't
**                           supported.
**                           ERR_PARAM_VALUE - Value of Tx priority is
**                           fail.
**                           ERR_BUSY - CAN module is busy.
*/
/* ===================================================================*/
LDD_TError CAN1_SendFrame(LDD_TDeviceData *DeviceDataPtr, LDD_CAN_TMBIndex BufferIdx, LDD_CAN_TFrame *Frame)
{
  CAN1_TDeviceData *DeviceDataPrv = (CAN1_TDeviceData *)DeviceDataPtr;
  LDD_CAN_TBufferMask RemapBufferMask; /* Remap bitmask of the requested message buffer */
  LDD_CAN_TBufferMask BufferMask;      /* Bitmask of the requested message buffer */
  LDD_CAN_TBufferMask TxMbStatus = MSCAN_PDD_GetTxMessageBufferFlagMask(MSCAN_BASE_PTR); /* Get Tx message buffer status */
  uint8_t DataIndex;

  if (BufferIdx > DeviceDataPrv->MaxBufferIndex) { /* Is BufferIdx greater than MaxBuffers? */
    return ERR_PARAM_RANGE;            /* If yes then error */
  }
  BufferMask = (LDD_CAN_TBufferMask)(0x01UL << BufferIdx);
  if ((BufferMask & DeviceDataPrv->TxBufferMask) == 0U) { /* Is used buffer defined of BufferIdx for transmit? */
    return ERR_PARAM_INDEX;            /* If no then error */
  }
  if (Frame->Length > DeviceDataPrv->MaxDataLen) { /* Is number of data greater than MaxDataLen? */
    return ERR_PARAM_LENGTH;           /* If yes then error */
  }
  if (Frame->FrameType > LDD_CAN_REMOTE_FRAME) { /* Is FrameType other than LDD_CAN_DATA_FRAME or LDD_CAN_REMOTE_FRAME? */
    return ERR_PARAM_ATTRIBUTE_SET;    /* If yes then error */
  }
  RemapBufferMask = (LDD_CAN_TBufferMask)((LDD_CAN_TBufferMask)0x01U << DeviceDataPrv->BuffersIdxToHwPtr[BufferIdx]);
  if ((TxMbStatus & RemapBufferMask) == 0U) { /* Are both IDLE and TX/RX bits of the status register cleared?*/
    return ERR_BUSY;                   /* If yes then error */
  }
  /* {Default RTOS Adapter} Critical section begin, general PE function is used */
  EnterCritical();
  MSCAN_PDD_SelectTxMessageBufferMask(MSCAN_BASE_PTR, RemapBufferMask); /* Select requested transmit buffer */
  if ((Frame->MessageID & LDD_CAN_MESSAGE_ID_EXT) != 0U) { /* Is the frame configured as Extended ID? */
    MSCAN_PDD_SetTxMessageBufferID(MSCAN_BASE_PTR, MSCAN_PDD_BUFFER_ID_EXT, (Frame->MessageID)&~(LDD_CAN_MESSAGE_ID_EXT)); /* Assign extended ID to buffer */
    MSCAN_PDD_EnableTxMessageBufferIDExt(MSCAN_BASE_PTR, PDD_ENABLE); /* Set ID extended */
  } else {
    MSCAN_PDD_SetTxMessageBufferID(MSCAN_BASE_PTR, MSCAN_PDD_BUFFER_ID_STD, Frame->MessageID); /* Assign Standard ID to buffer */
    MSCAN_PDD_EnableTxMessageBufferIDExt(MSCAN_BASE_PTR, PDD_DISABLE); /* Set ID standard */
  }
  MSCAN_PDD_SetTxBufferPriority(MSCAN_BASE_PTR, Frame->LocPriority); /* Set Tx buffer priority */
  if (Frame->FrameType == LDD_CAN_DATA_FRAME) { /* Is it a data frame? */
    for (DataIndex = 0x00U; DataIndex < Frame->Length; DataIndex++) { /* Fill message buffer data array */
      MSCAN_PDD_SetTxMessageBufferData(MSCAN_BASE_PTR, DataIndex, Frame->Data[DataIndex]);
    }
    MSCAN_PDD_SetTxMessageBufferDataLength(MSCAN_BASE_PTR, Frame->Length); /* Set the length of the message */
    if ((Frame->MessageID & LDD_CAN_MESSAGE_ID_EXT) != 0U) { /* Is the frame configured as Extended ID? */
      MSCAN_PDD_EnableTxMessageBufferRTRExtId(MSCAN_BASE_PTR, PDD_DISABLE); /* If yes then set message type as "data frame" */
    } else {
      MSCAN_PDD_EnableTxMessageBufferRTRStdId(MSCAN_BASE_PTR, PDD_DISABLE); /* If no then set message type as "data frame" */
    }
  } else {                             /* Remote frame */
    MSCAN_PDD_SetTxMessageBufferDataLength(MSCAN_BASE_PTR, 0x00U); /* Set the length of the message */
    if ((Frame->MessageID & LDD_CAN_MESSAGE_ID_EXT) != 0U) { /* Is the frame configured as Extended ID? */
      MSCAN_PDD_EnableTxMessageBufferRTRExtId(MSCAN_BASE_PTR, PDD_ENABLE); /* If yes then set message type as "remote frame" */
    } else {
      MSCAN_PDD_EnableTxMessageBufferRTRStdId(MSCAN_BASE_PTR, PDD_ENABLE); /* If no then set message type as "remote frame" */
    }
  }
  Frame->TimeStamp = MSCAN_PDD_GetTxBufferTimeStampValue(MSCAN_BASE_PTR); /* Return Tx timer stamp value */
  MSCAN_PDD_ClearTxMessageBufferFlagMask(MSCAN_BASE_PTR, RemapBufferMask); /* Start transmission */
  DeviceDataPrv->TxBuffersPendingMask |= RemapBufferMask; /* Set pending Tx buffer mask */
  MSCAN_PDD_EnableTxMessageBufferInterruptMask(MSCAN_BASE_PTR, RemapBufferMask); /* Transmitter empty interrupt enable */
  /* {Default RTOS Adapter} Critical section end, general PE function is used */
  ExitCritical();
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  CAN1_ReadFrame (component CAN_LDD)
*/
/*!
**     @brief
**         Reads a frame from the CAN device. The user is informed
**         about CAN reception through OnFullRxBuffer event or
**         GetStateRX method.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         BufferIdx       - Index of the Rx message buffer.
**     @param
**         Frame           - Pointer to the received CAN frame.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_DISABLED - This component is disabled
**                           by user
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_PARAM_RANGE - Value of buffer index is
**                           out of range.
**                           ERR_PARAM_INDEX - Index of message buffer
**                           is not for receive.
**                           ERR_BUSY - CAN module is busy.
**                           ERR_RXEMPTY - The receive buffer is empty.
**                           ERR_OVERRUN - The receive buffer is overrun.
*/
/* ===================================================================*/
LDD_TError CAN1_ReadFrame(LDD_TDeviceData *DeviceDataPtr, LDD_CAN_TMBIndex BufferIdx, LDD_CAN_TFrame *Frame)
{
  LDD_CAN_TBufferMask BufferMask;      /* Bitmask of the requested message buffer */
  uint8_t DataIndex;
  CAN1_TDeviceData *DeviceDataPrv = (CAN1_TDeviceData *)DeviceDataPtr;

  if (BufferIdx > DeviceDataPrv->MaxBufferIndex) { /* Is the number of the required message buffer out of range?*/
    return ERR_PARAM_RANGE;            /* If yes then error */
  }
  BufferMask = (LDD_CAN_TBufferMask)(0x01UL << BufferIdx); /* Calculate bitmask of the requested message buffer */
  if ((BufferMask & DeviceDataPrv->RxBufferMask) == 0U) { /* Is used buffer defined of BufferIdx for receive? */
    return ERR_PARAM_INDEX;            /* If no then error */
  }
  if ((MSCAN_PDD_GetRxStatusInterruptFlags(MSCAN_BASE_PTR) & MSCAN_PDD_RX_BUFFER_FULL_FLAG) == 0U) { /* Is the receive buffer empty? */
    return ERR_RXEMPTY;                /* If yes then error */
  }
  /* {Default RTOS Adapter} Critical section begin, general PE function is used */
  EnterCritical();
  if (MSCAN_PDD_GetRxMessageBufferIDExt(MSCAN_BASE_PTR) == PDD_ENABLE) { /* Extended ID? */
    Frame->MessageID = (LDD_CAN_TMessageID)(MSCAN_PDD_GetRxMessageBufferID(MSCAN_BASE_PTR, MSCAN_PDD_BUFFER_ID_EXT) | (LDD_CAN_TMessageID)LDD_CAN_MESSAGE_ID_EXT);
    Frame->FrameType = (MSCAN_PDD_GetRxMessageBufferRTRExtId(MSCAN_BASE_PTR) != 0U ? LDD_CAN_REMOTE_FRAME : LDD_CAN_DATA_FRAME); /* Result the frame type */
  } else {
    Frame->MessageID = (LDD_CAN_TMessageID)MSCAN_PDD_GetRxMessageBufferID(MSCAN_BASE_PTR, MSCAN_PDD_BUFFER_ID_STD);
    Frame->FrameType = (MSCAN_PDD_GetRxMessageBufferRTRStdId(MSCAN_BASE_PTR) != 0U ? LDD_CAN_REMOTE_FRAME : LDD_CAN_DATA_FRAME); /* Result the frame type */
  }
  Frame->Length = MSCAN_PDD_GetRxMessageBufferDataLength(MSCAN_BASE_PTR); /* Result length of the message */
  Frame->TimeStamp = MSCAN_PDD_GetRxBufferTimeStampValue(MSCAN_BASE_PTR); /* Value time stamp of the message */
  if (Frame->FrameType != LDD_CAN_REMOTE_FRAME) { /* Is it "data frame"? */
    for (DataIndex = 0x00U; DataIndex<Frame->Length; DataIndex++) {
      Frame->Data[DataIndex] = MSCAN_PDD_GetRxMessageBufferData(MSCAN_BASE_PTR, DataIndex); /* Return received data */
    }
  }
  MSCAN_PDD_ClearRxStatusInterruptFlags(MSCAN_BASE_PTR, MSCAN_PDD_RX_BUFFER_FULL_FLAG); /* Release the buffer */
  MSCAN_PDD_EnableRxInterruptsMask(MSCAN_BASE_PTR, MSCAN_PDD_RX_BUFFER_FULL_INT); /* Enable receive full interrupt */
  /* {Default RTOS Adapter} Critical section end, general PE function is used */
  ExitCritical();
  return ERR_OK;
}


/*
** ===================================================================
**     Method      :  CAN1_TxInterrupt (component CAN_LDD)
**
**     Description :
**         Transmit Warning Interrupt
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
PE_ISR(CAN1_TxInterrupt)
{
  /* {Default RTOS Adapter} ISR parameter is passed through the global variable */
  CAN1_TDeviceDataPtr DeviceDataPrv = INT_MSCAN_TX__DEFAULT_RTOS_ISRPARAM;
  { /* Transmit interrupt part begin */
  LDD_CAN_TBufferMask TxBufferMask, BufferMask = 0x01U;
  LDD_CAN_TMBIndex MBIndex;

  TxBufferMask = (LDD_CAN_TBufferMask)(DeviceDataPrv->TxBuffersPendingMask & MSCAN_PDD_GetTxMessageBufferFlagMask(MSCAN_BASE_PTR)); /* Calculate Tx buffer complete interrupt mask */
  if (TxBufferMask != 0U) {
    MSCAN_PDD_DisableTxMessageBufferInterruptMask(MSCAN_BASE_PTR, TxBufferMask); /* Disable Tx empty interrupt */
    DeviceDataPrv->TxBuffersPendingMask &= (LDD_CAN_TBufferMask)~(TxBufferMask); /* Clear used Tx buffer mask */
    for (MBIndex=0x00U; MBIndex<DeviceDataPrv->BuffersNumber; MBIndex++) {
      if ((TxBufferMask & BufferMask) != 0U) {
        CAN1_OnFreeTxBuffer(DeviceDataPrv->UserData, DeviceDataPrv->HwToTxBuffersIdxPtr[MBIndex]); /* Invoke user event */
      }
      BufferMask = (LDD_CAN_TBufferMask)(BufferMask << 0x01U);
    }
  }
  } /* Transmit interrupt part end */
}

/*
** ===================================================================
**     Method      :  CAN1_RxInterrupt (component CAN_LDD)
**
**     Description :
**         Receive Warning Interrupt
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
PE_ISR(CAN1_RxInterrupt)
{
  /* {Default RTOS Adapter} ISR parameter is passed through the global variable */
  CAN1_TDeviceDataPtr DeviceDataPrv = INT_MSCAN_RX__DEFAULT_RTOS_ISRPARAM;
  MSCAN_PDD_DisableRxInterruptsMask(MSCAN_BASE_PTR, MSCAN_PDD_RX_BUFFER_FULL_INT); /* Disable receive full interrupt */
  CAN1_OnFullRxBuffer(DeviceDataPrv->UserData, DeviceDataPrv->HwToRxBuffersIdxPtr[0]); /* Invoke user event */
}

/* END CAN1. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
