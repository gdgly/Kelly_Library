/*
 * typedefine.h
 *
 *  Created on: 2017Äê8ÔÂ21ÈÕ
 *      Author: ted
 */

#ifndef TYPEDEFINE_H_
#define TYPEDEFINE_H_

/* PE types definition */
typedef unsigned char           bool;
typedef unsigned char           byte;
typedef unsigned short          word;
typedef unsigned long           dword;
typedef unsigned long long      dlong;

/* Other basic data types */
typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed long int         int32_t;

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned long int       uint32_t;

#define CAL_DATA_SIZE  512
#define CAL_RAM_SIZE												1024

#define CAL_SECTION		 __attribute__((section(".cal_rodata")))
#define INT_SECTION		 __attribute__((section(".int_text")))
#define APP_SECTION		 __attribute__((section(".code_text")))
#define CAL_RAMSCT		 __attribute__((section(".calram")))

#define _EI() __asm volatile ("cpsie i" : : : "memory")
#define _DI() __asm volatile ("cpsid i" : : : "memory")

#endif /* TYPEDEFINE_H_ */
