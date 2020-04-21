/*******************************************************************************
*
* Copyright 2004-2014 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale FreeMASTER License
* distributed with this Material.
* See the LICENSE file distributed for more details.
* 
****************************************************************************//*!
*
* @brief  FreeMASTER Driver ColdFire 52xx hardware dependent stuff
* 
*******************************************************************************/

#ifndef __FREEMASTER_MCF52xx_H
#define __FREEMASTER_MCF52xx_H

/******************************************************************************
* platform-specific default configuration
******************************************************************************/

/* use 32-bit (EX) commands by default */
#ifndef FMSTR_USE_EX_CMDS
#define FMSTR_USE_EX_CMDS 1
#endif

/* do not use 16-bit (no-EX) commands by default */
#ifndef FMSTR_USE_NOEX_CMDS
#define FMSTR_USE_NOEX_CMDS 0
#endif

/* at least one of EX or no-EX command handling must be enabled */
#if !(FMSTR_USE_EX_CMDS) && !(FMSTR_USE_NOEX_CMDS)
    #error At least one of EX or no-EX command handling must be enabled (please set FMSTR_USE_EX_CMDS)
    #undef  FMSTR_USE_EX_CMDS
    #define FMSTR_USE_EX_CMDS 1
#endif

/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define FMSTR_PROT_VER           3      /* protocol version 3 */
#define FMSTR_CFG_FLAGS  (FMSTR_CFGFLAG_BIGENDIAN | FMSTR_CFG_REC_LARGE_MODE) /* board info flags */
#define FMSTR_CFG_BUS_WIDTH      1      /* data bus width */
#define FMSTR_GLOB_VERSION_MAJOR 2      /* driver version */
#define FMSTR_GLOB_VERSION_MINOR 0
#define FMSTR_IDT_STRING "MCF52xx FreeMASTER Drv."
#define FMSTR_TSA_FLAGS          0

/******************************************************************************
* platform-specific types
******************************************************************************/

typedef unsigned char  FMSTR_U8;        /* smallest memory entity */
typedef unsigned short FMSTR_U16;       /* 16bit value */
typedef unsigned long  FMSTR_U32;       /* 32bit value */

typedef signed char    FMSTR_S8;        /* signed 8bit value */
typedef signed short   FMSTR_S16;       /* signed 16bit value */
typedef signed long    FMSTR_S32;       /* signed 32bit value */

#if FMSTR_REC_FLOAT_TRIG
typedef float          FMSTR_FLOAT;     /* float value */
#endif

typedef unsigned char  FMSTR_FLAGS;     /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned char  FMSTR_SIZE8;     /* one-byte size value */
typedef signed short   FMSTR_INDEX;     /* general for-loop index (must be signed) */

typedef unsigned char  FMSTR_BCHR;      /* type of a single character in comm.buffer */
typedef unsigned char* FMSTR_BPTR;      /* pointer within a communication buffer */

typedef unsigned char  FMSTR_SCISR;     /* data type to store SCI status register */

/******************************************************************************
* communication buffer access functions 
******************************************************************************/

void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize);
FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);
void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);

/* mixed EX and non-EX commands may occur */
#if (FMSTR_USE_EX_CMDS) && (FMSTR_USE_NOEX_CMDS)
void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx);
#else
/* otherwise, we always know what addresses are used, (ignore FMSTR_SetExAddr) */
#define FMSTR_SetExAddr(bNextAddrIsEx) 
#endif

/*********************************************************************************
* communication buffer access functions. Most of them are trivial simple on MCF52xx
*********************************************************************************/

#define FMSTR_ValueFromBuffer8(pDest, pSrc) \
    ( (*((FMSTR_U8*)(pDest)) = *(FMSTR_U8*)(pSrc)), (((FMSTR_BPTR)(pSrc))+1) )

#define FMSTR_ValueFromBuffer16(pDest, pSrc) \
    ( (*((FMSTR_U16*)(pDest)) = *(FMSTR_U16*)(pSrc)), (((FMSTR_BPTR)(pSrc))+2) )

#define FMSTR_ValueFromBuffer32(pDest, pSrc) \
    ( (*((FMSTR_U32*)(pDest)) = *(FMSTR_U32*)(pSrc)), (((FMSTR_BPTR)(pSrc))+4) )


#define FMSTR_ValueToBuffer8(pDest, src) \
    ( (*((FMSTR_U8*)(pDest)) = (FMSTR_U8)(src)), (((FMSTR_BPTR)(pDest))+1) )

#define FMSTR_ValueToBuffer16(pDest, src) \
    ( (*((FMSTR_U16*)(pDest)) = (FMSTR_U16)(src)), (((FMSTR_BPTR)(pDest))+2) )

#define FMSTR_ValueToBuffer32(pDest, src) \
    ( (*((FMSTR_U32*)(pDest)) = (FMSTR_U32)(src)), (((FMSTR_BPTR)(pDest))+4) )


#define FMSTR_SkipInBuffer(pDest, nSize) \
    ( ((FMSTR_BPTR)(pDest)) + (nSize) )


#define FMSTR_ConstToBuffer8  FMSTR_ValueToBuffer8
#define FMSTR_ConstToBuffer16 FMSTR_ValueToBuffer16

/* EX address used only: fetching 32bit word */
#if (FMSTR_USE_EX_CMDS) && !(FMSTR_USE_NOEX_CMDS)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer32(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer32(pDest, nAddr)

/* no-EX address used only: fetching 16bit word  */
#elif !(FMSTR_USE_EX_CMDS) && (FMSTR_USE_NOEX_CMDS)
    #define FMSTR_AddressFromBuffer(pDest, pSrc) \
        FMSTR_ValueFromBuffer16(pDest, pSrc)
    #define FMSTR_AddressToBuffer(pDest, nAddr) \
        FMSTR_ValueToBuffer16(pDest, nAddr)

/* mixed addresses used, need to process it programatically */
#else
    FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc);
    FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr);
#endif

#define FMSTR_GetS8(addr)  ( *(FMSTR_S8*)(addr) )
#define FMSTR_GetU8(addr)  ( *(FMSTR_U8*)(addr) )
#define FMSTR_GetS16(addr) ( *(FMSTR_S16*)(addr) )
#define FMSTR_GetU16(addr) ( *(FMSTR_U16*)(addr) )
#define FMSTR_GetS32(addr) ( *(FMSTR_S32*)(addr) )
#define FMSTR_GetU32(addr) ( *(FMSTR_U32*)(addr) )

#if FMSTR_REC_FLOAT_TRIG
#define FMSTR_GetFloat(addr) ( *(FMSTR_FLOAT*)(addr) )
#endif

/****************************************************************************************
* Other helper macros
*****************************************************************************************/

/* This macro assigns C pointer to FMSTR_ADDR-typed variable */
#define FMSTR_PTR2ADDR(tmpAddr,ptr) ( tmpAddr = (FMSTR_ADDR) (FMSTR_U8*) ptr )
#define FMSTR_ARR2ADDR FMSTR_PTR2ADDR

/****************************************************************************************
* Platform-specific configuration
*****************************************************************************************/

/* FlexCAN functionality implemented on MCF52xx */
#define FMSTR_CANHW_FLEXCAN 1 

/* Legacy USB stack is used by ColdFire CPUs */
#define FMSTR_USB_LEGACY_STACK 1

/****************************************************************************************
* General peripheral space access macros
*****************************************************************************************/

#define FMSTR_SETREG8(base, offset, value)    (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG8(base, offset)           (*(volatile FMSTR_U8*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETREG16(base, offset, value)   (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG16(base, offset)          (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U16)~((FMSTR_U16)(bit)))
#define FMSTR_TSTBIT16(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) & (bit))
#define FMSTR_SETREG32(base, offset, value)   (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG32(base, offset)          (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)))


/****************************************************************************************
* UART module constants
*****************************************************************************************/

/* UART module registers */
#define FMSTR_USR_OFFSET  0x04
#define FMSTR_UCR_OFFSET  0x08
#define FMSTR_URB_OFFSET  0x0C
#define FMSTR_UTB_OFFSET  0x0C
#define FMSTR_UIMR_OFFSET 0x14

/* UART Status Register bits */
#define FMSTR_USR_RB    0x80
#define FMSTR_USR_FE    0x40
#define FMSTR_USR_PE    0x20
#define FMSTR_USR_OE    0x10
#define FMSTR_USR_TXEMP 0x08
#define FMSTR_USR_TXRDY 0x04
#define FMSTR_USR_FFULL 0x02
#define FMSTR_USR_RXRDY 0x01

/* UART Command Register values (not bits!) */
#define FMSTR_UCR_RE 0x1
#define FMSTR_UCR_RD 0x2
#define FMSTR_UCR_TE 0x4
#define FMSTR_UCR_TD 0x8

/* UART Interrupt Mask Register (write only) */
#define FMSTR_UIMR_RXRDY 0x02
#define FMSTR_UIMR_TXRDY 0x01

/* emulate traditional SCI status bits as required by FreeMASTER driver */
#define FMSTR_SCISR_TDRE FMSTR_USR_TXRDY
#define FMSTR_SCISR_RDRF FMSTR_USR_RXRDY

/*******************************************************************************************
* SCI access macros (emualted on ColdFire UART module)
*****************************************************************************************/

/* transmitter enable/disable */
#define FMSTR_SCI_TE() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UCR_OFFSET, FMSTR_UCR_TE)
#define FMSTR_SCI_TD() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UCR_OFFSET, FMSTR_UCR_TD)

/* receiver enable/disable */
#define FMSTR_SCI_RE() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UCR_OFFSET, FMSTR_UCR_RE)
#define FMSTR_SCI_RD() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UCR_OFFSET, FMSTR_UCR_RD)

#define FMSTR_SCI_TE_RE() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UCR_OFFSET, FMSTR_UCR_RE | FMSTR_UCR_TE)

/* Interrupt enable/disable macros may look a bit strange on ColdFire UART Module.
   As the UIMR register is write-only, we cannot use bit-set or bit-clear statements.
   So, to disable one interrupt, we disable both RX and TX interrupts - luckily this
   is okay as the FreeMASTER driver always use the disable-enable sequence when
   exchanging RX and TX interrupts */

/* Transmitter-empty interrupt enable/disable  */
#define FMSTR_SCI_ETXI() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UIMR_OFFSET, FMSTR_UIMR_TXRDY)
#define FMSTR_SCI_DTXI() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UIMR_OFFSET, 0)

/* Receiver-full interrupt enable/disable */
#define FMSTR_SCI_ERXI() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UIMR_OFFSET, FMSTR_UIMR_RXRDY)
#define FMSTR_SCI_DRXI() FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UIMR_OFFSET, 0)

/* Tranmsit character */
#define FMSTR_SCI_PUTCHAR(ch) FMSTR_SETREG8(FMSTR_SCI_BASE, FMSTR_UTB_OFFSET, (FMSTR_U8) (ch))

/* Get received character */
#define FMSTR_SCI_GETCHAR() FMSTR_GETREG8(FMSTR_SCI_BASE, FMSTR_URB_OFFSET)

/* read status register */
#define FMSTR_SCI_GETSR()   FMSTR_GETREG8(FMSTR_SCI_BASE, FMSTR_USR_OFFSET)

/* read & clear status register */
#define FMSTR_SCI_RDCLRSR() FMSTR_SCI_GETSR()

/****************************************************************************************
* FCAN module constants
*****************************************************************************************/

/* FCAN module MB CODEs */
#define FMSTR_FCANMB_CODE_MASK      0x0F   /* defines mask of codes */

#define FMSTR_FCANMB_CRXVOID        0x00   /* buffer void after received data read-out */
#define FMSTR_FCANMB_CRXEMPTY       0x04   /* active and empty */

#define FMSTR_FCANMB_CTXTRANS_ONCE  0x0C   /* Initialize transmiting data from buffer */
#define FMSTR_FCANMB_CTXREADY       0x08   /* Message buffer not ready for transmit */

/* FCAN module registers offsets */
#define FMSTR_FCANTMR_OFFSET   0x08
#define FMSTR_FCANIER_OFFSET   0x2A
#define FMSTR_FCANIFR_OFFSET   0x32
#define FMSTR_FCANRXFG_OFFSET  (0x80 + ((FMSTR_FLEXCAN_RXMB) * 0x10))
#define FMSTR_FCANTXFG_OFFSET  (0x80 + ((FMSTR_FLEXCAN_TXMB) * 0x10))

/* FCAN MB registers offsets (must also add FCANxxFG_OFFSET) */
#define FMSTR_FCMBCSR   0x00
#define FMSTR_FCMBIDR0  0x04
#define FMSTR_FCMBIDR1  0x05
#define FMSTR_FCMBIDR2  0x06
#define FMSTR_FCMBIDR3  0x07
#define FMSTR_FCMBDSR0  0x08
#define FMSTR_FCMBDSR1  0x09
#define FMSTR_FCMBDSR2  0x0A
#define FMSTR_FCMBDSR3  0x0B
#define FMSTR_FCMBDSR4  0x0C
#define FMSTR_FCMBDSR5  0x0D
#define FMSTR_FCMBDSR6  0x0E
#define FMSTR_FCMBDSR7  0x0F

/* FCAN CANMSCSR */
#define FMSTR_FCANCTRL_IDE     0x20
#define FMSTR_FCANCTRL_STD_RTR 0x10
#define FMSTR_FCANCTRL_EXT_RTR 0x10
#define FMSTR_FCANCTRL_EXT_SRR 0x40

/* FCAN ID flags */
#define FMSTR_FCANID0_EXT_FLG  0x80

/* FCAN: enable/disable CAN RX/TX interrupts */
#define FMSTR_FCAN_ETXI() FMSTR_SETBIT16(FMSTR_CAN_BASE, FMSTR_FCANIER_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB)))
#define FMSTR_FCAN_DTXI() FMSTR_CLRBIT16(FMSTR_CAN_BASE, FMSTR_FCANIER_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB)))
#define FMSTR_FCAN_ERXI() FMSTR_SETBIT16(FMSTR_CAN_BASE, FMSTR_FCANIER_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB)))
#define FMSTR_FCAN_DRXI() FMSTR_CLRBIT16(FMSTR_CAN_BASE, FMSTR_FCANIER_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB)))

/* FCAN: read RX status register */
#define FMSTR_FCAN_TEST_RXFLG()   FMSTR_TSTBIT16(FMSTR_CAN_BASE, FMSTR_FCANIFR_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB)))
#define FMSTR_FCAN_CLEAR_RXFLG()  FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANIFR_OFFSET, (1<<(FMSTR_FLEXCAN_RXMB)))

/* FCAN: read TX status register */
#define FMSTR_FCAN_TEST_TXFLG()   FMSTR_TSTBIT16(FMSTR_CAN_BASE, FMSTR_FCANIFR_OFFSET, (1<<(FMSTR_FLEXCAN_TXMB)))

/* FCAN: read TX MB status register */
#define FMSTR_FCAN_GET_MBSTATUS() (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBCSR)&FMSTR_FCANMB_CODE_MASK)

/* FCAN: id to idr translation */
#define FMSTR_FCAN_MAKEIDR0(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((((id)>>24)&0x1f) | FMSTR_FCANID0_EXT_FLG) : (((id)>>6)&0x1f) ))
#define FMSTR_FCAN_MAKEIDR1(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((id)>>16) : ((id)<<2) ))
#define FMSTR_FCAN_MAKEIDR2(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? ((id)>>8) : 0 ))
#define FMSTR_FCAN_MAKEIDR3(id) ((FMSTR_U8)( ((id)&FMSTR_CAN_EXTID) ? (id) : 0 ))

/* FCAN reception, configuring the buffer, just once at the initialization phase */
#define FMSTR_FCAN_RINIT(idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
        (((idr0)&FMSTR_FCANID0_EXT_FLG) ? \
        (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR, (FMSTR_FCANMB_CRXVOID<<8 | FMSTR_FCANCTRL_IDE | FMSTR_FCANCTRL_EXT_SRR))) : \
        (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR, (FMSTR_FCANMB_CRXVOID<<8 |  FMSTR_FCANCTRL_EXT_SRR))));\
        FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBIDR0, ((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3) );\
    FMSTR_MACROCODE_END()

/* FCAN transmission, configuring the buffer, just once at the initialization phase */
#define FMSTR_FCAN_TINIT(idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
      (((idr0)&FMSTR_FCANID0_EXT_FLG) ? \
      (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR, (FMSTR_FCANMB_CTXREADY<<8 | FMSTR_FCANCTRL_IDE))) : \
      (FMSTR_SETREG16(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR, (FMSTR_FCANMB_CTXREADY<<8 ))));\
    FMSTR_MACROCODE_END()

/* FCAN reception, configuring the buffer for receiving (each time receiver is re-enabled) */
#define FMSTR_FCAN_RCFG() \
    FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBCSR, FMSTR_FCANMB_CRXEMPTY)

/* FCAN: CAN transmission */
typedef struct
{
    FMSTR_U8 nDataIx;
} FMSTR_FCAN_TCTX;

/* FCAN transmission, put one data byte into buffer */
#define FMSTR_FCAN_TLEN(pctx, len) \
    FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBCSR+1, (FMSTR_U8)((len & 0x0f) | \
        (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBCSR+1)&(FMSTR_FCANCTRL_IDE | FMSTR_FCANCTRL_EXT_SRR | FMSTR_FCANCTRL_EXT_RTR))))

/* FCAN transmission, put one data byte into buffer */
#define FMSTR_FCAN_PUTBYTE(pctx, dataByte) \
    FMSTR_MACROCODE_BEGIN() \
        FMSTR_SETREG8(FMSTR_CAN_BASE, (FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBDSR0) + ((pctx)->nDataIx), (dataByte) ); \
        (pctx)->nDataIx++; \
    FMSTR_MACROCODE_END()

/* FCAN: CAN transmission, configuring the buffer before each transmission */
#define FMSTR_FCAN_TCFG(pctx) \
    FMSTR_MACROCODE_BEGIN() \
        (pctx)->nDataIx = 0; \
    FMSTR_MACROCODE_END()

/* FCAN: CAN transmission, preparing the buffer before each transmission */
#define FMSTR_FCAN_TID(pctx, idr0, idr1, idr2, idr3) \
    FMSTR_MACROCODE_BEGIN() \
        FMSTR_SETREG32(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET+FMSTR_FCMBIDR0, ((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3) ); \
    FMSTR_MACROCODE_END()

/* FCAN transmission, set transmit priority */
#define FMSTR_FCAN_TPRI(pctx, txPri) /* in FCAN module is not implemented */

/* FCAN transmission, final firing of the buffer */
#define FMSTR_FCAN_TX(pctx) \
        FMSTR_SETREG8(FMSTR_CAN_BASE, FMSTR_FCANTXFG_OFFSET + FMSTR_FCMBCSR, (FMSTR_FCANMB_CTXTRANS_ONCE & 0x0f) )

/* FCAN reception */
typedef struct
{
    FMSTR_U8 nDataIx;
} FMSTR_FCAN_RCTX;

/* FCAN reception, lock frame */
#define FMSTR_FCAN_RX(pctx) \
    (pctx)->nDataIx = 0;

/* FCAN reception, test if received message ID matches the one given, TRUE if matching */
#define FMSTR_FCAN_TEST_RIDR(pctx, idr0, idr1, idr2, idr3) \
    ( (idr0 & FMSTR_FCANID0_EXT_FLG) ? \
        /* ext id compare */ \
        ( ((((idr0)<<24) | ((idr1)<<16) | ((idr2)<<8) | (idr3))&0x1f000000)==((FMSTR_GETREG32(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBIDR0))&0x1f000000) ) : \
        /* std id compare */ \
        ( (((idr0)<<8) | (idr1))==((FMSTR_GETREG16(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBIDR0))&0x1ffc) ) )

/* FCAN reception, get receviced frame length */
#define FMSTR_FCAN_RLEN(pctx) \
    (FMSTR_GETREG8(FMSTR_CAN_BASE, FMSTR_FCANRXFG_OFFSET+FMSTR_FCMBCSR+1) & 0x0f)

/* FCAN reception, get one received byte */
#define FMSTR_FCAN_GETBYTE(pctx) \
        ((FMSTR_U8) (FMSTR_GETREG8(FMSTR_CAN_BASE, (FMSTR_FCANRXFG_OFFSET + FMSTR_FCMBDSR0) + ((pctx)->nDataIx) ))); \
        (pctx)->nDataIx++

/* FCAN reception, unlock the buffer */
#define FMSTR_FCAN_RFINISH(pctx) \
    FMSTR_SETBIT16(FMSTR_CAN_BASE, FMSTR_FCANTMR_OFFSET, 0)

#endif /* __FREEMASTER_MCF52xx_H */
