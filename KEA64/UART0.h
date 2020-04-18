#ifndef _UART0_H_
#define _UART0_H_

#define UART0_BM_9600BAUD   0u   /* Baud rate constant for switch to the baud rate mode 0 */
#define UART0_BM_19200BAUD  1u   /* Baud rate constant for switch to the baud rate mode 1 */
#define UART0_BM_38400BAUD  2u   /* Baud rate constant for switch to the baud rate mode 2 */
#define UART0_BM_57600BAUD  3u   /* Baud rate constant for switch to the baud rate mode 3 */
#define UART0_BM_115200BAUD 4u   /* Baud rate constant for switch to the baud rate mode 4 */
#define UART0_BM_250000BAUD 5u   /* Baud rate constant for switch to the baud rate mode 5 */


#include "Cpu.h"
#include "PE_Types.h"


/*! Peripheral base address of a device allocated by the component. This constant can be used directly in PDD macros. */
#define UART0_PRPH_BASE_ADDRESS  0x4006A000U
  
/* Methods configuration constants - generated for all enabled component's methods */
#define UART0_Init_METHOD_ENABLED /*!< Init method of the component UART0 is enabled (generated) */
#define UART0_SendBlock_METHOD_ENABLED /*!< SendBlock method of the component UART0 is enabled (generated) */
#define UART0_ReceiveBlock_METHOD_ENABLED /*!< ReceiveBlock method of the component UART0 is enabled (generated) */
#define UART0_GetError_METHOD_ENABLED /*!< GetError method of the component UART0 is enabled (generated) */
#define UART0_SelectBaudRate_METHOD_ENABLED /*!< SelectBaudRate method of the component UART0 is enabled (generated) */

/* Events configuration constants - generated for all enabled component's events */
#define UART0_OnBlockReceived_EVENT_ENABLED /*!< OnBlockReceived event of the component UART0 is enabled (generated) */
#define UART0_OnBlockSent_EVENT_ENABLED /*!< OnBlockSent event of the component UART0 is enabled (generated) */
#define UART0_OnBreak_EVENT_ENABLED /*!< OnBreak event of the component UART0 is enabled (generated) */
#define UART0_OnError_EVENT_ENABLED /*!< OnError event of the component UART0 is enabled (generated) */

#define ENABLED_TX_INT        0x01U    /*!< TX interrupt enabled      */
#define BREAK_DETECTED        0x02U    /*!< Break detected            */
#define TX_COMPLETED          0x04U    /*!< Transmission completed    */
#define ENABLE_TX_COMPLETE    0x10U    /*!< Enable/Disable of TX complete detection. Used in the polling mode only */

/*! Device data structure type */
typedef struct {
  uint16_t SerFlag;                    /*!< Flags for serial communication */
  LDD_SERIAL_TBaudMode BaudMode;       /*!< Baud rate mode */
  LDD_SERIAL_TError ErrFlag;           /*!< Error flags mirror of SerFlag */
  uint16_t BaudDivisorSpeed0;          /*!< Baudrate prescaler in speed 0 mode */
  uint16_t InpRecvDataNum;             /*!< The counter of received characters */
  uint8_t *InpDataPtr;                 /*!< The buffer pointer for received characters */
  uint16_t InpDataNumReq;              /*!< The counter of characters to receive by ReceiveBlock() */
  uint16_t OutSentDataNum;             /*!< The counter of sent characters */
  uint8_t *OutDataPtr;                 /*!< The buffer pointer for data to be transmitted */
  uint16_t OutDataNumReq;              /*!< The counter of characters to be send by SendBlock() */
  LDD_TUserData *UserDataPtr;          /*!< Pointer to user data */
} UART0_TDeviceData;

typedef UART0_TDeviceData *UART0_TDeviceDataPtr ; /*!< Pointer to the device data structure. */

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
LDD_TDeviceData* UART0_Init(LDD_TUserData *UserDataPtr);

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
LDD_TError UART0_ReceiveBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData *BufferPtr, uint16_t Size);

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
LDD_TError UART0_SendBlock(LDD_TDeviceData *DeviceDataPtr, LDD_TData *BufferPtr, uint16_t Size);

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
LDD_TError UART0_SelectBaudRate(LDD_TDeviceData *DeviceDataPtr, LDD_SERIAL_TBaudMode Mode);


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
LDD_TError UART0_GetError(LDD_TDeviceData *DeviceDataPtr, LDD_SERIAL_TError *ErrorPtr);


#endif
