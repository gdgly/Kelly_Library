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
* @file flash.c
*
* @author Freescale
*
* @version 0.0.1
*
* @date Jun. 25, 2013
*
* @brief providing APIs for configuring FLASH module (FLASH). 
*
*******************************************************************************
*
* provide APIs for configuring FLASH module (FLASH)
******************************************************************************/
#include "flash.h"
#include "Function.h"
#include "Variable.h"
#include "Platform.h"
#include <string.h>
/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/

/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Local functions
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

/******************************************************************************
* define NVM APIs
*
*//*! @addtogroup nvm_api_list
* @{
*******************************************************************************/

/*****************************************************************************//*!
  *
  * @brief initialize flash driver.
  *        
  * @param[in]  u32BusClock current bus clock.
  *
  * @return flash error status.
  *
  * @ Pass/ Fail criteria: none.
*****************************************************************************/

/*****************************************************************************//*!
  *
  * @brief initialize flash driver.
  *        
  * @param[in]  u32BusClock current bus clock.
  *
  * @return flash error status.
  *
  * @ Pass/ Fail criteria: none.
*****************************************************************************/

void FLASH_Init(void)
{
  LMEM->PCCCR = 0x85000001; /* Invalidate cache & enable write buffer, cache */
  MSCM->OCMDR[0] = 0x00000020; /* Bit 5 = 1: Enable program flash prefetch buffer */
  MSCM->OCMDR[1] = 0x00000020; /* Bit 5 = 1: Enable data flash prefetch buffer */
}

/*****************************************************************************//*!
  *
  * @brief program flash routine.
  *        
  * @param[in]   u32NVMTargetAddress programed flash address.
  * @param[in]   pData programming data pointer.
  * @param[in]   u16SizeBytes programming data length.
  *
  * @return flash error status.
  *
  * @ Pass/ Fail criteria: none.
*****************************************************************************/

uint16_t FLASH_Program(uint32_t u32NVMTargetAddress, uint8_t *pData, uint16_t u16SizeBytes)
{
	uint16_t u16Err = FLASH_ERR_SUCCESS;
	uint16_t u16w2LongWordCount = u16SizeBytes>>3;
	uint8_t  u8WrLeftBytes = (u16SizeBytes & 0x07);
	uint16_t u16WrLeftLongWords = u8WrLeftBytes>>2;
	uint32_t u32WrTargetAddress = u32NVMTargetAddress;
	uint32_t u32DwData0,u32DwData1;
	uint32_t *pDwData = (uint32_t*)pData;
	int  i;
	
	// Check address to see if it is aligned to 4 bytes
	// Global address [1:0] must be 00.
	if(u32NVMTargetAddress & 0x03)
	{
		u16Err = FLASH_ERR_INVALID_PARAM;
		return (u16Err);
	}
	// Loop for the two longwords (8 bytes) programming
	for(i = 0; i < u16w2LongWordCount; i++)
	{
		u32DwData0 = *pDwData++;
		u32DwData1 = *pDwData++;
		u16Err = FLASH_Program2LongWords(u32WrTargetAddress, u32DwData0, u32DwData1);
		if(u16Err)
		{
			goto EndP;
			//break;
		}
		u32WrTargetAddress += 8;
	}
	// Loop for the single longword (4 bytes) programming
	for(i = 0; i < u16WrLeftLongWords; i++)
	{
		u32DwData0 = *pDwData++;
		u16Err = FLASH_Program1LongWord(u32WrTargetAddress, u32DwData0);
		if(u16Err)
		{			
			goto EndP;
			//break;
		}
		u32WrTargetAddress += 4;
	}

EndP:	
	return (u16Err);
}
/*****************************************************************************//*!
  *
  * @brief program flash routine, program 1 long word to flash.
  *        
  * @param[in]   u32NVMTargetAddress programed flash address.
  * @param[in]   u32DwData programming data.
  *
  * @return flash error status.
  *
  * @ Pass/ Fail criteria: none.
*****************************************************************************/

uint16_t FLASH_Program1LongWord(uint32_t u32NVMTargetAddress, uint32_t u32DwData)
{
	uint16_t u16Err = FLASH_ERR_SUCCESS;
	
	// Check address to see if it is aligned to 4 bytes
	// Global address [1:0] must be 00.
	if(u32NVMTargetAddress & 0x03)
	{
		u16Err = FLASH_ERR_INVALID_PARAM;
		return (u16Err);
	}
	// Clear error flags
	FTFC->FSTAT = 0x70;

	FTFC->FCCOB[3] = 0x07;  //Program Phrase command (0x07)
	FTFC->FCCOB[2] = u32NVMTargetAddress>>16;  //Flash address [23:16]
	FTFC->FCCOB[1] = u32NVMTargetAddress>>8;  //Flash address [15:08]
	FTFC->FCCOB[0] = u32NVMTargetAddress;  //Flash address [7:0]
	FTFC->FCCOB[7] = u32DwData>>24;  //data0  31~24
	FTFC->FCCOB[6] = u32DwData>>16;  //23~16
	FTFC->FCCOB[5] = u32DwData>>8;  //15~08
	FTFC->FCCOB[4] = u32DwData;  //7~0
	FTFC->FCCOB[11] = 0xff; //
	FTFC->FCCOB[10] = 0xff; //
	FTFC->FCCOB[9] = 0xff;  //
	FTFC->FCCOB[8] = 0xff;  //
	

	// Launch the command
	FLASH_LaunchCMD(TRUE);
	
	// Check error status
	if(FTFC->FSTAT & FTFC_FSTAT_ACCERR_MASK)
	{
		u16Err |= FLASH_ERR_ACCESS;
	}
	if(FTFC->FSTAT & FTFC_FSTAT_FPVIOL_MASK)
	{
		u16Err |= FLASH_ERR_PROTECTION;		
	}
	if(FTFC->FSTAT & FTFC_FSTAT_MGSTAT0_MASK)
	{
		u16Err |= FLASH_ERR_MGSTAT0;		
	}
	
	return (u16Err);
}

/*****************************************************************************//*!
  *
  * @brief program flash routine, program 2long word to flash.
  *        
  * @param[in]   u32NVMTargetAddress programed flash address.
  * @param[in]   u32DwData0 programming data0.
  * @param[in]   u32DwData1 programming data1.
  *
  * @return flash error status.
  *
  * @ Pass/ Fail criteria: none
*****************************************************************************/

uint16_t FLASH_Program2LongWords(uint32_t u32NVMTargetAddress, uint32_t u32DwData0, uint32_t u32DwData1)
{
	uint16_t u16Err = FLASH_ERR_SUCCESS;
	
	// Check address to see if it is aligned to 4 bytes
	// Global address [1:0] must be 00.
	if(u32NVMTargetAddress & 0x03)
	{
		u16Err = FLASH_ERR_INVALID_PARAM;
		return (u16Err);
	}
	// Clear error flags
	FTFC->FSTAT = 0x70;
	
	FTFC->FCCOB[3] = 0x07;  //Program Phrase command (0x07)
	FTFC->FCCOB[2] = u32NVMTargetAddress>>16;  //Flash address [23:16]
	FTFC->FCCOB[1] = u32NVMTargetAddress>>8;  //Flash address [15:08]
	FTFC->FCCOB[0] = u32NVMTargetAddress;  //Flash address [7:0]
	FTFC->FCCOB[7] = u32DwData0>>24;  //data0  31~24
	FTFC->FCCOB[6] = u32DwData0>>16;  //23~16
	FTFC->FCCOB[5] = u32DwData0>>8;  //15~08
	FTFC->FCCOB[4] = u32DwData0;  //7~0
	FTFC->FCCOB[11] = u32DwData1>>24; //
	FTFC->FCCOB[10] = u32DwData1>>16; //
	FTFC->FCCOB[9] = u32DwData1>>8;  //
	FTFC->FCCOB[8] = u32DwData1;  //
	

	// Launch the command
	FLASH_LaunchCMD(TRUE);
	
	// Check error status
	if(FTFC->FSTAT & FTFC_FSTAT_ACCERR_MASK)
	{
		u16Err |= FLASH_ERR_ACCESS;
	}
	if(FTFC->FSTAT & FTFC_FSTAT_FPVIOL_MASK)
	{
		u16Err |= FLASH_ERR_PROTECTION;		
	}
	if(FTFC->FSTAT & FTFC_FSTAT_MGSTAT0_MASK)
	{
		u16Err |= FLASH_ERR_MGSTAT0;		
	}
	
	return (u16Err);
}



/*****************************************************************************//*!
  *
  * @brief erase flash sector, each flash sector is of 512 bytes long.
  *      
  *        
  * @param[in]   u32NVMTargetAddress erase sector address.
  *
  * @return error status.
  *
  * @ Pass/ Fail criteria: none.
*****************************************************************************/
uint16_t FLASH_EraseSector(uint32_t u32NVMTargetAddress)
{
	uint16_t u16Err = FLASH_ERR_SUCCESS;
	
	// Check address to see if it is aligned to 4 bytes
	// Global address [1:0] must be 00.
	if(u32NVMTargetAddress & 0x03)
	{
		u16Err = FLASH_ERR_INVALID_PARAM;
		return (u16Err);
	}
	// Clear error flags
	FTFC->FSTAT = 0x70;
	
	FTFC->FCCOB[3] = 0x09;	//Erase Flash Sector command (0x09)
	FTFC->FCCOB[2] = u32NVMTargetAddress>>16;	//Flash address [23:16]
	FTFC->FCCOB[1] = u32NVMTargetAddress>>8;	//Flash address [15:08]
	FTFC->FCCOB[0] = u32NVMTargetAddress;	//Flash address [7:0]

	// Launch the command
	FLASH_LaunchCMD(TRUE);
	
	// Check error status
	if(FTFC->FSTAT & FTFC_FSTAT_ACCERR_MASK)
	{
		u16Err |= FLASH_ERR_ACCESS;
	}
	if(FTFC->FSTAT & FTFC_FSTAT_FPVIOL_MASK)
	{
		u16Err |= FLASH_ERR_PROTECTION;		
	}
	if(FTFC->FSTAT & FTFC_FSTAT_MGSTAT0_MASK)
	{
		u16Err |= FLASH_ERR_MGSTAT0;		
	}


	return (u16Err);
}


/*****************************************************************************//*!
  *
  * @brief launch NVM command. if bWaitComplete is true, then wait till command
  *        is completed. It should be placed in RAM for some commands like
  *        verify backdoor key if enabling flash controller bit is cleared. Otherwise,
  *        it can be executed in flash.
  *        
  * @param[in] bWaitComplete:  true if need wait till command completed before return.  
  *	
  *
  * @return none
  *
  * @ Pass/ Fail criteria: none
*****************************************************************************/
void FLASH_LaunchCMD(uint8_t bWaitComplete)
{

		FTFC->FSTAT = FTFC_FSTAT_CCIF_MASK;

    if(bWaitComplete)
    {
      // Wait till command is completed
      while (!(FTFC->FSTAT & FTFC_FSTAT_CCIF_MASK))
      {
    	 // WDOG_Clear();
      }
    }
}


void OpenFlash(void)
{

  uint32_t par;
  uint32_t i;
  par=CAL_DATA_FLASH_ADDRESS;
	for(i=0;i<CAL_RAM_SIZE;i++)
	{
		Cal_data[i]=(*(uint8_t *)par++);
	}
}


void CloseFlash(void)
{
  disable_WDOG();

  memcpy(&__MY_RAM_ADR , &__MY_RAM_ROM_ADR, &__MY_RAM_SIZE);

  FLASH_EraseSector(CAL_DATA_FLASH_ADDRESS);
  FLASH_EraseSector(CAL_DATA_FLASH_ADDRESS +2048);
  FLASH_Program(CAL_DATA_FLASH_ADDRESS,&Cal_data[0],CAL_RAM_SIZE );

  Init_WDOG();
}
/*! @} End of nvm_api_list                                               						*/


/*! @} End of nvm_api_list                                               						*/

