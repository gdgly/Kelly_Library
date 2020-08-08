/*
 * CAN_Init.c
 *
 *  Created on: 2018��5��30��
 *      Author: Administrator
 */
#include "typedefine.h"
#include "S32K142.h"
#include "CAN.h"


/*************************************************************************
*  �������ƣ�CANInit
*  ����˵����CANChannel��ģ��� 0,1,2
//		       baudrateKHz: ������, 250,500,1000
*  �������أ�0���ɹ���1��ʧ��
*************************************************************************/
APP_SECTION uint8_t CANInit(uint8_t CANChannel,uint32_t baudrateKHz,uint32_t id_ext,uint32_t *id_table)
{
    uint8_t i;
    CAN_MemMapPtr CANBaseAdd;
		MailBox				*pMBox;

    //ͨ��ģ���ѡ��ģ�����ַ,ʹ��FlexCAN�ⲿʱ��
    if(CANChannel == 0)
    {
			CANBaseAdd = CAN0;
			PCC-> PCCn[PCC_FlexCAN0_INDEX] = PCC_PCCn_CGC_MASK;   //BUSCLK
    }
    else
    {
			CANBaseAdd = CAN1;
			PCC-> PCCn[PCC_FlexCAN1_INDEX] = PCC_PCCn_CGC_MASK;   //BUSCLK
    }

    // The CAN engine clock source is XTAL  8MHz
    CANBaseAdd->MCR |= CAN_MCR_MDIS_MASK;
    CANBaseAdd->CTRL1 &= ~CAN_CTRL1_CLKSRC_MASK;
    CANBaseAdd->MCR &= ~CAN_MCR_MDIS_MASK;

    // Soft Reset
    CANBaseAdd->MCR ^= CAN_MCR_SOFTRST_MASK;
    while(CAN_MCR_SOFTRST_MASK & CANBaseAdd->MCR);

    // Enter Fraze Mode
    CANBaseAdd->MCR |= CAN_MCR_FRZ_MASK ;
    CANBaseAdd->MCR |= CAN_MCR_HALT_MASK ;
    while(!(CAN_MCR_FRZACK_MASK & CANBaseAdd->MCR));

		// ���ò�����
    if(baudrateKHz>=1000)
    {
			/* Configure for 500 KHz bit time, 16 time quanta, cpi_clk = 8MHz *///100k=8M/(0+1+3+4);
			CANBaseAdd->CTRL1 = CAN_CTRL1_PRESDIV(0) |
													CAN_CTRL1_RJW(0) |
													CAN_CTRL1_PSEG1(0) |
													CAN_CTRL1_PSEG2(1) |
													CAN_CTRL1_PROPSEG(3);
		 }
		 else if(baudrateKHz>=500)
     {
			/* Configure for 500 KHz bit time, 16 time quanta, cpi_clk = 8MHz *///500k=8M/(4+4+4+4);
			CANBaseAdd->CTRL1 = CAN_CTRL1_PRESDIV(0) |
													CAN_CTRL1_RJW(3) |
													CAN_CTRL1_PSEG1(4) |
													CAN_CTRL1_PSEG2(4) |
													CAN_CTRL1_PROPSEG(4);
		 }
		 else
     {
			/* Configure for 250 KHz bit time, 16 time quanta, cpi_clk = 8MHz *///500k=8M/2/(4+4+4+4);
			CANBaseAdd->CTRL1 = CAN_CTRL1_PRESDIV(1) |
													CAN_CTRL1_RJW(3) |
													CAN_CTRL1_PSEG1(4) |
													CAN_CTRL1_PSEG2(4) |
													CAN_CTRL1_PROPSEG(4);
		 }


    //��ʼ������Ĵ���,IDλȫ�˲�
   	CANBaseAdd->RXMGMASK = 0;
 	  CANBaseAdd->RXFGMASK = 0;

		if(id_ext == 1)
		{
			for(i=0;i<16;i++)
				CANBaseAdd->RXIMR[i] = 0xfffffffe;
		}
		else
		{
			for(i=0;i<16;i++)
				CANBaseAdd->RXIMR[i] = 0xfff80000;
		}
		//���˲�
		//for(i=0;i<16;i++)
		//	CANBaseAdd->RXIMR[i] = 0x00000000;


		// ���÷��ͽ���MailBox
    CANBaseAdd->MCR |= CAN_MCR_SRXDIS_MASK;
    CANBaseAdd->MCR |= 15; 									// 16��MailBox
    CANBaseAdd->MCR |= CAN_MCR_RFEN_MASK; 	// Rx FIFO
    CANBaseAdd->MCR |= CAN_MCR_IRMQ_MASK;	/* Individual mask register used */
		CANBaseAdd->CTRL2 = 0x00A00000;

	  pMBox	= (MailBox *)(CANBaseAdd->RAMn);
		//	Rx Filter
		for(i=0;i<8;i++)
		{
				if(id_ext == 1)
					pMBox->RxFilter[i].RXIDA	= id_table[i] << 1;
				else
					pMBox->RxFilter[i].RXIDA	= id_table[i] << 19;
				pMBox->RxFilter[i].IDE 		= id_ext;//��׼֡Ϊ0����չ֡Ϊ1
				pMBox->RxFilter[i].RTR 		= 0;
		}
    // Tx MailBox
		for(i=0;i<8;i++)
		{
				pMBox ->TxMBox[i].CODE = FLEXCAN_MB_CODE_TX_INACTIVE;
		}

    // Exit Fraze Mode
	  CANBaseAdd->MCR &= ~(CAN_MCR_FRZ_MASK);
	  while( CANBaseAdd->MCR & CAN_MCR_FRZACK_MASK);
	  while( CANBaseAdd->MCR & CAN_MCR_NOTRDY_MASK);

    return 0;
}


/*************************************************************************
*  �������ƣ�CANSendData
*  ����˵������������
*  ����˵����CANChannel��ģ���
//	         id: ID��
//           length�����ݳ���
//	         Data[8]:�������ݻ�����
*  �������أ�0���ɹ���1��ʧ��
*************************************************************************/
APP_SECTION uint8_t CANSendData(uint8_t CANChannel, uint32_t id_ext, uint32_t id, uint8_t length,uint8_t Data[])
{
    CAN_MemMapPtr CANBaseAdd;
		MailBox		*pMBox;
    uint16_t  i;

    if(CANChannel == 0)
      CANBaseAdd = CAN0;
    else
      CANBaseAdd = CAN1;

	  pMBox	= (MailBox *)(CANBaseAdd->RAMn);

		for(i=0;i<8;i++)
		{
			  if(pMBox->TxMBox[i].CODE == FLEXCAN_MB_CODE_TX_INACTIVE) break;
			  if(pMBox->TxMBox[i].CODE == FLEXCAN_MB_CODE_TX_ABORT) break;
		}
		if(i==8)	return 1;

		if(id_ext) 	pMBox->TxMBox[i].ID = id;
		else				pMBox->TxMBox[i].ID = id<<18;
		pMBox->TxMBox[i].DLC = length;

		pMBox->TxMBox[i].Data[3] = Data[0];
		pMBox->TxMBox[i].Data[2] = Data[1];
		pMBox->TxMBox[i].Data[1] = Data[2];
		pMBox->TxMBox[i].Data[0] = Data[3];
		pMBox->TxMBox[i].Data[7] = Data[4];
		pMBox->TxMBox[i].Data[6] = Data[5];
		pMBox->TxMBox[i].Data[5] = Data[6];
		pMBox->TxMBox[i].Data[4] = Data[7];

		pMBox->TxMBox[i].IDE  = id_ext;
		pMBox->TxMBox[i].RTR  = 0;
		pMBox->TxMBox[i].CODE = FLEXCAN_MB_CODE_TX_ONCE;

    return 0;
}

/*************************************************************************
*  �������ƣ���������
*  ����˵����ʹ�ܽ��ջ�����
*  ����˵����CANChannel��CANģ���
*  �������أ�0���ɹ���1��ʧ��
//	         id: ID��
//	         lenght�����ݳ���
//	         Data: �������ݻ�����
*************************************************************************/
APP_SECTION uint8_t CANRecData(uint8_t CANChannel, uint32_t *id,uint8_t *Datalenght,uint8_t *Data)
{
    CAN_MemMapPtr CANBaseAdd;
		MailBox		*pMBox;

    if(CANChannel == 0)
      CANBaseAdd = CAN0;
    else
      CANBaseAdd = CAN1;

	  pMBox	= (MailBox *)(CANBaseAdd->RAMn);

		if((CANBaseAdd->IFLAG1 & CAN_IFLAG1_BUF5I_MASK)==0)	return 1;

		*id = pMBox->RxMBox[0].ID;
    if(pMBox->RxMBox[0].IDE==0) *id >>= 18;

		*Datalenght = pMBox->RxMBox[0].DLC;

    Data[0] = pMBox->RxMBox[0].Data[3];
    Data[1] = pMBox->RxMBox[0].Data[2];
    Data[2] = pMBox->RxMBox[0].Data[1];
    Data[3] = pMBox->RxMBox[0].Data[0];
    Data[4] = pMBox->RxMBox[0].Data[7];
    Data[5] = pMBox->RxMBox[0].Data[6];
    Data[6] = pMBox->RxMBox[0].Data[5];
    Data[7] = pMBox->RxMBox[0].Data[4];

		CANBaseAdd->IFLAG1 |= CAN_IFLAG1_BUF5I_MASK;

		return 0;
}


