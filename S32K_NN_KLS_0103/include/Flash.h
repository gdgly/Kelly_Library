/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file flash.h
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun. 25, 2013
*
* @brief header file for FLASH module utilities (FLASH). 
*
*******************************************************************************
*
* provide APIs for accessing FLASH module (FLASH)
******************************************************************************/

#ifndef FLASH_H_
#define FLASH_H_
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* Includes
******************************************************************************/

#include "typedefine.h"
#include "PE_Error.h"
#include "S32K142.h"
#include "Function.h"
/******************************************************************************
* Constants
******************************************************************************/

/******************************************************************************
* Macros
******************************************************************************/

/******************************************************************************
* Uncomment the following line to support programming flash while running code from flash 
*
*//*! @addtogroup flash_stalling_control_enable
* @{
*******************************************************************************/

#define FLASH_ENABLE_STALLING_FLASH_CONTROLLER	/*!< enable flash stalling control */

/*! @} End of flash_stalling_control_enable                                  */

/******************************************************************************
* Memory Controller Command Completion Status Flag
*
*//*! @addtogroup flash_MGSTAT_list
* @{
*******************************************************************************/


	#define FLASH_SECTOR_SIZE			4096							  /*!< FTMRE sector size*/

	/******************************************************************************
	* flash and eeprom driver error status
	*
	*//*! @addtogroup flash_error_list
	* @{
	*******************************************************************************/
	#define FLASH_ERR_BASE				0x3000										/*!< FTMRE error base */
	#define FLASH_ERR_SUCCESS			0													/*!< FTMRE sucess */
	#define FLASH_ERR_INVALID_PARAM		(FLASH_ERR_BASE+1)		/*!<  invalid parameter error code*/
	#define FLASH_ERR_ACCESS			(FLASH_ERR_BASE+8)				/*!< flash access error code*/
	#define FLASH_ERR_PROTECTION		(FLASH_ERR_BASE+0x10)		/*!<  flash protection error code*/
	#define FLASH_ERR_MGSTAT0			(FLASH_ERR_BASE+0x11)			/*!<  flash verification error code*/
	#define FLASH_ERR_MGSTAT1			(FLASH_ERR_BASE+0x12)			/*!<  flash non-correctable error code*/
	#define FLASH_ERR_INIT_CCIF			(FLASH_ERR_BASE+0x14)		/*!<  flash driver init error with CCIF = 1*/
	#define FLASH_ERR_INIT_FDIV			(FLASH_ERR_BASE+0x18)		/*!<  flash driver init error with wrong FDIV*/ 
	/*! @} End of flash_error_list                                                */




/******************************************************************************
*
*//*! @addtogroup nvm_api_list
* @{
*******************************************************************************/
/******************************************************************************
* inline function
******************************************************************************/




/******************************************************************************
* Types
******************************************************************************/


/******************************************************************************
* Global variables
******************************************************************************/
extern unsigned int __MY_RAM_ADR;
extern unsigned int __MY_RAM_ROM_ADR;
extern unsigned int __MY_RAM_SIZE;
/******************************************************************************
* Global functions
******************************************************************************/


__attribute__ ((section(".MyRamCode"))) uint16_t FLASH_Program1LongWord(uint32_t wNVMTargetAddress, uint32_t dwData);
__attribute__ ((section(".MyRamCode"))) uint16_t FLASH_Program2LongWords(uint32_t wNVMTargetAddress, uint32_t dwData0, uint32_t dwData1);
__attribute__ ((section(".MyRamCode")))uint16_t FLASH_EraseSector(uint32_t wNVMTargetAddress);

__attribute__ ((section(".MyRamCode")))uint16_t FLASH_Program(uint32_t wNVMTargetAddress, uint8_t *pData, uint16_t sizeBytes);

__attribute__ ((section(".MyRamCode")))void FLASH_LaunchCMD(uint8_t bWaitComplete);

__attribute__ ((section(".MyRamCode")))void FLASH_Init(void);

/*! @} End of nvm_api_list                                              */
/********************************************************************/
#ifdef __cplusplus
}
#endif
#endif /* FLASH_H_ */
