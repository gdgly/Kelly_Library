

// Based on PE UART_LLD Logical Device Driver
// Baud rate derived from 20 Mhz bus clock

/*
**         This component "Serial_LDD" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial communication channel.
**     Settings    :
**          Component name                                 : UART0
**          Device                                         : UART0
**          Interrupt service/event                        : Enabled
**            Interrupt RxD                                : INT_UART0
**            Interrupt RxD priority                       : medium priority
**            Interrupt TxD                                : INT_UART0
**            Interrupt TxD priority                       : medium priority
**            Interrupt Error                              : INT_UART0
**            Interrupt Error priority                     : medium priority
**          Settings                                       : 
**            Data width                                   : 8 bits
**            Parity                                       : None
**            Stop bits                                    : 1
**            Loop mode                                    : Normal
**            Baud rate                                    : 9600 baud
**            Wakeup condition                             : Idle line wakeup
**            Stop in wait mode                            : no
**            Idle line mode                               : Starts after start bit
**            Transmitter output                           : Not inverted
**            Receiver input                               : Not inverted
**            Break generation length                      : 10/11 bits
**            Receiver                                     : Enabled
**              RxD                                        : PTA2/KBI0_P2/UART0_RX/I2C0_SDA
**              RxD pin signal                             : UART0_RX
**            Transmitter                                  : Enabled
**              TxD                                        : PTA3/KBI0_P3/UART0_TX/I2C0_SCL
**              TxD pin signal                             : UART0_TX
**            Flow control                                 : None
**          Initialization                                 : 
**            Enabled in init. code                        : yes
**            Auto initialization                          : no
**            Event mask                                   : 
**              OnBlockSent                                : Enabled
**              OnBlockReceived                            : Enabled
**              OnTxComplete                               : Disabled
**              OnError                                    : Enabled
**              OnBreak                                    : Enabled

**     Contents    :
**         Init           - LDD_TDeviceData* UART0_Init(LDD_TUserData *UserDataPtr);
**         SendBlock      - LDD_TError UART0_SendBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData...
**         ReceiveBlock   - LDD_TError UART0_ReceiveBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData...
**         GetError       - LDD_TError UART0_GetError(LDD_TDeviceData *DeviceDataPtr,...
**         SelectBaudRate - LDD_TError UART0_SelectBaudRate(LDD_TDeviceData *DeviceDataPtr,...
**
***/

#include "KEA64/Serial0.h" //include for call backs for now
#include "KEA64/UART0.h"

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "PE_Error.h"
#include "IO_Map.h"

#include "UART_PDD.h"
#include "SIM_PDD.h"

/*! The mask of available events used to enable/disable events during runtime. */
#define AVAILABLE_EVENTS_MASK (LDD_SERIAL_ON_BLOCK_RECEIVED | LDD_SERIAL_ON_BLOCK_SENT | LDD_SERIAL_ON_BREAK | LDD_SERIAL_ON_ERROR)

/* {Default RTOS Adapter} Static object used for simulation of dynamic driver memory allocation */
static UART0_TDeviceData DeviceDataPrv__DEFAULT_RTOS_ALLOC;
/* {Default RTOS Adapter} Global variable used for passing a parameter into ISR */
static UART0_TDeviceDataPtr INT_UART0__DEFAULT_RTOS_ISRPARAM;

/*
** ===================================================================
**     Method      :  UART0_Init (component Serial_LDD)
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
**                         - Device data structure pointer.
*/
/* ===================================================================*/
LDD_TDeviceData* UART0_Init(LDD_TUserData *UserDataPtr)
{
  /* Allocate device structure */
  UART0_TDeviceDataPtr DeviceDataPrv;
  /* {Default RTOS Adapter} Driver memory allocation: Dynamic allocation is simulated by a pointer to the static object */
  DeviceDataPrv = &DeviceDataPrv__DEFAULT_RTOS_ALLOC;

  /* Clear the receive counters and pointer */
  DeviceDataPrv->InpRecvDataNum = 0x00U; /* Clear the counter of received characters */
  DeviceDataPrv->InpDataNumReq = 0x00U; /* Clear the counter of characters to receive by ReceiveBlock() */
  DeviceDataPrv->InpDataPtr = NULL;    /* Clear the buffer pointer for received characters */
  /* Clear the transmit counters and pointer */
  DeviceDataPrv->OutSentDataNum = 0x00U; /* Clear the counter of sent characters */
  DeviceDataPrv->OutDataNumReq = 0x00U; /* Clear the counter of characters to be send by SendBlock() */
  DeviceDataPrv->OutDataPtr = NULL;    /* Clear the buffer pointer for data to be transmitted */
  DeviceDataPrv->UserDataPtr = UserDataPtr; /* Store the RTOS device structure */
  /* Allocate interrupt vectors */
  /* {Default RTOS Adapter} Set interrupt vector: IVT is static, ISR parameter is passed by the global variable */
  INT_UART0__DEFAULT_RTOS_ISRPARAM = DeviceDataPrv;
  /* SIM_SCGC: UART0=1 */
  SIM_SCGC |= SIM_SCGC_UART0_MASK;
  /* SIM_PINSEL: UART0PS=1 */
  SIM_PINSEL |= SIM_PINSEL_UART0PS_MASK;
  /* SIM_SOPT: RXDFE=0 */
  SIM_SOPT &= (uint32_t)~(uint32_t)(SIM_SOPT_RXDFE_MASK);
  /* SIM_PINSEL: UART0PS=1 */
  SIM_PINSEL |= SIM_PINSEL_UART0PS_MASK;
  /* NVIC_IPR3: PRI_12=0x80 */
  NVIC_IPR3 = (uint32_t)((NVIC_IPR3 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_12(0x7F)
              )) | (uint32_t)(
               NVIC_IP_PRI_12(0x80)
              ));
  /* NVIC_ISER: SETENA|=0x1000 */
  NVIC_ISER |= NVIC_ISER_SETENA(0x1000);
  UART_PDD_EnableTransmitter(UART0_BASE_PTR, PDD_DISABLE); /* Disable transmitter. */
  UART_PDD_EnableReceiver(UART0_BASE_PTR, PDD_DISABLE); /* Disable receiver. */
  DeviceDataPrv->BaudDivisorSpeed0 = 0x82u; /* Baudrate divisor in speed 0 mode */
  DeviceDataPrv->BaudMode = UART0_BM_9600BAUD; /* Initial baud rate mode index */
  DeviceDataPrv->SerFlag = 0x00U;      /* Reset flags */
  DeviceDataPrv->ErrFlag = 0x00U;      /* Reset error flags */
  /* UART0_C1: LOOPS=0,UARTSWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  UART0_C1 = 0x00U;                    /*  Set the C1 register */
  /* UART0_C3: R8=0,T8=0,TXDIR=0,TXINV=0,ORIE=0,NEIE=0,FEIE=0,PEIE=0 */
  UART0_C3 = 0x00U;                    /*  Set the C3 register */
  /* UART0_S2: LBKDIF=0,RXEDGIF=0,??=0,RXINV=0,RWUID=0,BRK13=0,LBKDE=0,RAF=0 */
  UART0_S2 = 0x00U;                    /*  Set the S2 register */
  UART_PDD_SetBaudRate(UART0_BASE_PTR, 130U); /* Set the baud rate register. */
  UART_PDD_EnableTransmitter(UART0_BASE_PTR, PDD_ENABLE); /* Enable transmitter */
  UART_PDD_EnableReceiver(UART0_BASE_PTR, PDD_ENABLE); /* Enable receiver */
  UART_PDD_EnableInterrupt(UART0_BASE_PTR, ( UART_PDD_INTERRUPT_RECEIVER | UART_PDD_INTERRUPT_PARITY_ERROR | UART_PDD_INTERRUPT_FRAMING_ERROR | UART_PDD_INTERRUPT_NOISE_ERROR | UART_PDD_INTERRUPT_OVERRUN_ERROR )); /* Enable interrupts */
  /* Registration of the device structure */
  //PE_LDD_RegisterDeviceStructure(PE_LDD_COMPONENT_UART0_ID, DeviceDataPrv);
  return ((LDD_TDeviceData *)DeviceDataPrv);
}

/*
** ===================================================================
**     Method      :  AS1_Deinit (component Serial_LDD)
*/
/*!
**     @brief
**          Deinitializes the device. Switches off the device, frees
**         the device data structure memory, interrupts vectors, etc.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
*/
/* ===================================================================*/
void UART0_DeInit(void)
{
  UART_PDD_DisableInterrupt(UART0_BASE_PTR, ( UART_PDD_INTERRUPT_RECEIVER | UART_PDD_INTERRUPT_TRANSMITTER )); /* Disable interrupts */
  UART_PDD_EnableTransmitter(UART0_BASE_PTR, PDD_DISABLE); /* Disable transmitter. */
  UART_PDD_EnableReceiver(UART0_BASE_PTR, PDD_DISABLE); /* Disable receiver. */
  /* Disable clock gate */
  /* SIM_SCGC: UART0=0 */
  SIM_SCGC &= (uint32_t)~(uint32_t)(SIM_SCGC_UART0_MASK);
}

/*
** ===================================================================
**     Method      :  UART0_ReceiveBlock (component Serial_LDD)
*/
/*!
**     @brief
**         Specifies the number of data to receive. The method returns
**         ERR_BUSY until the specified number of characters is
**         received. Method [CancelBlockReception] can be used to
**         cancel a running receive operation. If a receive operation
**         is not in progress (the method was not called or a previous
**         operation has already finished) all received characters will
**         be lost without any notification. To prevent the loss of
**         data call the method immediately after the last receive
**         operation has finished (e.g. from the [OnBlockReceived]
**         event). This method finishes immediately after calling it -
**         it doesn't wait the end of data reception. Use event
**         [OnBlockReceived] to check the end of data reception or
**         method GetReceivedDataNum to check the state of receiving.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         BufferPtr       - Pointer to a buffer where
**                           received characters will be stored. In case
**                           of 8bit character width each character in
**                           buffer occupies 1 byte. In case of 9 and
**                           more bit long character width each
**                           character in buffer occupies 2 bytes.
**     @param
**         Size            - Number of characters to receive
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - The component does not work in
**                           the active clock configuration.
**                           ERR_PARAM_SIZE - Parameter Size is out of
**                           expected range.
**                           ERR_DISABLED - The component or device is
**                           disabled.
**                           ERR_BUSY - The previous receive request is
**                           pending.
*/
/* ===================================================================*/
LDD_TError UART0_ReceiveBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData *BufferPtr, uint16_t Size)
{
  UART0_TDeviceDataPtr DeviceDataPrv = (UART0_TDeviceDataPtr)DeviceDataPtr;

  if (Size == 0U) {                    /* Is the parameter Size within an expected range? */
    return ERR_PARAM_SIZE;             /* If no then error */
  }
  if (DeviceDataPrv->InpDataNumReq != 0x00U) { /* Is the previous receive operation pending? */
    return ERR_BUSY;                   /* If yes then error */
  }
  /* {Default RTOS Adapter} Critical section begin, general PE function is used */
  EnterCritical();
  DeviceDataPrv->InpDataPtr = (uint8_t*)BufferPtr; /* Store a pointer to the input data. */
  DeviceDataPrv->InpDataNumReq = Size; /* Store a number of characters to be received. */
  DeviceDataPrv->InpRecvDataNum = 0x00U; /* Set number of received characters to zero. */
  /* {Default RTOS Adapter} Critical section end, general PE function is used */
  ExitCritical();
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_SendBlock (component Serial_LDD)
*/
/*!
**     @brief
**         Sends a block of characters. The method returns ERR_BUSY
**         when the previous block transmission is not completed.
**         Method [CancelBlockTransmission] can be used to cancel a
**         transmit operation. This method is available only if the
**         transmitter property is enabled. This method finishes
**         immediately after calling it - it doesn't wait the end of
**         data transmission. Use event [OnBlockSent] to check the end
**         of data transmission or method GetSentDataNum to check the
**         state of sending.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         BufferPtr       - Pointer to a buffer from where
**                           data will be sent. In case of 8bit
**                           character width each character in buffer
**                           occupies 1 byte. In case of 9 and more bit
**                           long character width each character in
**                           buffer occupies 2 bytes.
**     @param
**         Size            - Number of characters in the buffer.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - The component does not work in
**                           the active clock configuration.
**                           ERR_PARAM_SIZE - Parameter Size is out of
**                           expected range.
**                           ERR_DISABLED - The component or device is
**                           disabled.
**                           ERR_BUSY - The previous transmit request is
**                           pending.
*/
/* ===================================================================*/
LDD_TError UART0_SendBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData *BufferPtr, uint16_t Size)
{
  UART0_TDeviceDataPtr DeviceDataPrv = (UART0_TDeviceDataPtr)DeviceDataPtr;

  if (Size == 0U) {                    /* Is the parameter Size within an expected range? */
    return ERR_PARAM_SIZE;             /* If no then error */
  }
  if (DeviceDataPrv->OutDataNumReq != 0x00U) { /* Is the previous transmit operation pending? */
    return ERR_BUSY;                   /* If yes then error */
  }
  /* {Default RTOS Adapter} Critical section begin, general PE function is used */
  EnterCritical();
  DeviceDataPrv->OutDataPtr = (uint8_t*)BufferPtr; /* Set a pointer to the output data. */
  DeviceDataPrv->OutDataNumReq = Size; /* Set the counter of characters to be sent. */
  DeviceDataPrv->OutSentDataNum = 0x00U; /* Clear the counter of sent characters. */
  DeviceDataPrv->SerFlag |= ENABLED_TX_INT; /* Set the flag ENABLED_TX_INT */
  UART_PDD_EnableInterrupt(UART0_BASE_PTR, UART_PDD_INTERRUPT_TRANSMITTER); /* Enable TX interrupt */
  /* {Default RTOS Adapter} Critical section end, general PE function is used */
  ExitCritical();
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  UART0_SelectBaudRate (component Serial_LDD)
*/
/*!
**     @brief
**         This method changes the channel communication speed (baud
**         rate). This method is enabled only if the user specifies a
**         list of possible period settings at design time (see [Timing
**         dialog box] - Runtime setting - from a list of values). Each
**         of these settings constitutes a _/mode/_ and Processor
**         Expert assigns them a _/mode identifier/_. The prescaler and
**         compare values corresponding to each mode are calculated in
**         design time. The user may switch modes at runtime by
**         referring to a mode identifier. No run-time calculations are
**         performed, all the calculations are performed at design time.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         Mode            - Timing mode to set
**                           Note: Special constant is generated in the
**                           components header file for each mode from
**                           the list of values.
**                           This constant can be directly passed to the
**                           parameter. Format of the constant is:
**                           <BeanName>_BM_<Timing> e.g.
**                           "as1_BM_9600BAUD" for baud rate set to 9600
**                           baud and component name "as1". See header
**                           file of the generated code for details.
**     @return
**                         - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - The component does not work in
**                           the active clock configuration.
**                           ERR_DISABLED - The component or device is
**                           disabled.
**                           ERR_PARAM_MODE - Invalid ID of the baud
**                           rate mode.
*/
/* ===================================================================*/
LDD_TError UART0_SelectBaudRate(LDD_TDeviceData *DeviceDataPtr, LDD_SERIAL_TBaudMode Mode)
{
  UART0_TDeviceDataPtr DeviceDataPrv = (UART0_TDeviceDataPtr)DeviceDataPtr;
  static const uint16_t UART0_BaudDivisorSpeed0[0x06] = {0x82u,0x41u,0x21u,0x16u,0x0Bu,0x05u};

  if (Mode >= 0x06U) {                 /* Is mode in baud mode list */
    return ERR_PARAM_MODE ;            /* If no then error */
  }
  DeviceDataPrv->BaudMode = Mode;
  DeviceDataPrv->BaudDivisorSpeed0 = UART0_BaudDivisorSpeed0[Mode]; /* Prescaler in clock configuration 0. */
  UART_PDD_SetBaudRate(UART0_BASE_PTR, DeviceDataPrv->BaudDivisorSpeed0); /* Set the baud rate register in high clock configuration. */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  InterruptRx (component Serial_LDD)
**
**     Description :
**         The method services the receive interrupt of the selected 
**         peripheral(s) and eventually invokes the bean's event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void InterruptRx(UART0_TDeviceDataPtr DeviceDataPrv)
{
  register uint16_t Data;              /* Temporary variable for data */

  Data = (uint16_t)UART_PDD_GetChar8(UART0_BASE_PTR); /* Read an 8-bit character from the receiver */
  if (DeviceDataPrv->InpDataNumReq != 0x00U) { /* Is the receive block operation pending? */
    *(DeviceDataPrv->InpDataPtr++) = (uint8_t)Data; /* Put an 8-bit character to the receive buffer */
    DeviceDataPrv->InpRecvDataNum++;   /* Increment received char. counter */
    if (DeviceDataPrv->InpRecvDataNum == DeviceDataPrv->InpDataNumReq) { /* Is the requested number of characters received? */
      DeviceDataPrv->InpDataNumReq = 0x00U; /* If yes then clear number of requested characters to be received. */
      UART0_OnBlockReceived(DeviceDataPrv->UserDataPtr);
    }
  }
}

/*
** ===================================================================
**     Method      :  InterruptTx (component Serial_LDD)
**
**     Description :
**         The method services the receive interrupt of the selected 
**         peripheral(s) and eventually invokes the bean's event(s).
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void InterruptTx(UART0_TDeviceDataPtr DeviceDataPrv)
{

  if (DeviceDataPrv->OutSentDataNum < DeviceDataPrv->OutDataNumReq) { /* Is number of sent characters less than the number of requested incoming characters? */
    UART_PDD_PutChar8(UART0_BASE_PTR, *(DeviceDataPrv->OutDataPtr++)); /* Put a 8-bit character to the transmit register */
    DeviceDataPrv->OutSentDataNum++;   /* Increment the counter of sent characters. */
    if (DeviceDataPrv->OutSentDataNum == DeviceDataPrv->OutDataNumReq) {
      DeviceDataPrv->OutDataNumReq = 0x00U; /* Clear the counter of characters to be send by SendBlock() */
      UART0_OnBlockSent(DeviceDataPrv->UserDataPtr);
    }
  } else {
    UART_PDD_DisableInterrupt(UART0_BASE_PTR, UART_PDD_INTERRUPT_TRANSMITTER); /* Disable TX interrupt */
    DeviceDataPrv->SerFlag &= (uint16_t)(~(uint16_t)ENABLED_TX_INT); /* Clear the flag ENABLED_TX_INT */
  }
}

/*
** ===================================================================
**     Method      :  UART0_Interrupt (component Serial_LDD)
**
**     Description :
**         The ISR function handling the device receive/transmit 
**         interrupt. Calls InterruptTX/InterruptRX methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
PE_ISR(UART0_ISR)
{
  /* {Default RTOS Adapter} ISR parameter is passed through the global variable */
  UART0_TDeviceDataPtr DeviceDataPrv = INT_UART0__DEFAULT_RTOS_ISRPARAM;
  register uint32_t StatReg = UART_PDD_ReadInterruptStatusReg(UART0_BASE_PTR); /* Read status register */
  register uint16_t OnErrorFlags = 0U; /* Temporary variable for flags */
  register uint8_t  OnBreakFlag = 0U;  /* Temporary variable flag for OnBreak event */
  register uint16_t Data;              /* Temporary variable for data */

  if (StatReg & (UART_S1_NF_MASK | UART_S1_OR_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) { /* Is any error flag set? */
    Data = (uint16_t)UART_PDD_GetChar8(UART0_BASE_PTR); /* Read an 8-bit character from receiver */
    if ((StatReg & UART_S1_FE_MASK) != 0U) { /* Is the framing error detected? */
      if (((StatReg & UART_S1_RDRF_MASK) != 0U) && (Data == 0U)) { /* Is the zero character in the receiver? */
        OnBreakFlag++;
        DeviceDataPrv->SerFlag |= BREAK_DETECTED; /* If yes then set the flag */
      } else {
        OnErrorFlags |= LDD_SERIAL_FRAMING_ERROR; /* If yes then set the flag */
      }
    }
    if ((StatReg & UART_S1_OR_MASK) != 0U) { /* Is the overrun error flag set? */
      OnErrorFlags |= LDD_SERIAL_RX_OVERRUN; /* If yes then set the flag */
    }
    if ((StatReg & UART_S1_PF_MASK) != 0U) { /* Is the parity error flag set? */
      OnErrorFlags |= LDD_SERIAL_PARITY_ERROR; /* If yes then set the flag */
    }
    if ((StatReg & UART_S1_NF_MASK) != 0U) { /* Is the noise error flag set? */
      OnErrorFlags |= LDD_SERIAL_NOISE_ERROR; /* If yes then set the flag */
    }
    DeviceDataPrv->ErrFlag |= OnErrorFlags; /* Copy flags status to ErrFlag status variable */
    StatReg &= (uint32_t)(~(uint32_t)UART_S1_RDRF_MASK); /* Clear the receive data flag to discard the errorneous data */
    if (OnBreakFlag != 0U) {
      UART0_OnBreak(DeviceDataPrv->UserDataPtr); /* If yes then invoke user event */
    } else {
      UART0_OnError(DeviceDataPrv->UserDataPtr); /* Invoke user event */
    }
  }
  if (StatReg & UART_S1_RDRF_MASK) {   /* Is the receiver's interrupt flag set? */
    InterruptRx(DeviceDataPrv);        /* If yes, then invoke the internal service routine. This routine is inlined. */
  }
  if (DeviceDataPrv->SerFlag & ENABLED_TX_INT) { /* Is the transmitter interrupt enabled? */
    if (StatReg & UART_S1_TDRE_MASK) { /* Is the transmitter empty? */
      InterruptTx(DeviceDataPrv);      /* If yes, then invoke the internal service routine. This routine is inlined. */
    }
  }
}

/*
** ===================================================================
**     Method      :  UART0_GetError (component Serial_LDD)
*/
/*!
**     @brief
**         This method returns a set of asserted flags. The flags are
**         accumulated in the set. After calling this method the set is
**         returned and cleared.
**     @param
**         DeviceDataPtr   - Device data structure
**                           pointer returned by [Init] method.
**     @param
**         ErrorPtr        - A pointer to the returned set of
**                           error flags:
**                           LDD_SERIAL_RX_OVERRUN - Receiver overrun.
**                           LDD_SERIAL_PARITY_ERROR - Parity error
**                           (only if HW supports parity feature).
**                           LDD_SERIAL_FRAMING_ERROR - Framing error.
**                           LDD_SERIAL_NOISE_ERROR - Noise error.
**     @return
**                         - Error code (if GetError did not succeed),
**                           possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active clock configuration
**                           ERR_DISABLED - Component is disabled
*/
/* ===================================================================*/
LDD_TError UART0_GetError(LDD_TDeviceData *DeviceDataPtr, LDD_SERIAL_TError *ErrorPtr)
{
  UART0_TDeviceDataPtr DeviceDataPrv = (UART0_TDeviceDataPtr)DeviceDataPtr;

  /* {Default RTOS Adapter} Critical section begin, general PE function is used */
  EnterCritical();
  *ErrorPtr = DeviceDataPrv->ErrFlag;
  DeviceDataPrv->ErrFlag = 0x00U;      /* Reset error flags */
  /* {Default RTOS Adapter} Critical section end, general PE function is used */
  ExitCritical();
  return ERR_OK;                       /* OK */
}
