#ifndef _TERMINAL_
#define _TERMINAL_

#include <stdint.h>
#include <stdbool.h>

/* Color constants */
#define   clBlack   30U                /* Black color */
#define   clRed     31U                /* Red color */
#define   clGreen   32U                /* Green color */
#define   clYellow  33U                /* Yellow color */
#define   clBlue    34U                /* Blue color */
#define   clMagenta 35U                /* Magenta color */
#define   clCyan    36U                /* Cyan color */
#define   clWhite   37U                /* White color */

void Terminal_ReadLine(void *str);
void Terminal_SendChar(uint8_t Val);
void Terminal_ReadChar(uint8_t *c);
bool Terminal_KeyPressed(void);
void Terminal_CRLF(void);
void Terminal_SendStr(void *str);
void Terminal_SendNum(int32_t number);
uint8_t Terminal_SendFloatNum(float number);
void Terminal_Cls(void);
void Terminal_CursorUp(uint8_t lines);
void Terminal_CursorDown(uint8_t lines);
void Terminal_CursorLeft(uint8_t columns);
void Terminal_CursorRight(uint8_t columns);
void Terminal_MoveTo(uint8_t x, uint8_t y);
void Terminal_SetColor(uint8_t foreground, uint8_t background);
void Terminal_EraseLine(void);

#endif
