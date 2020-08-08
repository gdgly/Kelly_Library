/*
 * interrupt.c
 *
 *  Created on: 2017Äê8ÔÂ18ÈÕ
 *      Author: ted
 */

#include "typedefine.h"
#include "Events.h"
#include "S32K142.h"
#include "Function.h"
#include "Motor.h"

APP_SECTION void Init_IRQs (uint8_t IRQ_Num,uint8_t Pri) {

	uint8_t temp1 = IRQ_Num / 32;
	uint8_t temp2 = IRQ_Num % 32;
	S32_NVIC->ICPR[temp1] = 1 << temp2;  /* clr any pending IRQ*/
	S32_NVIC->ISER[temp1] = 1 << temp2;  /* enable IRQ */
 	S32_NVIC->IP[IRQ_Num] = Pri << 4;              /*priority  0-15.  0,high*/
}

void PE_DEBUGHALT(void)
{}

INT_SECTION void   NMI_Handler (void) {PE_DEBUGHALT();}                                     /* NMI Handler*/
    INT_SECTION void   HardFault_Handler (void) {PE_DEBUGHALT();}                               /* Hard Fault Handler*/
    INT_SECTION void   MemManage_Handler (void) {PE_DEBUGHALT();}                               /* MPU Fault Handler*/
    INT_SECTION void   BusFault_Handler (void) {PE_DEBUGHALT();}                                /* Bus Fault Handler*/
    INT_SECTION void   UsageFault_Handler (void) {PE_DEBUGHALT();}                              /* Usage Fault Handler*/
    INT_SECTION void   Reserved7_IRQHandler (void) {PE_DEBUGHALT();}                                               /* Reserved*/
    INT_SECTION void   Reserved8_IRQHandler (void) {PE_DEBUGHALT();}                                               /* Reserved*/
    INT_SECTION void   Reserved9_IRQHandler (void) {PE_DEBUGHALT();}                                               /* Reserved*/
    INT_SECTION void   Reserved10_IRQHandler (void) {PE_DEBUGHALT();}                                               /* Reserved*/
    INT_SECTION void   SVC_Handler (void) {PE_DEBUGHALT();}                                     /* SVCall Handler*/
    INT_SECTION void   DebugMon_Handler (void) {PE_DEBUGHALT();}                                /* Debug Monitor Handler*/
    INT_SECTION void   Reserved13_IRQHandler (void) {PE_DEBUGHALT();}                                               /* Reserved*/
    INT_SECTION void   PendSV_Handler (void) {PE_DEBUGHALT();}                                  /* PendSV Handler*/
    INT_SECTION void   SysTick_Handler (void) {PE_DEBUGHALT();}                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    INT_SECTION void   DMA0_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 0 transfer complete*/
    INT_SECTION void   DMA1_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 1 transfer complete*/
    INT_SECTION void   DMA2_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 2 transfer complete*/
    INT_SECTION void   DMA3_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 3 transfer complete*/
    INT_SECTION void   DMA4_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 4 transfer complete*/
    INT_SECTION void   DMA5_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 5 transfer complete*/
    INT_SECTION void   DMA6_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 6 transfer complete*/
    INT_SECTION void   DMA7_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 7 transfer complete*/
    INT_SECTION void   DMA8_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 8 transfer complete*/
    INT_SECTION void   DMA9_IRQHandler (void) {PE_DEBUGHALT();}                                 /* DMA channel 9 transfer complete*/
    INT_SECTION void   DMA10_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 10 transfer complete*/
    INT_SECTION void   DMA11_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 11 transfer complete*/
    INT_SECTION void   DMA12_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 12 transfer complete*/
    INT_SECTION void   DMA13_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 13 transfer complete*/
    INT_SECTION void   DMA14_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 14 transfer complete*/
    INT_SECTION void   DMA15_IRQHandler (void) {PE_DEBUGHALT();}                                /* DMA channel 15 transfer complete*/
    INT_SECTION void   DMA_Error_IRQHandler (void) {PE_DEBUGHALT();}                            /* DMA error interrupt channels 0-15*/
    INT_SECTION void   MCM_IRQHandler (void) {PE_DEBUGHALT();}                                  /* FPU sources*/
    INT_SECTION void   FTFC_IRQHandler (void) {PE_DEBUGHALT();}                                 /* FTFC Command complete*/
    INT_SECTION void   Read_Collision_IRQHandler (void) {PE_DEBUGHALT();}                       /* FTFC Read collision*/
    INT_SECTION void   LVD_LVW_IRQHandler (void) {PE_DEBUGHALT();}                              /* PMC Low voltage detect interrupt*/
    INT_SECTION void   FTFC_Fault_IRQHandler (void) {PE_DEBUGHALT();}                           /* FTFC Double bit fault detect*/
    INT_SECTION void   WDOG_EWM_IRQHandler (void) {PE_DEBUGHALT();}                             /* Single interrupt vector for WDOG and EWM*/
    INT_SECTION void   RCM_IRQHandler (void) {PE_DEBUGHALT();}                                  /* RCM Asynchronous Interrupt*/
    INT_SECTION void   LPI2C0_Master_IRQHandler (void) {PE_DEBUGHALT();}                        /* LPI2C0 Master Interrupt*/
    INT_SECTION void   LPI2C0_Slave_IRQHandler (void) {PE_DEBUGHALT();}                         /* LPI2C0 Slave Interrupt*/
    INT_SECTION void   LPSPI0_IRQHandler (void) {PE_DEBUGHALT();}                               /* LPSPI0 Interrupt*/
    INT_SECTION void   LPSPI1_IRQHandler (void) {PE_DEBUGHALT();}                               /* LPSPI1 Interrupt*/
    INT_SECTION void   Reserved44_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 44*/
    INT_SECTION void   Reserved45_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 45*/
    INT_SECTION void   Reserved46_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 46*/
    INT_SECTION void   LPUART0_RxTx_IRQHandler (void) {PE_DEBUGHALT();}                         /* LPUART0 Transmit / Receive Interrupt*/
    INT_SECTION void   Reserved48_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 48*/
    INT_SECTION void   LPUART1_RxTx_IRQHandler (void) {PE_DEBUGHALT();}                         /* LPUART1 Transmit / Receive  Interrupt*/
    INT_SECTION void   Reserved50_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 50*/
    INT_SECTION void   Reserved51_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 51*/
    INT_SECTION void   Reserved52_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 52*/
    INT_SECTION void   Reserved53_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 53*/
    INT_SECTION void   Reserved54_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 54*/
    INT_SECTION void   ADC0_IRQHandler (void) {PE_DEBUGHALT();}                                 /* ADC0 interrupt request.*/
    INT_SECTION void   ADC1_IRQHandler (void) {PE_DEBUGHALT();}                                 /* ADC1 interrupt request.*/
    INT_SECTION void   CMP0_IRQHandler (void) {PE_DEBUGHALT();}                                 /* CMP0 interrupt request*/
    INT_SECTION void   Reserved58_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 58*/
    INT_SECTION void   Reserved59_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 59*/
    INT_SECTION void   ERM_single_fault_IRQHandler (void) {PE_DEBUGHALT();}                     /* ERM single bit error correction*/
    INT_SECTION void   ERM_double_fault_IRQHandler (void) {PE_DEBUGHALT();}                     /* ERM double bit error non-correctable*/
    INT_SECTION void   RTC_IRQHandler (void) {PE_DEBUGHALT();}                                  /* RTC alarm interrupt*/
    INT_SECTION void   RTC_Seconds_IRQHandler (void) {PE_DEBUGHALT();}                          /* RTC seconds interrupt*/
    INT_SECTION void   LPIT0_Ch0_IRQHandler (void) {PE_DEBUGHALT();}                            /* LPIT0 channel 0 overflow interrupt*/
    INT_SECTION void   LPIT0_Ch1_IRQHandler (void) {PE_DEBUGHALT();}                            /* LPIT0 channel 1 overflow interrupt*/
    INT_SECTION void   LPIT0_Ch2_IRQHandler (void) {PE_DEBUGHALT();}                            /* LPIT0 channel 2 overflow interrupt*/
    INT_SECTION void   LPIT0_Ch3_IRQHandler (void) {PE_DEBUGHALT();}                            /* LPIT0 channel 3 overflow interrupt*/
    INT_SECTION void   PDB0_IRQHandler (void) {PE_DEBUGHALT();}                                 /* PDB0 interrupt*/
    INT_SECTION void   Reserved69_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 69*/
    INT_SECTION void   Reserved70_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 70*/
    INT_SECTION void   Reserved71_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 71*/
    INT_SECTION void   Reserved72_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 72*/
    INT_SECTION void   SCG_IRQHandler (void) {PE_DEBUGHALT();}                                  /* SCG bus interrupt request*/
    INT_SECTION void   LPTMR0_IRQHandler (void) {PE_DEBUGHALT();}                               /* LPTIMER interrupt request*/
    INT_SECTION void   PORTA_IRQHandler (void) {PE_DEBUGHALT();}                                /* Port A pin detect interrupt*/
    INT_SECTION void   PORTB_IRQHandler (void) {PE_DEBUGHALT();}                                /* Port B pin detect interrupt*/
    INT_SECTION void   PORTC_IRQHandler (void) {PE_DEBUGHALT();}                                /* Port C pin detect interrupt*/
    INT_SECTION void   PORTD_IRQHandler (void) {PE_DEBUGHALT();}                                /* Port D pin detect interrupt*/
    INT_SECTION void   PORTE_IRQHandler (void) {PE_DEBUGHALT();}                                /* Port E pin detect interrupt*/
    INT_SECTION void   SWI_IRQHandler (void) {PE_DEBUGHALT();}                                  /* Software interrupt*/
    INT_SECTION void   Reserved81_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 81*/
    INT_SECTION void   Reserved82_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 82*/
    INT_SECTION void   Reserved83_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 83*/
    INT_SECTION void   PDB1_IRQHandler (void) {PE_DEBUGHALT();}                                 /* PDB1 interrupt*/
    INT_SECTION void   FLEXIO_IRQHandler (void) {PE_DEBUGHALT();}                               /* FlexIO Interrupt*/
    INT_SECTION void   Reserved86_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 86*/
    INT_SECTION void   Reserved87_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 87*/
    INT_SECTION void   Reserved88_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 88*/
    INT_SECTION void   Reserved89_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 89*/
    INT_SECTION void   Reserved90_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 90*/
    INT_SECTION void   Reserved91_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 91*/
    INT_SECTION void   Reserved92_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 92*/
    INT_SECTION void   Reserved93_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 93*/
    INT_SECTION void   CAN0_ORed_IRQHandler (void) {PE_DEBUGHALT();}                            /* CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    INT_SECTION void   CAN0_Error_IRQHandler (void) {PE_DEBUGHALT();}                           /* CAN0 Interrupt indicating that errors were detected on the CAN bus*/
    INT_SECTION void   CAN0_Wake_Up_IRQHandler (void) {PE_DEBUGHALT();}                         /* CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode*/
    INT_SECTION void   CAN0_ORed_0_15_MB_IRQHandler (void) {PE_DEBUGHALT();}                    /* CAN0 OR'ed Message buffer (0-15)*/
    INT_SECTION void   CAN0_ORed_16_31_MB_IRQHandler (void) {PE_DEBUGHALT();}                   /* CAN0 OR'ed Message buffer (16-31)*/
    INT_SECTION void   Reserved99_IRQHandler (void) {PE_DEBUGHALT();}                           /* Reserved Interrupt 99*/
    INT_SECTION void   Reserved100_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 100*/
    INT_SECTION void   CAN1_ORed_IRQHandler (void) {PE_DEBUGHALT();}                            /* CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    INT_SECTION void   CAN1_Error_IRQHandler (void) {PE_DEBUGHALT();}                           /* CAN1 Interrupt indicating that errors were detected on the CAN bus*/
    INT_SECTION void   Reserved103_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 103*/
    INT_SECTION void   CAN1_ORed_0_15_MB_IRQHandler (void) {PE_DEBUGHALT();}                    /* CAN1 OR'ed Interrupt for Message buffer (0-15)*/
    INT_SECTION void   Reserved105_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 105*/
    INT_SECTION void   Reserved106_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 106*/
    INT_SECTION void   Reserved107_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 107*/
    INT_SECTION void   Reserved108_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 108*/
    INT_SECTION void   Reserved109_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 109*/
    INT_SECTION void   Reserved110_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 110*/
    INT_SECTION void   Reserved111_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 111*/
    INT_SECTION void   Reserved112_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 112*/
    INT_SECTION void   Reserved113_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 113*/
    INT_SECTION void   Reserved114_IRQHandler (void) {PE_DEBUGHALT();}                          /* Reserved Interrupt 114*/
    INT_SECTION void   FTM0_Ch0_Ch1_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM0 Channel 0 and 1 interrupt*/
    INT_SECTION void   FTM0_Ch2_Ch3_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM0 Channel 2 and 3 interrupt*/
    INT_SECTION void   FTM0_Ch4_Ch5_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM0 Channel 4 and 5 interrupt*/
    INT_SECTION void   FTM0_Ch6_Ch7_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM0 Channel 6 and 7 interrupt*/
    INT_SECTION void   FTM0_Fault_IRQHandler (void) {PE_DEBUGHALT();}                           /* FTM0 Fault interrupt*/
    INT_SECTION void   FTM0_Ovf_Reload_IRQHandler (void) {ISR_Timer();}                      /* FTM0 Counter overflow and Reload interrupt*/
    INT_SECTION void   FTM1_Ch0_Ch1_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM1 Channel 0 and 1 interrupt*/
    INT_SECTION void   FTM1_Ch2_Ch3_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM1 Channel 2 and 3 interrupt*/
    INT_SECTION void   FTM1_Ch4_Ch5_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM1 Channel 4 and 5 interrupt*/
    INT_SECTION void   FTM1_Ch6_Ch7_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM1 Channel 6 and 7 interrupt*/
    INT_SECTION void   FTM1_Fault_IRQHandler (void) {PE_DEBUGHALT();}                           /* FTM1 Fault interrupt*/
    INT_SECTION void   FTM1_Ovf_Reload_IRQHandler (void) {PE_DEBUGHALT();}                      /* FTM1 Counter overflow and Reload interrupt*/
    INT_SECTION void   FTM2_Ch0_Ch1_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM2 Channel 0 and 1 interrupt*/
    INT_SECTION void   FTM2_Ch2_Ch3_IRQHandler (void) {ISR_HALL1_AB();}                         /* FTM2 Channel 2 and 3 interrupt*/
    INT_SECTION void   FTM2_Ch4_Ch5_IRQHandler (void) {ISR_HALL1_C();}                         /* FTM2 Channel 4 and 5 interrupt*/
    INT_SECTION void   FTM2_Ch6_Ch7_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM2 Channel 6 and 7 interrupt*/
    INT_SECTION void   FTM2_Fault_IRQHandler (void) {PE_DEBUGHALT();}                           /* FTM2 Fault interrupt*/
    INT_SECTION void   FTM2_Ovf_Reload_IRQHandler (void) {ISR_FTM2_Ovf();}                      /* FTM2 Counter overflow and Reload interrupt*/
    INT_SECTION void   FTM3_Ch0_Ch1_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM3 Channel 0 and 1 interrupt*/
    INT_SECTION void   FTM3_Ch2_Ch3_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM3 Channel 2 and 3 interrupt*/
    INT_SECTION void   FTM3_Ch4_Ch5_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM3 Channel 4 and 5 interrupt*/
    INT_SECTION void   FTM3_Ch6_Ch7_IRQHandler (void) {PE_DEBUGHALT();}                         /* FTM3 Channel 6 and 7 interrupt*/
    INT_SECTION void   FTM3_Fault_IRQHandler (void) {PE_DEBUGHALT();}                           /* FTM3 Fault interrupt*/
    INT_SECTION void   FTM3_Ovf_Reload_IRQHandler (void) {PE_DEBUGHALT();}                      /* FTM3 Counter overflow and Reload interrupt*/
	INT_SECTION void	Reserved139_IRQHandler (void) {PE_DEBUGHALT();}					/* 139*/
	INT_SECTION void	Reserved140_IRQHandler (void) {PE_DEBUGHALT();}					/* 140*/
	INT_SECTION void	Reserved141_IRQHandler (void) {PE_DEBUGHALT();}					/* 141*/
	INT_SECTION void	Reserved142_IRQHandler (void) {PE_DEBUGHALT();}					/* 142*/
	INT_SECTION void	Reserved143_IRQHandler (void) {PE_DEBUGHALT();}					/* 143*/
	INT_SECTION void	Reserved144_IRQHandler (void) {PE_DEBUGHALT();}					/* 144*/
	INT_SECTION void	Reserved145_IRQHandler (void) {PE_DEBUGHALT();}					/* 145*/
	INT_SECTION void	Reserved146_IRQHandler (void) {PE_DEBUGHALT();}					/* 146*/
	INT_SECTION void	Reserved147_IRQHandler (void) {PE_DEBUGHALT();}					/* 147*/
	INT_SECTION void	Reserved148_IRQHandler (void) {PE_DEBUGHALT();}					/* 148*/
	INT_SECTION void	Reserved149_IRQHandler (void) {PE_DEBUGHALT();}					/* 149*/
	INT_SECTION void	Reserved150_IRQHandler (void) {PE_DEBUGHALT();}					/* 150*/
	INT_SECTION void	Reserved151_IRQHandler (void) {PE_DEBUGHALT();}					/* 151*/
	INT_SECTION void	Reserved152_IRQHandler (void) {PE_DEBUGHALT();}					/* 152*/
	INT_SECTION void	Reserved153_IRQHandler (void) {PE_DEBUGHALT();}					/* 153*/
	INT_SECTION void	Reserved154_IRQHandler (void) {PE_DEBUGHALT();}					/* 154*/
	INT_SECTION void	Reserved155_IRQHandler (void) {PE_DEBUGHALT();}					/* 155*/
	INT_SECTION void	Reserved156_IRQHandler (void) {PE_DEBUGHALT();}					/* 156*/
	INT_SECTION void	Reserved157_IRQHandler (void) {PE_DEBUGHALT();}					/* 157*/
	INT_SECTION void	Reserved158_IRQHandler (void) {PE_DEBUGHALT();}					/* 158*/
	INT_SECTION void	Reserved159_IRQHandler (void) {PE_DEBUGHALT();}					/* 159*/
	INT_SECTION void	Reserved160_IRQHandler (void) {PE_DEBUGHALT();}					/* 160*/
	INT_SECTION void	Reserved161_IRQHandler (void) {PE_DEBUGHALT();}					/* 161*/
	INT_SECTION void	Reserved162_IRQHandler (void) {PE_DEBUGHALT();}					/* 162*/
	INT_SECTION void	Reserved163_IRQHandler (void) {PE_DEBUGHALT();}					/* 163*/
	INT_SECTION void	Reserved164_IRQHandler (void) {PE_DEBUGHALT();}					/* 164*/
	INT_SECTION void	Reserved165_IRQHandler (void) {PE_DEBUGHALT();}					/* 165*/
	INT_SECTION void	Reserved166_IRQHandler (void) {PE_DEBUGHALT();}					/* 166*/
	INT_SECTION void	Reserved167_IRQHandler (void) {PE_DEBUGHALT();}					/* 167*/
	INT_SECTION void	Reserved168_IRQHandler (void) {PE_DEBUGHALT();}					/* 168*/
	INT_SECTION void	Reserved169_IRQHandler (void) {PE_DEBUGHALT();}					/* 169*/
	INT_SECTION void	Reserved170_IRQHandler (void) {PE_DEBUGHALT();}					/* 170*/
	INT_SECTION void	Reserved171_IRQHandler (void) {PE_DEBUGHALT();}					/* 171*/
	INT_SECTION void	Reserved172_IRQHandler (void) {PE_DEBUGHALT();}					/* 172*/
	INT_SECTION void	Reserved173_IRQHandler (void) {PE_DEBUGHALT();}					/* 173*/
	INT_SECTION void	Reserved174_IRQHandler (void) {PE_DEBUGHALT();}					/* 174*/
	INT_SECTION void	Reserved175_IRQHandler (void) {PE_DEBUGHALT();}					/* 175*/
	INT_SECTION void	Reserved176_IRQHandler (void) {PE_DEBUGHALT();}					/* 176*/
	INT_SECTION void	Reserved177_IRQHandler (void) {PE_DEBUGHALT();}					/* 177*/
	INT_SECTION void	Reserved178_IRQHandler (void) {PE_DEBUGHALT();}					/* 178*/
	INT_SECTION void	Reserved179_IRQHandler (void) {PE_DEBUGHALT();}					/* 179*/
	INT_SECTION void	Reserved180_IRQHandler (void) {PE_DEBUGHALT();}					/* 180*/
	INT_SECTION void	Reserved181_IRQHandler (void) {PE_DEBUGHALT();}					/* 181*/
	INT_SECTION void	Reserved182_IRQHandler (void) {PE_DEBUGHALT();}					/* 182*/
	INT_SECTION void	Reserved183_IRQHandler (void) {PE_DEBUGHALT();}					/* 183*/
	INT_SECTION void	Reserved184_IRQHandler (void) {PE_DEBUGHALT();}					/* 184*/
	INT_SECTION void	Reserved185_IRQHandler (void) {PE_DEBUGHALT();}					/* 185*/
	INT_SECTION void	Reserved186_IRQHandler (void) {PE_DEBUGHALT();}					/* 186*/
	INT_SECTION void	Reserved187_IRQHandler (void) {PE_DEBUGHALT();}					/* 187*/
	INT_SECTION void	Reserved188_IRQHandler (void) {PE_DEBUGHALT();}					/* 188*/
	INT_SECTION void	Reserved189_IRQHandler (void) {PE_DEBUGHALT();}					/* 189*/
	INT_SECTION void	Reserved190_IRQHandler (void) {PE_DEBUGHALT();}					/* 190*/
	INT_SECTION void	Reserved191_IRQHandler (void) {PE_DEBUGHALT();}					/* 191*/
	INT_SECTION void	Reserved192_IRQHandler (void) {PE_DEBUGHALT();}					/* 192*/
	INT_SECTION void	Reserved193_IRQHandler (void) {PE_DEBUGHALT();}					/* 193*/
	INT_SECTION void	Reserved194_IRQHandler (void) {PE_DEBUGHALT();}					/* 194*/
	INT_SECTION void	Reserved195_IRQHandler (void) {PE_DEBUGHALT();}					/* 195*/
	INT_SECTION void	Reserved196_IRQHandler (void) {PE_DEBUGHALT();}					/* 196*/
	INT_SECTION void	Reserved197_IRQHandler (void) {PE_DEBUGHALT();}					/* 197*/
	INT_SECTION void	Reserved198_IRQHandler (void) {PE_DEBUGHALT();}					/* 198*/
	INT_SECTION void	Reserved199_IRQHandler (void) {PE_DEBUGHALT();}					/* 199*/
	INT_SECTION void	Reserved200_IRQHandler (void) {PE_DEBUGHALT();}					/* 200*/
	INT_SECTION void	Reserved201_IRQHandler (void) {PE_DEBUGHALT();}					/* 201*/
	INT_SECTION void	Reserved202_IRQHandler (void) {PE_DEBUGHALT();}					/* 202*/
	INT_SECTION void	Reserved203_IRQHandler (void) {PE_DEBUGHALT();}					/* 203*/
	INT_SECTION void	Reserved204_IRQHandler (void) {PE_DEBUGHALT();}					/* 204*/
	INT_SECTION void	Reserved205_IRQHandler (void) {PE_DEBUGHALT();}					/* 205*/
	INT_SECTION void	Reserved206_IRQHandler (void) {PE_DEBUGHALT();}					/* 206*/
	INT_SECTION void	Reserved207_IRQHandler (void) {PE_DEBUGHALT();}					/* 207*/
	INT_SECTION void	Reserved208_IRQHandler (void) {PE_DEBUGHALT();}					/* 208*/
	INT_SECTION void	Reserved209_IRQHandler (void) {PE_DEBUGHALT();}					/* 209*/
	INT_SECTION void	Reserved210_IRQHandler (void) {PE_DEBUGHALT();}					/* 210*/
	INT_SECTION void	Reserved211_IRQHandler (void) {PE_DEBUGHALT();}					/* 211*/
	INT_SECTION void	Reserved212_IRQHandler (void) {PE_DEBUGHALT();}					/* 212*/
	INT_SECTION void	Reserved213_IRQHandler (void) {PE_DEBUGHALT();}					/* 213*/
	INT_SECTION void	Reserved214_IRQHandler (void) {PE_DEBUGHALT();}					/* 214*/
	INT_SECTION void	Reserved215_IRQHandler (void) {PE_DEBUGHALT();}					/* 215*/
	INT_SECTION void	Reserved216_IRQHandler (void) {PE_DEBUGHALT();}					/* 216*/
	INT_SECTION void	Reserved217_IRQHandler (void) {PE_DEBUGHALT();}					/* 217*/
	INT_SECTION void	Reserved218_IRQHandler (void) {PE_DEBUGHALT();}					/* 218*/
	INT_SECTION void	Reserved219_IRQHandler (void) {PE_DEBUGHALT();}					/* 219*/
	INT_SECTION void	Reserved220_IRQHandler (void) {PE_DEBUGHALT();}					/* 220*/
	INT_SECTION void	Reserved221_IRQHandler (void) {PE_DEBUGHALT();}					/* 221*/
	INT_SECTION void	Reserved222_IRQHandler (void) {PE_DEBUGHALT();}					/* 222*/
	INT_SECTION void	Reserved223_IRQHandler (void) {PE_DEBUGHALT();}					/* 223*/
	INT_SECTION void	Reserved224_IRQHandler (void) {PE_DEBUGHALT();}					/* 224*/
	INT_SECTION void	Reserved225_IRQHandler (void) {PE_DEBUGHALT();}					/* 225*/
	INT_SECTION void	Reserved226_IRQHandler (void) {PE_DEBUGHALT();}					/* 226*/
	INT_SECTION void	Reserved227_IRQHandler (void) {PE_DEBUGHALT();}					/* 227*/
	INT_SECTION void	Reserved228_IRQHandler (void) {PE_DEBUGHALT();}					/* 228*/
	INT_SECTION void	Reserved229_IRQHandler (void) {PE_DEBUGHALT();}					/* 229*/
	INT_SECTION void	Reserved230_IRQHandler (void) {PE_DEBUGHALT();}					/* 230*/
	INT_SECTION void	Reserved231_IRQHandler (void) {PE_DEBUGHALT();}					/* 231*/
	INT_SECTION void	Reserved232_IRQHandler (void) {PE_DEBUGHALT();}					/* 232*/
	INT_SECTION void	Reserved233_IRQHandler (void) {PE_DEBUGHALT();}					/* 233*/
	INT_SECTION void	Reserved234_IRQHandler (void) {PE_DEBUGHALT();}					/* 234*/
	INT_SECTION void	Reserved235_IRQHandler (void) {PE_DEBUGHALT();}					/* 235*/
	INT_SECTION void	Reserved236_IRQHandler (void) {PE_DEBUGHALT();}					/* 236*/
	INT_SECTION void	Reserved237_IRQHandler (void) {PE_DEBUGHALT();}					/* 237*/
	INT_SECTION void	Reserved238_IRQHandler (void) {PE_DEBUGHALT();}					/* 238*/
	INT_SECTION void	Reserved239_IRQHandler (void) {PE_DEBUGHALT();}					/* 239*/
	INT_SECTION void	Reserved240_IRQHandler (void) {PE_DEBUGHALT();}					/* 240*/
	INT_SECTION void	Reserved241_IRQHandler (void) {PE_DEBUGHALT();}					/* 241*/
	INT_SECTION void	Reserved242_IRQHandler (void) {PE_DEBUGHALT();}					/* 242*/
	INT_SECTION void	Reserved243_IRQHandler (void) {PE_DEBUGHALT();}					/* 243*/
	INT_SECTION void	Reserved244_IRQHandler (void) {PE_DEBUGHALT();}					/* 244*/
	INT_SECTION void	Reserved245_IRQHandler (void) {PE_DEBUGHALT();}					/* 245*/
	INT_SECTION void	Reserved246_IRQHandler (void) {PE_DEBUGHALT();}					/* 246*/
	INT_SECTION void	Reserved247_IRQHandler (void) {PE_DEBUGHALT();}					/* 247*/
	INT_SECTION void	Reserved248_IRQHandler (void) {PE_DEBUGHALT();}					/* 248*/
	INT_SECTION void	Reserved249_IRQHandler (void) {PE_DEBUGHALT();}					/* 249*/
	INT_SECTION void	Reserved250_IRQHandler (void) {PE_DEBUGHALT();}					/* 250*/
	INT_SECTION void	Reserved251_IRQHandler (void) {PE_DEBUGHALT();}					/* 251*/
	INT_SECTION void	Reserved252_IRQHandler (void) {PE_DEBUGHALT();}					/* 252*/
	INT_SECTION void	Reserved253_IRQHandler (void) {PE_DEBUGHALT();}					/* 253*/
	INT_SECTION void	Reserved254_IRQHandler (void) {PE_DEBUGHALT();}					/* 254*/
