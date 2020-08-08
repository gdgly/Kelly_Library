/* ###################################################################
**     Filename    : Events.h
**     Project     : KBS
**     Processor   : SKEAZN64MLH2
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2014-07-22, 17:04, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"


#ifdef __cplusplus
extern "C" {
#endif 

/* END Events */
extern void Init_IRQs (uint8_t IRQ_Num,uint8_t Pri);
extern void	ISR_Timer(void);
extern void Vector_Control(void);
extern void Get_ATD_Data(uint32_t TMIR_En);

#ifdef __cplusplus
}  /* extern "C" */
#endif 



#endif 
/* ifndef __Events_H*/
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
