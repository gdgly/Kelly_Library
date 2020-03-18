/*!
**	This component generates low-level methods for redirecting console I/O to the selected UART.
**	These methods are typically used by printf()/scanf() methods.
*/

#include "IO_Map.h"
#include "stdio.h"
#include <stdarg.h>
#include "UART_PDD.h"

/*
** ===================================================================
**     Method      :  CsIO1__read (component ConsoleIO)
**
**     Description :
**         _read
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
int _read (int fd, const void *buf, size_t count)
{
  size_t CharCnt = 0x00;

  (void)fd;                            /* Parameter is not used, suppress unused argument warning */
  for (;count > 0x00; --count) {
    if ((UART_PDD_ReadInterruptStatusReg(UART0_BASE_PTR) & UART_S1_RDRF_MASK) == 0x00) { /* Any data in receiver buffer */
      if (CharCnt != 0x00) {           /* No, at least one char received? */
        break;                         /* Yes, return received char(s) */
      } else {                         /* Wait until a char is received */
        while ((UART_PDD_ReadInterruptStatusReg(UART0_BASE_PTR) & UART_S1_RDRF_MASK) == 0x00) {};
      }
    }
    CharCnt++;                         /* Increase char counter */
    /* Save character received by UARTx device into the receive buffer */
    *(uint8_t*)buf = (unsigned char)UART_PDD_GetChar8(UART0_BASE_PTR);
    /* Stop reading if CR (Ox0D) character is received */
    if (*(uint8_t*)buf == 0x0DU) {     /* New line character (CR) received ? */
      *(uint8_t*)buf = '\n';           /* Yes, convert LF to '\n' char. */
      break;                           /* Stop loop and return received char(s) */
    }
    (uint8_t*)buf++;                   /* Increase buffer pointer */
  }
  return CharCnt;
}

/*
** ===================================================================
**     Method      :  CsIO1__write (component ConsoleIO)
**
**     Description :
**         _write
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
int _write (int fd, const void *buf, size_t count)
{
  size_t CharCnt = 0x00;

  (void)fd;                            /* Parameter is not used, suppress unused argument warning */
  for (;count > 0x00; --count) {
    /* Wait until UART is ready for saving a next character into the transmit buffer */
    while ((UART_PDD_ReadInterruptStatusReg(UART0_BASE_PTR) & UART_S1_TDRE_MASK) == 0) {};
    if (*(uint8_t*)buf == '\n') {
      /* Send '\r'(0x0D) before each '\n'(0x0A). */
      /* Save a character into the transmit buffer of the UART0 device */
      UART_PDD_PutChar8(UART0_BASE_PTR, 0x0DU);
      /* Wait until UART is ready for saving a next character into the transmit buffer */
      while ((UART_PDD_ReadInterruptStatusReg(UART0_BASE_PTR) & UART_S1_TDRE_MASK) == 0) {};
    }
    /* Save a character into the transmit buffer of the UART0 device */
    UART_PDD_PutChar8(UART0_BASE_PTR, (unsigned char)*(uint8_t*)buf);
    (uint8_t*)buf++;                   /* Increase buffer pointer */
    CharCnt++;                         /* Increase char counter */
  }
  return CharCnt;
}
