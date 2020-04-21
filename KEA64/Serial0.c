

// Based on PE AsynchroSerial High Level Component
// Possibly generalize to serial module later. use serial struct and *UART_ functions

/*
**     Abstract    :
**         This component "AsynchroSerial" implements an asynchronous serial
**         communication. The component supports different settings of
**         parity, word width, stop-bit and communication speed,
**         user can select interrupt or polling handler.
**         Communication speed can be changed also in runtime.
**         The component requires one on-chip asynchronous serial channel.
**     Settings    :
**          Component name                                 : Serial0
**          Channel                                        : UART0
**          Interrupt service/event                        : Enabled
**            Interrupt RxD                                : INT_UART0
**            Interrupt RxD priority                       : medium priority
**            Interrupt TxD                                : INT_UART0
**            Interrupt TxD priority                       : medium priority
**            Interrupt Error                              : INT_UART0
**            Interrupt Error priority                     : medium priority
**            Input buffer size                            : 64
**            Output buffer size                           : 64
**            Handshake                                    : 
**              CTS                                        : Disabled
**              RTS                                        : Disabled
**          Settings                                       : 
**            Parity                                       : none
**            Width                                        : 8 bits
**            Stop bit                                     : 1
**            Receiver                                     : Enabled
**              RxD                                        : PTA2/KBI0_P2/UART0_RX/I2C0_SDA
**              RxD pin signal                             : UART0_RX
**            Transmitter                                  : Enabled
**              TxD                                        : PTA3/KBI0_P3/UART0_TX/I2C0_SCL
**              TxD pin signal                             : UART0_TX
**            Baud rate                                    : 9600 baud
**            Break signal                                 : Disabled
**            Wakeup condition                             : Idle line wakeup
**            Transmitter output                           : Not inverted
**            Receiver input                               : Not inverted
**            Stop in wait mode                            : no
**            Idle line mode                               : starts after start bit
**            Break generation length                      : Short

**     Contents    :
**         RecvChar        - byte Serial0_RecvChar(Serial0_TComData *Chr);
**         SendChar        - byte Serial0_SendChar(Serial0_TComData Chr);
**         GetCharsInRxBuf - word Serial0_GetCharsInRxBuf(void);
**         SetBaudRateMode - byte Serial0_SetBaudRateMode(byte Mod);
*/

#include "KEA64/Serial0.h"

#include "PE_Error.h"
#include "IO_Map.h"

#define OVERRUN_ERR      0x01U         /* Overrun error flag bit    */
#define FRAMING_ERR      0x02U         /* Framing error flag bit    */
#define PARITY_ERR       0x04U         /* Parity error flag bit     */
#define CHAR_IN_RX       0x08U         /* Char is in RX buffer      */
#define FULL_TX          0x10U         /* Full transmit buffer      */
#define RUNINT_FROM_TX   0x20U         /* Interrupt is in progress  */
#define FULL_RX          0x40U         /* Full receive buffer       */
#define NOISE_ERR        0x80U         /* Noise error flag bit      */
#define IDLE_ERR         0x0100U       /* Idle character flag bit   */
#define BREAK_ERR        0x0200U       /* Break detect              */
#define COMMON_ERR       0x0800U       /* Common error of RX       */

LDD_TDeviceData *UART0_DeviceDataPtr; /* Device data pointer */
static word SerFlag;                   /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Framing error */
                                       /*       2 - Parity error */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Full TX buffer */
                                       /*       5 - Running int from TX */
                                       /*       6 - Full RX buffer */
                                       /*       7 - Noise error */
                                       /*       8 - Idle character  */
                                       /*       9 - Break detected  */
                                       /*      10 - Unused */
                                       /*      11 - Unused */
static word Serial0_InpLen;              /* Length of input buffer's content */
static word InpIndexR;                 /* Index for reading from input buffer */
static word InpIndexW;                 /* Index for writing to input buffer */
static Serial0_TComData InpBuffer[Serial0_INP_BUF_SIZE]; /* Input buffer for SCI communication */
static Serial0_TComData BufferRead;      /* Input char for SCI communication */
static word Serial0_OutLen;              /* Length of output bufer's content */
static word OutIndexR;                 /* Index for reading from output buffer */
static word OutIndexW;                 /* Index for writing to output buffer */
static Serial0_TComData OutBuffer[Serial0_OUT_BUF_SIZE]; /* Output buffer for SCI communication */

/*
** ===================================================================
**     Method      :  HWEnDi (component AsynchroSerial)
**
**     Description :
**         Enables or disables the peripheral(s) associated with the bean.
**         The method is called automatically as a part of the Enable and 
**         Disable methods and several internal methods.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void HWEnDi(void)
{
  (void)UART0_ReceiveBlock(UART0_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
}

/*
** ===================================================================
**     Method      :  Serial0_RecvChar (component AsynchroSerial)
**     Description :
**         If any data is received, this method returns one character,
**         otherwise it returns an error code (it does not wait for
**         data). This method is enabled only if the receiver property
**         is enabled.
**         [Note:] Because the preferred method to handle error and
**         break exception in the interrupt mode is to use events
**         <OnError> and <OnBreak> the return value ERR_RXEMPTY has
**         higher priority than other error codes. As a consequence the
**         information about an exception in interrupt mode is returned
**         only if there is a valid character ready to be read.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Pointer to a received character
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_RXEMPTY - No data in receiver
**                           ERR_BREAK - Break character is detected
**                           (only when the <Interrupt service> property
**                           is disabled and the <Break signal> property
**                           is enabled)
**                           ERR_COMMON - common error occurred (the
**                           <GetError> method can be used for error
**                           specification)
** ===================================================================
*/
byte Serial0_RecvChar(Serial0_TComData *Chr)
{
  byte Result = ERR_OK;                /* Return error code */

  if (Serial0_InpLen > 0x00U) {          /* Is number of received chars greater than 0? */
    EnterCritical();                   /* Disable global interrupts */
    Serial0_InpLen--;                    /* Decrease number of received chars */
    *Chr = InpBuffer[InpIndexR++];     /* Received char */
    if (InpIndexR >= Serial0_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexR = 0x00U;               /* Set index to the first item into the receive buffer */
    }
    Result = (byte)((SerFlag & (OVERRUN_ERR|COMMON_ERR|FULL_RX))? ERR_COMMON : ERR_OK);
    SerFlag &= (word)~(word)(OVERRUN_ERR|COMMON_ERR|FULL_RX|CHAR_IN_RX); /* Clear all errors in the status variable */
    ExitCritical();                    /* Enable global interrupts */
  } else {
    return ERR_RXEMPTY;                /* Receiver is empty */
  }
  return Result;                       /* Return error code */
}

/*
** ===================================================================
**     Method      :  Serial0_SendChar (component AsynchroSerial)
**     Description :
**         Sends one character to the channel. If the component is
**         temporarily disabled (Disable method) SendChar method only
**         stores data into an output buffer. In case of a zero output
**         buffer size, only one character can be stored. Enabling the
**         component (Enable method) starts the transmission of the
**         stored data. This method is available only if the
**         transmitter property is enabled.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_TXFULL - Transmitter is full
** ===================================================================
*/
byte Serial0_SendChar(Serial0_TComData Chr)
{
  if (Serial0_OutLen == Serial0_OUT_BUF_SIZE) { /* Is number of chars in buffer is the same as a size of the transmit buffer */
    return ERR_TXFULL;                 /* If yes then error */
  }
  EnterCritical();                     /* Disable global interrupts */
  Serial0_OutLen++;                      /* Increase number of bytes in the transmit buffer */
  OutBuffer[OutIndexW++] = Chr;        /* Store char to buffer */
  if (OutIndexW >= Serial0_OUT_BUF_SIZE) { /* Is the pointer out of the transmit buffer */
    OutIndexW = 0x00U;                 /* Set index to first item in the transmit buffer */
  }
  if ((SerFlag & RUNINT_FROM_TX) == 0U) {
    SerFlag |= RUNINT_FROM_TX;         /* Set flag "running int from TX"? */
    (void)UART0_SendBlock(UART0_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
  }
  ExitCritical();                      /* Enable global interrupts */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  Serial0_GetCharsInRxBuf (component AsynchroSerial)
**     Description :
**         Returns the number of characters in the input buffer. This
**         method is available only if the receiver property is enabled.
**     Parameters  : None
**     Returns     :
**         ---             - The number of characters in the input
**                           buffer.
** ===================================================================
*/
word Serial0_GetCharsInRxBuf(void)
{
  return Serial0_InpLen;                 /* Return number of chars in receive buffer */
}

/*
** ===================================================================
**     Method      :  Serial0_SetBaudRateMode (component AsynchroSerial)
**     Description :
**         This method changes the channel communication speed (baud
**         rate). This method can be used only if the user specifies
**         a list of possible period settings at design time (see
**         <Timing dialog box> - Runtime setting - from a list of
**         values). Each of these settings constitutes a _/mode/_
**         and Processor Expert^[TM] assigns them a _/mode
**         identifier/_. The prescaler and compare values
**         corresponding to each mode are calculated at design time.
**         The user may switch modes at runtime by referring to a
**         mode identifier. No run-time calculations are performed,
**         all the calculations are performed at design time.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Mod             - Timing mode to set
**
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
** ===================================================================
*/
byte Serial0_SetBaudRateMode(byte Mod)
{
  LDD_TError Error;

  Error = UART0_SelectBaudRate(UART0_DeviceDataPtr, (LDD_SERIAL_TBaudMode)Mod); /* Set baud rate mode. */
  if (Error == ERR_PARAM_MODE) {
    Error = ERR_VALUE;
  }
  return (byte)Error;
}

/*
** ===================================================================
**     Method      :  Serial0_Init (component AsynchroSerial)
**
**     Description :
**         Initializes the associated peripheral(s) and the bean internal 
**         variables. The method is called automatically as a part of the 
**         application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void Serial0_Init(void)
{
  SerFlag = 0x00U;                     /* Reset flags */
  Serial0_InpLen = 0x00U;                /* No char in the receive buffer */
  InpIndexR = 0x00U;                   /* Set index on the first item in the receive buffer */
  InpIndexW = 0x00U;
  Serial0_OutLen = 0x00U;                /* No char in the transmit buffer */
  OutIndexR = 0x00U;                   /* Set index on the first item in the transmit buffer */
  OutIndexW = 0x00U;
  UART0_DeviceDataPtr = UART0_Init(NULL); /* Calling init method of the inherited component */
  HWEnDi();                            /* Enable/disable device according to status flags */
}

#define ON_ERROR    0x01U
#define ON_FULL_RX  0x02U
#define ON_RX_CHAR  0x04U
/*
** ===================================================================
**     Method      :  Serial0_UART0_OnBlockReceived (component AsynchroSerial)
**
**     Description :
**         This event is called when the requested number of data is 
**         moved to the input buffer.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_OnBlockReceived(LDD_TUserData *UserDataPtr)
{

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  if (Serial0_InpLen < Serial0_INP_BUF_SIZE) { /* Is number of bytes in the receive buffer lower than size of buffer? */
    Serial0_InpLen++;                    /* Increase number of chars in the receive buffer */
    InpBuffer[InpIndexW++] = (Serial0_TComData)BufferRead; /* Save received char to the receive buffer */
    if (InpIndexW >= Serial0_INP_BUF_SIZE) { /* Is the index out of the receive buffer? */
      InpIndexW = 0x00U;               /* Set index on the first item into the receive buffer */
    }
  } else {
    SerFlag |= FULL_RX;                /* Set flag "full RX buffer" */
  }
  (void)UART0_ReceiveBlock(UART0_DeviceDataPtr, &BufferRead, 1U); /* Receive one data byte */
}

#define ON_FREE_TX  0x01U
#define ON_TX_CHAR  0x02U
/*
** ===================================================================
**     Method      :  Serial0_UART0_OnBlockSent (component AsynchroSerial)
**
**     Description :
**         This event is called after the last character from the output 
**         buffer is moved to the transmitter.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_OnBlockSent(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  OutIndexR++;
  if (OutIndexR >= Serial0_OUT_BUF_SIZE) { /* Is the index out of the transmit buffer? */
    OutIndexR = 0x00U;                 /* Set index on the first item into the transmit buffer */
  }
  Serial0_OutLen--;                      /* Decrease number of chars in the transmit buffer */
  if (Serial0_OutLen != 0U) {            /* Is number of bytes in the transmit buffer greater then 0? */
    SerFlag |= RUNINT_FROM_TX;         /* Set flag "running int from TX"? */
    (void)UART0_SendBlock(UART0_DeviceDataPtr, (LDD_TData *)&OutBuffer[OutIndexR], 1U); /* Send one data byte */
  } else {
    SerFlag &= (byte)~(RUNINT_FROM_TX); /* Clear "running int from TX" and "full TX buff" flags */
  }
}

/*
** ===================================================================
**     Method      :  Serial0_UART0_OnError (component AsynchroSerial)
**
**     Description :
**         This event is called when a channel error (not the error 
**         returned by a given method) occurs.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_OnError(LDD_TUserData *UserDataPtr)
{
  LDD_SERIAL_TError SerialErrorMask;   /* Serial error mask variable */

  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  (void)UART0_GetError(UART0_DeviceDataPtr, &SerialErrorMask); /* Get error state */
  if (SerialErrorMask != 0U) {
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_PARITY_ERROR) != 0U ) ? PARITY_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_NOISE_ERROR) != 0U ) ? NOISE_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_RX_OVERRUN) != 0U ) ? OVERRUN_ERR : 0U);
    SerFlag |= (((SerialErrorMask & LDD_SERIAL_FRAMING_ERROR) != 0U ) ? FRAMING_ERR : 0U);
  }
}

/*
** ===================================================================
**     Method      :  Serial0_UART0_OnBreak (component AsynchroSerial)
**
**     Description :
**         This event is called when a break occurs on the input channel.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_OnBreak(LDD_TUserData *UserDataPtr)
{
  (void)UserDataPtr;                   /* Parameter is not used, suppress unused argument warning */
  SerFlag |= FRAMING_ERR;              /* Set framing error flag */
}

