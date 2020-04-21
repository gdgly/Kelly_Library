#ifndef __SERIAL0_
#define __SERIAL0_

#include "KEA64/UART0.h"

#include "Cpu.h"
#include "PE_Types.h"

#define Serial0_BM_9600BAUD         0    /* Baud rate constant for switch to the baud rate mode 0 */
#define Serial0_BM_19200BAUD        1    /* Baud rate constant for switch to the baud rate mode 1 */
#define Serial0_BM_38400BAUD        2    /* Baud rate constant for switch to the baud rate mode 2 */
#define Serial0_BM_57600BAUD        3    /* Baud rate constant for switch to the baud rate mode 3 */
#define Serial0_BM_115200BAUD       4    /* Baud rate constant for switch to the baud rate mode 4 */
#define Serial0_BM_250000BAUD       5    /* Baud rate constant for switch to the baud rate mode 5 */


#ifndef __BWUserType_Serial0_TError
#define __BWUserType_Serial0_TError
  typedef union {
  byte err;
  struct {
    bool OverRun  : 1;                 /* Overrun error flag */
    bool Framing  : 1;                 /* Framing error flag */
    bool Parity   : 1;                 /* Parity error flag */
    bool RxBufOvf : 1;                 /* Rx buffer full error flag */
    bool Noise    : 1;                 /* Noise error flag */
    bool Break    : 1;                 /* Break detect */
    bool LINSync  : 1;                 /* LIN synchronization error */
    bool BitError  : 1;                /* Bit error flag - mismatch to the expected value happened. */
  } errName;
} Serial0_TError;                        /* Error flags. For languages which don't support bit access is byte access only to error flags possible. */
#endif

#ifndef __BWUserType_Serial0_TComData
#define __BWUserType_Serial0_TComData
  typedef byte Serial0_TComData;         /* User type for communication. Size of this type depends on the communication data witdh */
#endif

#define Serial0_INP_BUF_SIZE  0x40U      /* Length of the RX buffer */
#define Serial0_OUT_BUF_SIZE  0x40U      /* Length of the TX buffer */

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
byte Serial0_RecvChar(Serial0_TComData *Chr);

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
byte Serial0_SendChar(Serial0_TComData Chr);

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
word Serial0_GetCharsInRxBuf(void);

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
byte Serial0_SetBaudRateMode(byte Mod);

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
void Serial0_Init(void);

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
void UART0_OnBlockReceived(LDD_TUserData *UserDataPtr);

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
void UART0_OnBlockSent(LDD_TUserData *UserDataPtr);

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
void UART0_OnError(LDD_TUserData *UserDataPtr);

/*
** ===================================================================
**     Method      :  Serial0_UART0_OnBreak (component AsynchroSerial)
**
**     Description :
**         This event is called when a break occurs on the input channel.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void UART0_OnBreak(LDD_TUserData *UserDataPtr);


#endif 
