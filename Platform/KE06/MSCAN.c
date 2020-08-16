#include <Platform/KE06/MSCAN.h>
#include "drivers/fsl_mscan.h"

#include "CANController/CANController.h"

#include <stdint.h>
#include <stdbool.h>

static uint32_t FreqClockSource;

bool KE06_MSCAN_GetTxBufferEmptyFlag(void)
{
	return (bool)(MSCAN->CANTFLG & MSCAN_CANTFLG_TXE_MASK);
}

bool KE06_MSCAN_GetRxBufferFullFlag(void)
{
	return (bool)(MSCAN->CANRFLG & MSCAN_CANRFLG_RXF_MASK);
}

void KE06_MSCAN_EnableTxBufferEmptyInterrupt(void)
{
	MSCAN->CANTIER |= MSCAN_CANTIER_TXEIE_MASK;
}

void KE06_MSCAN_EnableRxBufferFullInterrupt(void)
{
	MSCAN->CANRIER |= MSCAN_CANRIER_RXFIE_MASK;
}

void KE06_MSCAN_DisableTxBufferEmptyInterrupt(void)
{
	MSCAN->CANTIER &= ~MSCAN_CANTIER_TXEIE_MASK;
}

void KE06_MSCAN_DisableRxBufferFullInterrupt(void)
{
	MSCAN->CANRIER &= ~MSCAN_CANRIER_RXFIE_MASK;
}

void KE06_MSCAN_ClearRxBufferFullFlag(void)
{
	MSCAN->CANRFLG |= MSCAN_CANRFLG_RXF_MASK;
}


void KE06_MSCAN_SetBaudRate(uint32_t baudRate)
{
	MSCAN_SetBaudRate(MSCAN, FreqClockSource, baudRate);
}


//void KE06_MSCAN_WriteTxMessageBuffer(CAN_ID_t id, CAN_FrameFormat_t format, CAN_FrameType_t type, uint8_t length, uint8_t * p_data)
//{
//	mscan_frame_t txFrame;
//
//	txFrame.ID_Type.ID = id;
//    txFrame.format     = format;
//    txFrame.type       = type;
//    txFrame.DLR        = length;
//
//    for (i = 0U; i < length; i++)
//    {
//    	txFrame.DSR[i] = p_data[i];
//    }
//
//	MSCAN_WriteTxMb(MSCAN, &txFrame);
//}

void KE06_MSCAN_WriteTxMessageBuffer(CAN_Frame_t * p_txFrame)
{
    uint8_t txEmptyFlag;
    mscan_mb_t mb = {0};
    IDR1_3_UNION sIDR1, sIDR3;
    status_t status;
    uint8_t i;

    /* Write IDR. */
    if (kMSCAN_FrameFormatExtend == p_txFrame->Format)
    {
        /* Deal with Extended frame. */
        sIDR1.IDR1.EID20_18_OR_SID2_0 = p_txFrame->ID.ExtID.EID20_18;
        sIDR1.IDR1.R_TSRR             = 1U;
        sIDR1.IDR1.R_TEIDE            = 1U;
        sIDR1.IDR1.EID17_15           = p_txFrame->ID.ExtID.EID17_15;
        sIDR3.IDR3.EID6_0             = p_txFrame->ID.ExtID.EID6_0;
        sIDR3.IDR3.ERTR               = (kMSCAN_FrameTypeRemote == p_txFrame->Type) ? 1U : 0U;
        /* Write into MB structure. */
        mb.EIDR0 = p_txFrame->ID.ExtID.EID28_21;
        mb.EIDR1 = sIDR1.Bytes;
        mb.EIDR2 = p_txFrame->ID.ExtID.EID14_7;
        mb.EIDR3 = sIDR3.Bytes;
    }
    else
    {
        /* Deal with Standard frame. */
        sIDR1.IDR1.EID20_18_OR_SID2_0 = p_txFrame->ID.StdID.EID2_0;
        sIDR1.IDR1.R_TSRR             = 0U;
        sIDR1.IDR1.R_TEIDE            = 0U;
        sIDR1.IDR1.EID17_15           = 0U; /* Reserved for Standard frame*/
        /* Write into MB structure. */
        mb.EIDR0 = p_txFrame->ID.StdID.EID10_3;
        mb.EIDR1 = sIDR1.Bytes;
    }
    /* Write DLR, BPR */
    mb.DLR = p_txFrame->DLR;
    mb.BPR = p_txFrame->BPR;

    /* Write DSR */
    for (i = 0U; i < mb.DLR; i++)
    {
        mb.EDSR[i] = p_txFrame->DSR[i];
    }

    /* 1.Read TFLG to get the empty transmitter buffers. */
    txEmptyFlag = MSCAN_GetTxBufferEmptyFlag(MSCAN);

//    if ((uint8_t)kMSCAN_TxBufFull != txEmptyFlag)
//    {
        /* 2.Write TFLG value back. */
        MSCAN_TxBufferSelect(MSCAN, txEmptyFlag);
        /* Push contents of mb structure into hardware register. */
        MSCAN->TEIDR0 = mb.EIDR0;
        MSCAN->TEIDR1 = mb.EIDR1;
        MSCAN->TEIDR2 = mb.EIDR2;
        MSCAN->TEIDR3 = mb.EIDR3;
        for (i = 0U; i < mb.DLR; i++)
        {
        	MSCAN->TEDSR[i] = mb.EDSR[i];
        }
        MSCAN->TDLR = mb.DLR;
        MSCAN->TBPR = mb.BPR;

        /* 3.Read TBSEL again to get lowest tx buffer, then write 1 to clear
        the corresponding bit to schedule transmission. */
        MSCAN_TxBufferLaunch(MSCAN, MSCAN_GetTxBufferSelect(MSCAN));

//        status = kStatus_Success;
//    }
//    else
//    {
//        status = kStatus_Fail;
//    }

    //return status;
}

void KE06_MSCAN_ReadRxMessageBuffer(CAN_Frame_t * p_rxFrame)
{
    IDR1_3_UNION sIDR1;
    IDR1_3_UNION sIDR3;
    uint8_t i;
    status_t status;

//    if (0U != MSCAN_GetRxBufferFullFlag(MSCAN))
//    {
        sIDR1.Bytes      = MSCAN_ReadRIDR1(MSCAN);
        sIDR3.Bytes      = MSCAN_ReadRIDR3(MSCAN);
        p_rxFrame->Format = (mscan_frame_format_t)(sIDR1.IDR1.R_TEIDE);

        if (kMSCAN_FrameFormatExtend == p_rxFrame->Format) /* Extended frame. */
        {
            p_rxFrame->Type                   = (mscan_frame_type_t)(sIDR3.IDR3.ERTR);
            p_rxFrame->ID.ExtID.EID28_21 = MSCAN_ReadRIDR0(MSCAN);
            p_rxFrame->ID.ExtID.EID20_18 = sIDR1.IDR1.EID20_18_OR_SID2_0;
            p_rxFrame->ID.ExtID.EID17_15 = sIDR1.IDR1.EID17_15;
            p_rxFrame->ID.ExtID.EID14_7  = MSCAN_ReadRIDR2(MSCAN);
            p_rxFrame->ID.ExtID.EID6_0   = sIDR3.IDR3.EID6_0;
        }
        else /* Standard frame. */
        {
            p_rxFrame->Type                  = (mscan_frame_type_t)(sIDR1.IDR1.R_TSRR);
            p_rxFrame->ID.StdID.EID10_3 = MSCAN_ReadRIDR0(MSCAN);
            p_rxFrame->ID.StdID.EID2_0  = sIDR1.IDR1.EID20_18_OR_SID2_0;
        }

        p_rxFrame->DLR = MSCAN->RDLR & 0x0FU;
        for (i = 0; i < p_rxFrame->DLR; i++)
        {
            p_rxFrame->DSR[i] = MSCAN->REDSR[i];
        }

        p_rxFrame->TSRH = MSCAN->RTSRH;
        p_rxFrame->TSRL = MSCAN->RTSRL;

//        status = kStatus_Success;
//    }
//    else
//    {
//        status = kStatus_Fail;
//    }
//
//    return status;
}

/*! @brief Initialize KE06 MSCAN module. Default settings, interrupt driven for send and receive */
void KE06_MSCAN_Init(uint32_t busFreq)
{
	mscan_config_t mscanConfig;

	FreqClockSource = busFreq;

	mscanConfig.baudRate				= 500000;
	mscanConfig.enableTimer             = false;
	mscanConfig.enableWakeup            = false;
	mscanConfig.clkSrc                  = kMSCAN_ClkSrcBus;
	mscanConfig.enableLoopBack          = false;
	mscanConfig.enableListen            = false;
	mscanConfig.busoffrecMode           = kMSCAN_BusoffrecAuto;
	mscanConfig.filterConfig.filterMode = kMSCAN_Filter32Bit;

//    mscanConfig.filterConfig.u32IDAR0 = MSCAN_RX_MB_EXT_MASK(NODE_ID1);
//    mscanConfig.filterConfig.u32IDAR1 = MSCAN_RX_MB_EXT_MASK(NODE_ID1);
//    mscanConfig.filterConfig.u32IDMR0 = MSCAN_IDMR0;
//    mscanConfig.filterConfig.u32IDMR1 = MSCAN_IDMR1;

	MSCAN_Init(MSCAN, &mscanConfig, busFreq);

    MSCAN_EnableRxInterrupts(MSCAN, kMSCAN_RxFullInterruptEnable);
	MSCAN_EnableTxInterrupts(MSCAN, kMSCAN_TxEmptyInterruptEnable);
    EnableIRQ(MSCAN_1_IRQn);
    EnableIRQ(MSCAN_2_IRQn);
}

