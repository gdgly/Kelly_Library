// wrapper of serial Tx Rx chars for terminal use case.
// based on terminal component provided by PE

/*
**         CRLF         - void Terminal_CRLF(void);
**         SendStr      - void Terminal_SendStr(uint8_t *str);
**         SendNum      - void Terminal_SendNum(int32_t number);
**         SendFloatNum - uint8_t Terminal_SendFloatNum(float number);
**         SendChar     - void Terminal_SendChar(uint8_t Val);
**         Cls          - void Terminal_Cls(void);
**         CursorUp     - void Terminal_CursorUp(uint8_t lines);
**         CursorDown   - void Terminal_CursorDown(uint8_t lines);
**         CursorLeft   - void Terminal_CursorLeft(uint8_t columns);
**         CursorRight  - void Terminal_CursorRight(uint8_t columns);
**         MoveTo       - void Terminal_MoveTo(uint8_t x, uint8_t y);
**         SetColor     - void Terminal_SetColor(uint8_t foreground, uint8_t background);
**         EraseLine    - void Terminal_EraseLine(void);
**         ReadChar     - void Terminal_ReadChar(uint8_t *c);
**         KeyPressed   - bool Terminal_KeyPressed(void);
**         ReadLine     - void Terminal_ReadLine(uint8_t *str);
*/

#include "Terminal.h"

#include <stddef.h>

#define COLUMNS  80U                   /* Number of columns */
#define ROWS     24U                   /* Number of rows */

//todo remove
#define ERR_OK                          0x00U /*!< OK */
#define ERR_SPEED                       0x01U /*!< This device does not work in the active speed mode. */
#define ERR_RANGE                       0x02U /*!< Parameter out of range. */
#define ERR_VALUE                       0x03U /*!< Parameter of incorrect value. */
#define ERR_OVERFLOW                    0x04U /*!< Timer overflow. */
#define ERR_MATH                        0x05U /*!< Overflow during evaluation. */
#define ERR_ENABLED                     0x06U /*!< Device is enabled. */
#define ERR_DISABLED                    0x07U /*!< Device is disabled. */
#define ERR_BUSY                        0x08U /*!< Device is busy. */
#define ERR_NOTAVAIL                    0x09U /*!< Requested value or method not available. */
#define ERR_RXEMPTY                     0x0AU /*!< No data in receiver. */
#define ERR_TXFULL                      0x0BU /*!< Transmitter is full. */
#define ERR_BUSOFF                      0x0CU /*!< Bus not available. */
#define ERR_OVERRUN                     0x0DU /*!< Overrun error is detected. */
#define ERR_FRAMING                     0x0EU /*!< Framing error is detected. */
#define ERR_PARITY                      0x0FU /*!< Parity error is detected. */

static uint8_t (*Serial_RecvChar)(uint8_t *Chr);
static uint8_t (*Serial_SendChar)(uint8_t Chr);
static uint16_t (*Serial_GetCharsInRxBuf)(void);

void Terminal_Init(uint8_t (*recvChar)(uint8_t *Chr), uint8_t (*sendChar)(uint8_t Chr), uint16_t (*getCharsInRxBuf)(void))
{
	Serial_RecvChar = recvChar;
	Serial_SendChar = sendChar;
	Serial_GetCharsInRxBuf = getCharsInRxBuf;
}

/*
** ===================================================================
**     Method      :  Terminal_SendESCPrefix (component Term)
**
**     Description :
**         The method sends first (common) part of an excape sequence.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void SendESCPrefix(void)
{
  while (Serial_SendChar(0x1BU) == ERR_TXFULL){} /* Send the part of the escape sequence ('ESC') */
  while (Serial_SendChar(0x5BU) == ERR_TXFULL){} /* Send the part of the escape sequence ('[') */
}

/*
** ===================================================================
**     Method      :  Terminal_LongToStr (component Term)
**
**     Description :
**         The method converts long int number to string.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static void LongToStr(uint8_t* s, int32_t n)
{
  uint8_t i=0U, j;
  uint8_t tmp;
  uint8_t Tmp;
  bool sign = (bool)((n < 0) ? true : false);
  uint32_t u_n;

  u_n = (uint32_t)n;
  if (sign){
    u_n = (u_n ^ 0xFFFFFFFFUL) + 1U;
  }
  if (u_n == 0U){
    s[i++] = '0';
  }
  while (u_n > 0U) {
    Tmp = (uint8_t)((u_n % 10U) + 0x30U);
    s[i++] = (uint8_t)(Tmp);
    u_n /= 10U;
  }
  if (sign){
    s[i++] = '-';
  }
  for(j=0U; j<(i/2U); j++) {
    tmp = s[j];
    s[j] = s[(i-j)-1U];
    s[(i-j)-1U] = tmp;
  }
  s[i] = '\0';
}

/*
** ===================================================================
**     Method      :  Terminal_FloatToStr (component Term)
**
**     Description :
**         The method converts float number to string.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
static uint8_t FloatToStr(uint8_t* s, float n)
{
  uint8_t tmp;
  uint8_t i=4U, j;
  bool sign = (bool)((n < 0.0F) ? true : false);
  uint32_t Integral;
  uint16_t fract;
  float tmp_f;
  uint8_t Tmp;

  if (sign){
    n *= -1.0F;
  }
  if (n > 4294967295.0F) {
    return ERR_RANGE;
  }
  Integral = (uint32_t)n;
  tmp_f = (n-(float)Integral)*10000.0F;
  fract = (uint16_t)tmp_f;
  for(j=0U; j<4U; j++) {
    Tmp = (uint8_t)((fract % 10U) + 0x30U);
    s[j] = (uint8_t)(Tmp);
    fract /= 10U;
  }
  s[i++] = '.';
  if (i > 14U) {
    return ERR_RANGE;
  }
  if (Integral == 0U){
    s[i++] = '0';
    if (i > 14U) {
      return ERR_RANGE;
    }
  }
  while (Integral > 0U) {
    Tmp = (uint8_t)((Integral % 10U) + 0x30U);
    s[i++] = (uint8_t)(Tmp);
    if (i > 14U) {
      return ERR_RANGE;
    }
    Integral /= 10U;
  }
  if (sign){
    s[i++] = '-';
    if (i > 14U) {
      return ERR_RANGE;
    }
  }
  for(j=0U; j<(i/2U); j++) {
    tmp = s[j];
    s[j] = s[(i-j)-1U];
    s[(i-j)-1U] = tmp;
  }
  s[i] = '\0';
  return ERR_OK;
}

/*
** ===================================================================
**     Method      :  Terminal_ReadLine (component Term)
**     Description :
**         Read string from the terminal. Waits until whole string is
**         read. Method expects carriage return and line feed (CRLF)
**         escape sequence as a string termination.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * str             - Pointer to an user string buffer.
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_ReadLine(void *str)
{
  uint8_t *ptr =  ((uint8_t *) str);
  uint8_t c;

  do {                                 /* While not CR */
    while(Serial_RecvChar(&c) != ERR_OK){} /* Read character from terminal */
    *ptr++ = c;                        /* Append character to the string */
  } while ((c != 0x0DU) && (c != 0x0AU));
  *--ptr = 0x00U;                      /* Null terminated string */
}

/*
** ===================================================================
**     Method      :  Terminal_SendChar (component Term)
**     Description :
**         Send char to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Val             - Char to send
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_SendChar(uint8_t Val)
{
  while (Serial_SendChar((uint8_t)Val) == ERR_TXFULL){} /* Send char */
}


/*
** ===================================================================
**     Method      :  Terminal_ReadChar (component Term)
**     Description :
**         Read char from terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * c               - Pointer to a char variable
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_ReadChar(uint8_t *c)
{
  while(Serial_RecvChar((uint8_t *)c) != ERR_OK){} /* Receive char */
}

/*
** ===================================================================
**     Method      :  Terminal_KeyPressed (component Term)
**     Description :
**         Test if char was received from the terminal.
**     Parameters  : None
**     Returns     :
**         ---             - Return <true> if user pressed some key
** ===================================================================
*/
bool Terminal_KeyPressed(void)
{
  return (bool)((Serial_GetCharsInRxBuf() == 0U) ? false : true); /* Result number of chars in receive buffer */
}

/*
** ===================================================================
**     Method      :  Terminal_CRLF (component Term)
**     Description :
**         Send CRLF to terminal.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_CRLF(void)
{
  while (Serial_SendChar(0x0DU) == ERR_TXFULL){} /* Send the part of the escape sequence ('CR') */
  while (Serial_SendChar(0x0AU) == ERR_TXFULL){} /* Send the part of the escape sequence ('LF') */
}

/*
** ===================================================================
**     Method      :  Terminal_SendStr (component Term)
**     Description :
**         Send string to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * str             - Pointer to a string variable
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_SendStr(void *str)
{
  uint8_t *ptr =  ((uint8_t *) str);
  while (*ptr != 0x00U) {
    while (Serial_SendChar(*ptr) == ERR_TXFULL){} /* Send char */
    ptr++;                             /* Increment the pointer */
  }
}



/*
** ===================================================================
**     Method      :  Terminal_SendNum (component Term)
**     Description :
**         Send number to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         number          - Long number
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_SendNum(int32_t number)
{
  uint8_t str[15];                      /* Temporary variable */
  uint8_t i=0U;                        /* Temporary variable */

  LongToStr(str, number);              /* Conversion number to the string */
  while (str[i] != '\0') {
    while (Serial_SendChar(str[i]) == ERR_TXFULL){} /* Send char */
    i++;                               /* Increase the variable */
  }
}

/*
** ===================================================================
**     Method      :  Terminal_SendFloatNum (component Term)
**     Description :
**         Send a float number to the terminal. Due to the
**         transformations the maximum float number is limited
**         according to the following conditions: 
**         - positive number: 9 digits for integer part with 4 digits
**         for fractional part. 
**         - negative numbers: 8 digits for integer part with 4 digits
**         for fractional part.
**     Parameters  :
**         NAME            - DESCRIPTION
**         number          - Float variable
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_RANGE - Float number exceeds maximal
**                           number limitation.
** ===================================================================
*/
uint8_t Terminal_SendFloatNum(float number)
{
  uint8_t str[15];
  uint8_t i=0U;
  uint8_t err;

  err = FloatToStr(str, number);       /* Conversion number to the string */
  if (err == ERR_RANGE) {
    return ERR_RANGE;
  }
  while (str[i] != '\0') {
    while (Serial_SendChar(str[i]) == ERR_TXFULL){} /* Send char */
    i++;                               /* Increase the variable */
  }
  return(err);
}

/*
** ===================================================================
**     Method      :  Terminal_Cls (component Term)
**     Description :
**         Clear the terminal window.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_Cls(void)
{
  SendESCPrefix();
  while (Serial_SendChar(0x32U) == ERR_TXFULL){} /* Send the part of the escape sequence ('2') */
  while (Serial_SendChar(0x4AU) == ERR_TXFULL){} /* Send the part of the escape sequence ('J') */
}

/*
** ===================================================================
**     Method      :  Terminal_CursorUp (component Term)
**     Description :
**         Move cursor up.
**     Parameters  :
**         NAME            - DESCRIPTION
**         lines           - Number of rows for shift cursor up.
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_CursorUp(uint8_t lines)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Terminal_SendNum((int32_t) lines);      /* Send number of lines */
  while (Serial_SendChar(0x41U) == ERR_TXFULL){} /* Send the part of the escape sequence ('A') */
}

/*
** ===================================================================
**     Method      :  Terminal_CursorDown (component Term)
**     Description :
**         Move cursor down.
**     Parameters  :
**         NAME            - DESCRIPTION
**         lines           - Number of rows for shift cursor down..
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_CursorDown(uint8_t lines)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Terminal_SendNum((int32_t) lines);      /* Send number of lines */
  while (Serial_SendChar(0x42U) == ERR_TXFULL){} /* Send the part of the escape sequence ('B') */
}

/*
** ===================================================================
**     Method      :  Terminal_CursorLeft (component Term)
**     Description :
**         Move cursor left.
**     Parameters  :
**         NAME            - DESCRIPTION
**         columns         - Number of columns for shift cursor
**                           left
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_CursorLeft(uint8_t columns)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Terminal_SendNum((int32_t) columns);    /* Send number of columns */
  while (Serial_SendChar(0x44U) == ERR_TXFULL){} /* Send the part of the escape sequence ('D') */
}

/*
** ===================================================================
**     Method      :  Terminal_CursorRight (component Term)
**     Description :
**         Move cursor right.
**     Parameters  :
**         NAME            - DESCRIPTION
**         columns         - Number of columns for shift cursor
**                           right
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_CursorRight(uint8_t columns)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Terminal_SendNum((int32_t) columns);    /* Send number of columns */
  while (Serial_SendChar(0x43U) == ERR_TXFULL){} /* Send the part of the escape sequence ('C') */
}

/*
** ===================================================================
**     Method      :  Terminal_MoveTo (component Term)
**     Description :
**         Move cursor to position x, y. Range of coordinates can be
**         from 1 to the size, which depends on the used terminal.
**         Position [1,1] represents the upper left corner.
**     Parameters  :
**         NAME            - DESCRIPTION
**         x               - Column number
**         y               - Row number
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_MoveTo(uint8_t x,uint8_t y)
{
  SendESCPrefix();                     /* Send the escape prefix */
  Terminal_SendNum((int32_t) y);          /* Send number of line */
  while (Serial_SendChar(0x3BU) == ERR_TXFULL){} /* Send the part of the escape sequence (';') */
  Terminal_SendNum((int32_t) x);          /* Send number of column */
  while (Serial_SendChar(0x48U) == ERR_TXFULL){} /* Send the part of the escape sequence ('H') */
}

/*
** ===================================================================
**     Method      :  Terminal_SetColor (component Term)
**     Description :
**         Set foreground and background color. The following color
**         constant are defined in the header file:
**         clBlack - Black color
**         clRed - Red color
**         clGreen - Green color
**         clYellow - Yellow color
**         clBlue - Blue color
**         clMagenta - Magenta color
**         clCyan - Cyan color
**         clWhite - White color
**     Parameters  :
**         NAME            - DESCRIPTION
**         foreground      - Foreground color.
**         background      - Background color.
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_SetColor(uint8_t foreground, uint8_t background)
{
  SendESCPrefix();                     /* Send the escape prefix */
  while (Serial_SendChar(0x30U) == ERR_TXFULL){} /* Send text atributes ('0') */
  while (Serial_SendChar(0x3BU) == ERR_TXFULL){} /* Send the part of the escape sequence (';') */
  Terminal_SendNum((int32_t)foreground);  /* Send the foreground color */
  while (Serial_SendChar(0x3BU) == ERR_TXFULL){} /* Send the part of the escape sequence (';') */
  Terminal_SendNum((int32_t)background+10); /* Send the background color */
  while (Serial_SendChar(0x6DU) == ERR_TXFULL){} /* Send the part of the escape sequence ('m') */
}

/*
** ===================================================================
**     Method      :  Terminal_EraseLine (component Term)
**     Description :
**         Clears the current line from the cursor position to the end
**         of line.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Terminal_EraseLine(void)
{
  SendESCPrefix();                     /* Send the escape prefix */
  while (Serial_SendChar(0x4BU) == ERR_TXFULL){} /* Send the part of the escape sequence ('K') */
}

