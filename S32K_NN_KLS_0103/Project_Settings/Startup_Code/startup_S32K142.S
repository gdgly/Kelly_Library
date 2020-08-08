/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_S32K142.s                                                           */
/*  @purpose: GNU Compiler Collection Startup File                                        */
/*            S32K142                                                                     */
/*  @version: 1.0                                                                         */
/*  @date:    2017-1-10                                                                   */
/*  @build:   b170107                                                                     */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright (c) 1997 - 2016 , Freescale Semiconductor, Inc.                              */
/* Copyright 2017 NXP                                                                     */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR                          */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES              */
/* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.                */
/* IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,                    */
/* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES                     */
/* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                     */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)                     */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,                    */
/* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING                  */
/* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF                         */
/* THE POSSIBILITY OF SUCH DAMAGE.                                                        */
/*****************************************************************************/
/* Version: GNU Compiler Collection                                          */
/*****************************************************************************/
    .syntax unified
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   Reserved7_IRQHandler                                               /* Reserved*/
    .long   Reserved8_IRQHandler                                               /* Reserved*/
    .long   Reserved9_IRQHandler                                               /* Reserved*/
    .long   Reserved10_IRQHandler                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   Reserved13_IRQHandler                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   DMA0_IRQHandler                                 /* DMA channel 0 transfer complete*/
    .long   DMA1_IRQHandler                                 /* DMA channel 1 transfer complete*/
    .long   DMA2_IRQHandler                                 /* DMA channel 2 transfer complete*/
    .long   DMA3_IRQHandler                                 /* DMA channel 3 transfer complete*/
    .long   DMA4_IRQHandler                                 /* DMA channel 4 transfer complete*/
    .long   DMA5_IRQHandler                                 /* DMA channel 5 transfer complete*/
    .long   DMA6_IRQHandler                                 /* DMA channel 6 transfer complete*/
    .long   DMA7_IRQHandler                                 /* DMA channel 7 transfer complete*/
    .long   DMA8_IRQHandler                                 /* DMA channel 8 transfer complete*/
    .long   DMA9_IRQHandler                                 /* DMA channel 9 transfer complete*/
    .long   DMA10_IRQHandler                                /* DMA channel 10 transfer complete*/
    .long   DMA11_IRQHandler                                /* DMA channel 11 transfer complete*/
    .long   DMA12_IRQHandler                                /* DMA channel 12 transfer complete*/
    .long   DMA13_IRQHandler                                /* DMA channel 13 transfer complete*/
    .long   DMA14_IRQHandler                                /* DMA channel 14 transfer complete*/
    .long   DMA15_IRQHandler                                /* DMA channel 15 transfer complete*/
    .long   DMA_Error_IRQHandler                            /* DMA error interrupt channels 0-15*/
    .long   MCM_IRQHandler                                  /* FPU sources*/
    .long   FTFC_IRQHandler                                 /* FTFC Command complete*/
    .long   Read_Collision_IRQHandler                       /* FTFC Read collision*/
    .long   LVD_LVW_IRQHandler                              /* PMC Low voltage detect interrupt*/
    .long   FTFC_Fault_IRQHandler                           /* FTFC Double bit fault detect*/
    .long   WDOG_EWM_IRQHandler                             /* Single interrupt vector for WDOG and EWM*/
    .long   RCM_IRQHandler                                  /* RCM Asynchronous Interrupt*/
    .long   LPI2C0_Master_IRQHandler                        /* LPI2C0 Master Interrupt*/
    .long   LPI2C0_Slave_IRQHandler                         /* LPI2C0 Slave Interrupt*/
    .long   LPSPI0_IRQHandler                               /* LPSPI0 Interrupt*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 Interrupt*/
    .long   Reserved44_IRQHandler                           /* Reserved Interrupt 44*/
    .long   Reserved45_IRQHandler                           /* Reserved Interrupt 45*/
    .long   Reserved46_IRQHandler                           /* Reserved Interrupt 46*/
    .long   LPUART0_RxTx_IRQHandler                         /* LPUART0 Transmit / Receive Interrupt*/
    .long   Reserved48_IRQHandler                           /* Reserved Interrupt 48*/
    .long   LPUART1_RxTx_IRQHandler                         /* LPUART1 Transmit / Receive  Interrupt*/
    .long   Reserved50_IRQHandler                           /* Reserved Interrupt 50*/
    .long   Reserved51_IRQHandler                           /* Reserved Interrupt 51*/
    .long   Reserved52_IRQHandler                           /* Reserved Interrupt 52*/
    .long   Reserved53_IRQHandler                           /* Reserved Interrupt 53*/
    .long   Reserved54_IRQHandler                           /* Reserved Interrupt 54*/
    .long   ADC0_IRQHandler                                 /* ADC0 interrupt request.*/
    .long   ADC1_IRQHandler                                 /* ADC1 interrupt request.*/
    .long   CMP0_IRQHandler                                 /* CMP0 interrupt request*/
    .long   Reserved58_IRQHandler                           /* Reserved Interrupt 58*/
    .long   Reserved59_IRQHandler                           /* Reserved Interrupt 59*/
    .long   ERM_single_fault_IRQHandler                     /* ERM single bit error correction*/
    .long   ERM_double_fault_IRQHandler                     /* ERM double bit error non-correctable*/
    .long   RTC_IRQHandler                                  /* RTC alarm interrupt*/
    .long   RTC_Seconds_IRQHandler                          /* RTC seconds interrupt*/
    .long   LPIT0_Ch0_IRQHandler                            /* LPIT0 channel 0 overflow interrupt*/
    .long   LPIT0_Ch1_IRQHandler                            /* LPIT0 channel 1 overflow interrupt*/
    .long   LPIT0_Ch2_IRQHandler                            /* LPIT0 channel 2 overflow interrupt*/
    .long   LPIT0_Ch3_IRQHandler                            /* LPIT0 channel 3 overflow interrupt*/
    .long   PDB0_IRQHandler                                 /* PDB0 interrupt*/
    .long   Reserved69_IRQHandler                           /* Reserved Interrupt 69*/
    .long   Reserved70_IRQHandler                           /* Reserved Interrupt 70*/
    .long   Reserved71_IRQHandler                           /* Reserved Interrupt 71*/
    .long   Reserved72_IRQHandler                           /* Reserved Interrupt 72*/
    .long   SCG_IRQHandler                                  /* SCG bus interrupt request*/
    .long   LPTMR0_IRQHandler                               /* LPTIMER interrupt request*/
    .long   PORTA_IRQHandler                                /* Port A pin detect interrupt*/
    .long   PORTB_IRQHandler                                /* Port B pin detect interrupt*/
    .long   PORTC_IRQHandler                                /* Port C pin detect interrupt*/
    .long   PORTD_IRQHandler                                /* Port D pin detect interrupt*/
    .long   PORTE_IRQHandler                                /* Port E pin detect interrupt*/
    .long   SWI_IRQHandler                                  /* Software interrupt*/
    .long   Reserved81_IRQHandler                           /* Reserved Interrupt 81*/
    .long   Reserved82_IRQHandler                           /* Reserved Interrupt 82*/
    .long   Reserved83_IRQHandler                           /* Reserved Interrupt 83*/
    .long   PDB1_IRQHandler                                 /* PDB1 interrupt*/
    .long   FLEXIO_IRQHandler                               /* FlexIO Interrupt*/
    .long   Reserved86_IRQHandler                           /* Reserved Interrupt 86*/
    .long   Reserved87_IRQHandler                           /* Reserved Interrupt 87*/
    .long   Reserved88_IRQHandler                           /* Reserved Interrupt 88*/
    .long   Reserved89_IRQHandler                           /* Reserved Interrupt 89*/
    .long   Reserved90_IRQHandler                           /* Reserved Interrupt 90*/
    .long   Reserved91_IRQHandler                           /* Reserved Interrupt 91*/
    .long   Reserved92_IRQHandler                           /* Reserved Interrupt 92*/
    .long   Reserved93_IRQHandler                           /* Reserved Interrupt 93*/
    .long   CAN0_ORed_IRQHandler                            /* CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    .long   CAN0_Error_IRQHandler                           /* CAN0 Interrupt indicating that errors were detected on the CAN bus*/
    .long   CAN0_Wake_Up_IRQHandler                         /* CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode*/
    .long   CAN0_ORed_0_15_MB_IRQHandler                    /* CAN0 OR'ed Message buffer (0-15)*/
    .long   CAN0_ORed_16_31_MB_IRQHandler                   /* CAN0 OR'ed Message buffer (16-31)*/
    .long   Reserved99_IRQHandler                           /* Reserved Interrupt 99*/
    .long   Reserved100_IRQHandler                          /* Reserved Interrupt 100*/
    .long   CAN1_ORed_IRQHandler                            /* CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    .long   CAN1_Error_IRQHandler                           /* CAN1 Interrupt indicating that errors were detected on the CAN bus*/
    .long   Reserved103_IRQHandler                          /* Reserved Interrupt 103*/
    .long   CAN1_ORed_0_15_MB_IRQHandler                    /* CAN1 OR'ed Interrupt for Message buffer (0-15)*/
    .long   Reserved105_IRQHandler                          /* Reserved Interrupt 105*/
    .long   Reserved106_IRQHandler                          /* Reserved Interrupt 106*/
    .long   Reserved107_IRQHandler                          /* Reserved Interrupt 107*/
    .long   Reserved108_IRQHandler                          /* Reserved Interrupt 108*/
    .long   Reserved109_IRQHandler                          /* Reserved Interrupt 109*/
    .long   Reserved110_IRQHandler                          /* Reserved Interrupt 110*/
    .long   Reserved111_IRQHandler                          /* Reserved Interrupt 111*/
    .long   Reserved112_IRQHandler                          /* Reserved Interrupt 112*/
    .long   Reserved113_IRQHandler                          /* Reserved Interrupt 113*/
    .long   Reserved114_IRQHandler                          /* Reserved Interrupt 114*/
    .long   FTM0_Ch0_Ch1_IRQHandler                         /* FTM0 Channel 0 and 1 interrupt*/
    .long   FTM0_Ch2_Ch3_IRQHandler                         /* FTM0 Channel 2 and 3 interrupt*/
    .long   FTM0_Ch4_Ch5_IRQHandler                         /* FTM0 Channel 4 and 5 interrupt*/
    .long   FTM0_Ch6_Ch7_IRQHandler                         /* FTM0 Channel 6 and 7 interrupt*/
    .long   FTM0_Fault_IRQHandler                           /* FTM0 Fault interrupt*/
    .long   FTM0_Ovf_Reload_IRQHandler                      /* FTM0 Counter overflow and Reload interrupt*/
    .long   FTM1_Ch0_Ch1_IRQHandler                         /* FTM1 Channel 0 and 1 interrupt*/
    .long   FTM1_Ch2_Ch3_IRQHandler                         /* FTM1 Channel 2 and 3 interrupt*/
    .long   FTM1_Ch4_Ch5_IRQHandler                         /* FTM1 Channel 4 and 5 interrupt*/
    .long   FTM1_Ch6_Ch7_IRQHandler                         /* FTM1 Channel 6 and 7 interrupt*/
    .long   FTM1_Fault_IRQHandler                           /* FTM1 Fault interrupt*/
    .long   FTM1_Ovf_Reload_IRQHandler                      /* FTM1 Counter overflow and Reload interrupt*/
    .long   FTM2_Ch0_Ch1_IRQHandler                         /* FTM2 Channel 0 and 1 interrupt*/
    .long   FTM2_Ch2_Ch3_IRQHandler                         /* FTM2 Channel 2 and 3 interrupt*/
    .long   FTM2_Ch4_Ch5_IRQHandler                         /* FTM2 Channel 4 and 5 interrupt*/
    .long   FTM2_Ch6_Ch7_IRQHandler                         /* FTM2 Channel 6 and 7 interrupt*/
    .long   FTM2_Fault_IRQHandler                           /* FTM2 Fault interrupt*/
    .long   FTM2_Ovf_Reload_IRQHandler                      /* FTM2 Counter overflow and Reload interrupt*/
    .long   FTM3_Ch0_Ch1_IRQHandler                         /* FTM3 Channel 0 and 1 interrupt*/
    .long   FTM3_Ch2_Ch3_IRQHandler                         /* FTM3 Channel 2 and 3 interrupt*/
    .long   FTM3_Ch4_Ch5_IRQHandler                         /* FTM3 Channel 4 and 5 interrupt*/
    .long   FTM3_Ch6_Ch7_IRQHandler                         /* FTM3 Channel 6 and 7 interrupt*/
    .long   FTM3_Fault_IRQHandler                           /* FTM3 Fault interrupt*/
    .long   FTM3_Ovf_Reload_IRQHandler                      /* FTM3 Counter overflow and Reload interrupt*/
	.long	Reserved139_IRQHandler					/* 139*/
	.long	Reserved140_IRQHandler					/* 140*/
	.long	Reserved141_IRQHandler					/* 141*/
	.long	Reserved142_IRQHandler					/* 142*/
	.long	Reserved143_IRQHandler					/* 143*/
	.long	Reserved144_IRQHandler					/* 144*/
	.long	Reserved145_IRQHandler					/* 145*/
	.long	Reserved146_IRQHandler					/* 146*/
	.long	Reserved147_IRQHandler					/* 147*/
	.long	Reserved148_IRQHandler					/* 148*/
	.long	Reserved149_IRQHandler					/* 149*/
	.long	Reserved150_IRQHandler					/* 150*/
	.long	Reserved151_IRQHandler					/* 151*/
	.long	Reserved152_IRQHandler					/* 152*/
	.long	Reserved153_IRQHandler					/* 153*/
	.long	Reserved154_IRQHandler					/* 154*/
	.long	Reserved155_IRQHandler					/* 155*/
	.long	Reserved156_IRQHandler					/* 156*/
	.long	Reserved157_IRQHandler					/* 157*/
	.long	Reserved158_IRQHandler					/* 158*/
	.long	Reserved159_IRQHandler					/* 159*/
	.long	Reserved160_IRQHandler					/* 160*/
	.long	Reserved161_IRQHandler					/* 161*/
	.long	Reserved162_IRQHandler					/* 162*/
	.long	Reserved163_IRQHandler					/* 163*/
	.long	Reserved164_IRQHandler					/* 164*/
	.long	Reserved165_IRQHandler					/* 165*/
	.long	Reserved166_IRQHandler					/* 166*/
	.long	Reserved167_IRQHandler					/* 167*/
	.long	Reserved168_IRQHandler					/* 168*/
	.long	Reserved169_IRQHandler					/* 169*/
	.long	Reserved170_IRQHandler					/* 170*/
	.long	Reserved171_IRQHandler					/* 171*/
	.long	Reserved172_IRQHandler					/* 172*/
	.long	Reserved173_IRQHandler					/* 173*/
	.long	Reserved174_IRQHandler					/* 174*/
	.long	Reserved175_IRQHandler					/* 175*/
	.long	Reserved176_IRQHandler					/* 176*/
	.long	Reserved177_IRQHandler					/* 177*/
	.long	Reserved178_IRQHandler					/* 178*/
	.long	Reserved179_IRQHandler					/* 179*/
	.long	Reserved180_IRQHandler					/* 180*/
	.long	Reserved181_IRQHandler					/* 181*/
	.long	Reserved182_IRQHandler					/* 182*/
	.long	Reserved183_IRQHandler					/* 183*/
	.long	Reserved184_IRQHandler					/* 184*/
	.long	Reserved185_IRQHandler					/* 185*/
	.long	Reserved186_IRQHandler					/* 186*/
	.long	Reserved187_IRQHandler					/* 187*/
	.long	Reserved188_IRQHandler					/* 188*/
	.long	Reserved189_IRQHandler					/* 189*/
	.long	Reserved190_IRQHandler					/* 190*/
	.long	Reserved191_IRQHandler					/* 191*/
	.long	Reserved192_IRQHandler					/* 192*/
	.long	Reserved193_IRQHandler					/* 193*/
	.long	Reserved194_IRQHandler					/* 194*/
	.long	Reserved195_IRQHandler					/* 195*/
	.long	Reserved196_IRQHandler					/* 196*/
	.long	Reserved197_IRQHandler					/* 197*/
	.long	Reserved198_IRQHandler					/* 198*/
	.long	Reserved199_IRQHandler					/* 199*/
	.long	Reserved200_IRQHandler					/* 200*/
	.long	Reserved201_IRQHandler					/* 201*/
	.long	Reserved202_IRQHandler					/* 202*/
	.long	Reserved203_IRQHandler					/* 203*/
	.long	Reserved204_IRQHandler					/* 204*/
	.long	Reserved205_IRQHandler					/* 205*/
	.long	Reserved206_IRQHandler					/* 206*/
	.long	Reserved207_IRQHandler					/* 207*/
	.long	Reserved208_IRQHandler					/* 208*/
	.long	Reserved209_IRQHandler					/* 209*/
	.long	Reserved210_IRQHandler					/* 210*/
	.long	Reserved211_IRQHandler					/* 211*/
	.long	Reserved212_IRQHandler					/* 212*/
	.long	Reserved213_IRQHandler					/* 213*/
	.long	Reserved214_IRQHandler					/* 214*/
	.long	Reserved215_IRQHandler					/* 215*/
	.long	Reserved216_IRQHandler					/* 216*/
	.long	Reserved217_IRQHandler					/* 217*/
	.long	Reserved218_IRQHandler					/* 218*/
	.long	Reserved219_IRQHandler					/* 219*/
	.long	Reserved220_IRQHandler					/* 220*/
	.long	Reserved221_IRQHandler					/* 221*/
	.long	Reserved222_IRQHandler					/* 222*/
	.long	Reserved223_IRQHandler					/* 223*/
	.long	Reserved224_IRQHandler					/* 224*/
	.long	Reserved225_IRQHandler					/* 225*/
	.long	Reserved226_IRQHandler					/* 226*/
	.long	Reserved227_IRQHandler					/* 227*/
	.long	Reserved228_IRQHandler					/* 228*/
	.long	Reserved229_IRQHandler					/* 229*/
	.long	Reserved230_IRQHandler					/* 230*/
	.long	Reserved231_IRQHandler					/* 231*/
	.long	Reserved232_IRQHandler					/* 232*/
	.long	Reserved233_IRQHandler					/* 233*/
	.long	Reserved234_IRQHandler					/* 234*/
	.long	Reserved235_IRQHandler					/* 235*/
	.long	Reserved236_IRQHandler					/* 236*/
	.long	Reserved237_IRQHandler					/* 237*/
	.long	Reserved238_IRQHandler					/* 238*/
	.long	Reserved239_IRQHandler					/* 239*/
	.long	Reserved240_IRQHandler					/* 240*/
	.long	Reserved241_IRQHandler					/* 241*/
	.long	Reserved242_IRQHandler					/* 242*/
	.long	Reserved243_IRQHandler					/* 243*/
	.long	Reserved244_IRQHandler					/* 244*/
	.long	Reserved245_IRQHandler					/* 245*/
	.long	Reserved246_IRQHandler					/* 246*/
	.long	Reserved247_IRQHandler					/* 247*/
	.long	Reserved248_IRQHandler					/* 248*/
	.long	Reserved249_IRQHandler					/* 249*/
	.long	Reserved250_IRQHandler					/* 250*/
	.long	Reserved251_IRQHandler					/* 251*/
	.long	Reserved252_IRQHandler					/* 252*/
	.long	Reserved253_IRQHandler					/* 253*/
	.long	Reserved254_IRQHandler					/* 254*/
    .long   0xFFFFFFFF                                      /*  Reserved for user TRIM value*/

    .size    __isr_vector, . - __isr_vector

/* Flash Configuration */
    .section .FlashConfig, "a"
    .long 0xFFFFFFFF     /* 8 bytes backdoor comparison key           */
    .long 0xFFFFFFFF     /*                                           */
    .long 0xFFFFFFFF     /* 4 bytes program flash protection bytes    */
    .long 0xFFFF7FFE     /* FDPROT:FEPROT:FOPT:FSEC(0xFE = unsecured) */

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */

    /* Init the rest of the registers */
    ldr     r1,=0
    ldr     r2,=0
    ldr     r3,=0
    ldr     r4,=0
    ldr     r5,=0
    ldr     r6,=0
    ldr     r7,=0
    mov     r8,r7
    mov     r9,r7
    mov     r10,r7
    mov     r11,r7
    mov     r12,r7

#ifdef START_FROM_FLASH

    /* Init ECC RAM */

    ldr r1, =__RAM_START
    ldr r2, =__RAM_END

    subs    r2, r1
    subs    r2, #1
    ble .LC5

    movs    r0, 0
    movs    r3, #4
.LC4:
    str r0, [r1]
    add	r1, r1, r3
    subs r2, 4
    bge .LC4
.LC5:
#endif

    /* Initialize the stack pointer */
    ldr     r0,=__StackTop
    mov     r13,r0

#ifndef __NO_SYSTEM_INIT
    /* Call the system init routine */
    ldr     r0,=SystemInit
    blx     r0
#endif

    /* Init .data and .bss sections */
    ldr     r0,=init_data_bss
    blx     r0
    cpsie   i               /* Unmask interrupts */
    bl      main
JumpToSelf:
    b       JumpToSelf

    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    b       DefaultISR
    .size DefaultISR, . - DefaultISR

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler	handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler   NMI_Handler                                     /* NMI Handler*/
    def_irq_handler   HardFault_Handler                               /* Hard Fault Handler*/
    def_irq_handler   MemManage_Handler                               /* MPU Fault Handler*/
    def_irq_handler   BusFault_Handler                                /* Bus Fault Handler*/
    def_irq_handler   UsageFault_Handler                              /* Usage Fault Handler*/
    def_irq_handler   Reserved7_IRQHandler                                               /* Reserved*/
    def_irq_handler   Reserved8_IRQHandler                                               /* Reserved*/
    def_irq_handler   Reserved9_IRQHandler                                               /* Reserved*/
    def_irq_handler   Reserved10_IRQHandler                                               /* Reserved*/
    def_irq_handler   SVC_Handler                                     /* SVCall Handler*/
    def_irq_handler   DebugMon_Handler                                /* Debug Monitor Handler*/
    def_irq_handler   Reserved13_IRQHandler                                               /* Reserved*/
    def_irq_handler   PendSV_Handler                                  /* PendSV Handler*/
    def_irq_handler   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    def_irq_handler   DMA0_IRQHandler                                 /* DMA channel 0 transfer complete*/
    def_irq_handler   DMA1_IRQHandler                                 /* DMA channel 1 transfer complete*/
    def_irq_handler   DMA2_IRQHandler                                 /* DMA channel 2 transfer complete*/
    def_irq_handler   DMA3_IRQHandler                                 /* DMA channel 3 transfer complete*/
    def_irq_handler   DMA4_IRQHandler                                 /* DMA channel 4 transfer complete*/
    def_irq_handler   DMA5_IRQHandler                                 /* DMA channel 5 transfer complete*/
    def_irq_handler   DMA6_IRQHandler                                 /* DMA channel 6 transfer complete*/
    def_irq_handler   DMA7_IRQHandler                                 /* DMA channel 7 transfer complete*/
    def_irq_handler   DMA8_IRQHandler                                 /* DMA channel 8 transfer complete*/
    def_irq_handler   DMA9_IRQHandler                                 /* DMA channel 9 transfer complete*/
    def_irq_handler   DMA10_IRQHandler                                /* DMA channel 10 transfer complete*/
    def_irq_handler   DMA11_IRQHandler                                /* DMA channel 11 transfer complete*/
    def_irq_handler   DMA12_IRQHandler                                /* DMA channel 12 transfer complete*/
    def_irq_handler   DMA13_IRQHandler                                /* DMA channel 13 transfer complete*/
    def_irq_handler   DMA14_IRQHandler                                /* DMA channel 14 transfer complete*/
    def_irq_handler   DMA15_IRQHandler                                /* DMA channel 15 transfer complete*/
    def_irq_handler   DMA_Error_IRQHandler                            /* DMA error interrupt channels 0-15*/
    def_irq_handler   MCM_IRQHandler                                  /* FPU sources*/
    def_irq_handler   FTFC_IRQHandler                                 /* FTFC Command complete*/
    def_irq_handler   Read_Collision_IRQHandler                       /* FTFC Read collision*/
    def_irq_handler   LVD_LVW_IRQHandler                              /* PMC Low voltage detect interrupt*/
    def_irq_handler   FTFC_Fault_IRQHandler                           /* FTFC Double bit fault detect*/
    def_irq_handler   WDOG_EWM_IRQHandler                             /* Single interrupt vector for WDOG and EWM*/
    def_irq_handler   RCM_IRQHandler                                  /* RCM Asynchronous Interrupt*/
    def_irq_handler   LPI2C0_Master_IRQHandler                        /* LPI2C0 Master Interrupt*/
    def_irq_handler   LPI2C0_Slave_IRQHandler                         /* LPI2C0 Slave Interrupt*/
    def_irq_handler   LPSPI0_IRQHandler                               /* LPSPI0 Interrupt*/
    def_irq_handler   LPSPI1_IRQHandler                               /* LPSPI1 Interrupt*/
    def_irq_handler   Reserved44_IRQHandler                           /* Reserved Interrupt 44*/
    def_irq_handler   Reserved45_IRQHandler                           /* Reserved Interrupt 45*/
    def_irq_handler   Reserved46_IRQHandler                           /* Reserved Interrupt 46*/
    def_irq_handler   LPUART0_RxTx_IRQHandler                         /* LPUART0 Transmit / Receive Interrupt*/
    def_irq_handler   Reserved48_IRQHandler                           /* Reserved Interrupt 48*/
    def_irq_handler   LPUART1_RxTx_IRQHandler                         /* LPUART1 Transmit / Receive  Interrupt*/
    def_irq_handler   Reserved50_IRQHandler                           /* Reserved Interrupt 50*/
    def_irq_handler   Reserved51_IRQHandler                           /* Reserved Interrupt 51*/
    def_irq_handler   Reserved52_IRQHandler                           /* Reserved Interrupt 52*/
    def_irq_handler   Reserved53_IRQHandler                           /* Reserved Interrupt 53*/
    def_irq_handler   Reserved54_IRQHandler                           /* Reserved Interrupt 54*/
    def_irq_handler   ADC0_IRQHandler                                 /* ADC0 interrupt request.*/
    def_irq_handler   ADC1_IRQHandler                                 /* ADC1 interrupt request.*/
    def_irq_handler   CMP0_IRQHandler                                 /* CMP0 interrupt request*/
    def_irq_handler   Reserved58_IRQHandler                           /* Reserved Interrupt 58*/
    def_irq_handler   Reserved59_IRQHandler                           /* Reserved Interrupt 59*/
    def_irq_handler   ERM_single_fault_IRQHandler                     /* ERM single bit error correction*/
    def_irq_handler   ERM_double_fault_IRQHandler                     /* ERM double bit error non-correctable*/
    def_irq_handler   RTC_IRQHandler                                  /* RTC alarm interrupt*/
    def_irq_handler   RTC_Seconds_IRQHandler                          /* RTC seconds interrupt*/
    def_irq_handler   LPIT0_Ch0_IRQHandler                            /* LPIT0 channel 0 overflow interrupt*/
    def_irq_handler   LPIT0_Ch1_IRQHandler                            /* LPIT0 channel 1 overflow interrupt*/
    def_irq_handler   LPIT0_Ch2_IRQHandler                            /* LPIT0 channel 2 overflow interrupt*/
    def_irq_handler   LPIT0_Ch3_IRQHandler                            /* LPIT0 channel 3 overflow interrupt*/
    def_irq_handler   PDB0_IRQHandler                                 /* PDB0 interrupt*/
    def_irq_handler   Reserved69_IRQHandler                           /* Reserved Interrupt 69*/
    def_irq_handler   Reserved70_IRQHandler                           /* Reserved Interrupt 70*/
    def_irq_handler   Reserved71_IRQHandler                           /* Reserved Interrupt 71*/
    def_irq_handler   Reserved72_IRQHandler                           /* Reserved Interrupt 72*/
    def_irq_handler   SCG_IRQHandler                                  /* SCG bus interrupt request*/
    def_irq_handler   LPTMR0_IRQHandler                               /* LPTIMER interrupt request*/
    def_irq_handler   PORTA_IRQHandler                                /* Port A pin detect interrupt*/
    def_irq_handler   PORTB_IRQHandler                                /* Port B pin detect interrupt*/
    def_irq_handler   PORTC_IRQHandler                                /* Port C pin detect interrupt*/
    def_irq_handler   PORTD_IRQHandler                                /* Port D pin detect interrupt*/
    def_irq_handler   PORTE_IRQHandler                                /* Port E pin detect interrupt*/
    def_irq_handler   SWI_IRQHandler                                  /* Software interrupt*/
    def_irq_handler   Reserved81_IRQHandler                           /* Reserved Interrupt 81*/
    def_irq_handler   Reserved82_IRQHandler                           /* Reserved Interrupt 82*/
    def_irq_handler   Reserved83_IRQHandler                           /* Reserved Interrupt 83*/
    def_irq_handler   PDB1_IRQHandler                                 /* PDB1 interrupt*/
    def_irq_handler   FLEXIO_IRQHandler                               /* FlexIO Interrupt*/
    def_irq_handler   Reserved86_IRQHandler                           /* Reserved Interrupt 86*/
    def_irq_handler   Reserved87_IRQHandler                           /* Reserved Interrupt 87*/
    def_irq_handler   Reserved88_IRQHandler                           /* Reserved Interrupt 88*/
    def_irq_handler   Reserved89_IRQHandler                           /* Reserved Interrupt 89*/
    def_irq_handler   Reserved90_IRQHandler                           /* Reserved Interrupt 90*/
    def_irq_handler   Reserved91_IRQHandler                           /* Reserved Interrupt 91*/
    def_irq_handler   Reserved92_IRQHandler                           /* Reserved Interrupt 92*/
    def_irq_handler   Reserved93_IRQHandler                           /* Reserved Interrupt 93*/
    def_irq_handler   CAN0_ORed_IRQHandler                            /* CAN0 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    def_irq_handler   CAN0_Error_IRQHandler                           /* CAN0 Interrupt indicating that errors were detected on the CAN bus*/
    def_irq_handler   CAN0_Wake_Up_IRQHandler                         /* CAN0 Interrupt asserted when Pretended Networking operation is enabled, and a valid message matches the selected filter criteria during Low Power mode*/
    def_irq_handler   CAN0_ORed_0_15_MB_IRQHandler                    /* CAN0 OR'ed Message buffer (0-15)*/
    def_irq_handler   CAN0_ORed_16_31_MB_IRQHandler                   /* CAN0 OR'ed Message buffer (16-31)*/
    def_irq_handler   Reserved99_IRQHandler                           /* Reserved Interrupt 99*/
    def_irq_handler   Reserved100_IRQHandler                          /* Reserved Interrupt 100*/
    def_irq_handler   CAN1_ORed_IRQHandler                            /* CAN1 OR'ed [Bus Off OR Transmit Warning OR Receive Warning]*/
    def_irq_handler   CAN1_Error_IRQHandler                           /* CAN1 Interrupt indicating that errors were detected on the CAN bus*/
    def_irq_handler   Reserved103_IRQHandler                          /* Reserved Interrupt 103*/
    def_irq_handler   CAN1_ORed_0_15_MB_IRQHandler                    /* CAN1 OR'ed Interrupt for Message buffer (0-15)*/
    def_irq_handler   Reserved105_IRQHandler                          /* Reserved Interrupt 105*/
    def_irq_handler   Reserved106_IRQHandler                          /* Reserved Interrupt 106*/
    def_irq_handler   Reserved107_IRQHandler                          /* Reserved Interrupt 107*/
    def_irq_handler   Reserved108_IRQHandler                          /* Reserved Interrupt 108*/
    def_irq_handler   Reserved109_IRQHandler                          /* Reserved Interrupt 109*/
    def_irq_handler   Reserved110_IRQHandler                          /* Reserved Interrupt 110*/
    def_irq_handler   Reserved111_IRQHandler                          /* Reserved Interrupt 111*/
    def_irq_handler   Reserved112_IRQHandler                          /* Reserved Interrupt 112*/
    def_irq_handler   Reserved113_IRQHandler                          /* Reserved Interrupt 113*/
    def_irq_handler   Reserved114_IRQHandler                          /* Reserved Interrupt 114*/
    def_irq_handler   FTM0_Ch0_Ch1_IRQHandler                         /* FTM0 Channel 0 and 1 interrupt*/
    def_irq_handler   FTM0_Ch2_Ch3_IRQHandler                         /* FTM0 Channel 2 and 3 interrupt*/
    def_irq_handler   FTM0_Ch4_Ch5_IRQHandler                         /* FTM0 Channel 4 and 5 interrupt*/
    def_irq_handler   FTM0_Ch6_Ch7_IRQHandler                         /* FTM0 Channel 6 and 7 interrupt*/
    def_irq_handler   FTM0_Fault_IRQHandler                           /* FTM0 Fault interrupt*/
    def_irq_handler   FTM0_Ovf_Reload_IRQHandler                      /* FTM0 Counter overflow and Reload interrupt*/
    def_irq_handler   FTM1_Ch0_Ch1_IRQHandler                         /* FTM1 Channel 0 and 1 interrupt*/
    def_irq_handler   FTM1_Ch2_Ch3_IRQHandler                         /* FTM1 Channel 2 and 3 interrupt*/
    def_irq_handler   FTM1_Ch4_Ch5_IRQHandler                         /* FTM1 Channel 4 and 5 interrupt*/
    def_irq_handler   FTM1_Ch6_Ch7_IRQHandler                         /* FTM1 Channel 6 and 7 interrupt*/
    def_irq_handler   FTM1_Fault_IRQHandler                           /* FTM1 Fault interrupt*/
    def_irq_handler   FTM1_Ovf_Reload_IRQHandler                      /* FTM1 Counter overflow and Reload interrupt*/
    def_irq_handler   FTM2_Ch0_Ch1_IRQHandler                         /* FTM2 Channel 0 and 1 interrupt*/
    def_irq_handler   FTM2_Ch2_Ch3_IRQHandler                         /* FTM2 Channel 2 and 3 interrupt*/
    def_irq_handler   FTM2_Ch4_Ch5_IRQHandler                         /* FTM2 Channel 4 and 5 interrupt*/
    def_irq_handler   FTM2_Ch6_Ch7_IRQHandler                         /* FTM2 Channel 6 and 7 interrupt*/
    def_irq_handler   FTM2_Fault_IRQHandler                           /* FTM2 Fault interrupt*/
    def_irq_handler   FTM2_Ovf_Reload_IRQHandler                      /* FTM2 Counter overflow and Reload interrupt*/
    def_irq_handler   FTM3_Ch0_Ch1_IRQHandler                         /* FTM3 Channel 0 and 1 interrupt*/
    def_irq_handler   FTM3_Ch2_Ch3_IRQHandler                         /* FTM3 Channel 2 and 3 interrupt*/
    def_irq_handler   FTM3_Ch4_Ch5_IRQHandler                         /* FTM3 Channel 4 and 5 interrupt*/
    def_irq_handler   FTM3_Ch6_Ch7_IRQHandler                         /* FTM3 Channel 6 and 7 interrupt*/
    def_irq_handler   FTM3_Fault_IRQHandler                           /* FTM3 Fault interrupt*/
    def_irq_handler   FTM3_Ovf_Reload_IRQHandler                      /* FTM3 Counter overflow and Reload interrupt*/
	def_irq_handler	Reserved139_IRQHandler					/* 139*/
	def_irq_handler	Reserved140_IRQHandler					/* 140*/
	def_irq_handler	Reserved141_IRQHandler					/* 141*/
	def_irq_handler	Reserved142_IRQHandler					/* 142*/
	def_irq_handler	Reserved143_IRQHandler					/* 143*/
	def_irq_handler	Reserved144_IRQHandler					/* 144*/
	def_irq_handler	Reserved145_IRQHandler					/* 145*/
	def_irq_handler	Reserved146_IRQHandler					/* 146*/
	def_irq_handler	Reserved147_IRQHandler					/* 147*/
	def_irq_handler	Reserved148_IRQHandler					/* 148*/
	def_irq_handler	Reserved149_IRQHandler					/* 149*/
	def_irq_handler	Reserved150_IRQHandler					/* 150*/
	def_irq_handler	Reserved151_IRQHandler					/* 151*/
	def_irq_handler	Reserved152_IRQHandler					/* 152*/
	def_irq_handler	Reserved153_IRQHandler					/* 153*/
	def_irq_handler	Reserved154_IRQHandler					/* 154*/
	def_irq_handler	Reserved155_IRQHandler					/* 155*/
	def_irq_handler	Reserved156_IRQHandler					/* 156*/
	def_irq_handler	Reserved157_IRQHandler					/* 157*/
	def_irq_handler	Reserved158_IRQHandler					/* 158*/
	def_irq_handler	Reserved159_IRQHandler					/* 159*/
	def_irq_handler	Reserved160_IRQHandler					/* 160*/
	def_irq_handler	Reserved161_IRQHandler					/* 161*/
	def_irq_handler	Reserved162_IRQHandler					/* 162*/
	def_irq_handler	Reserved163_IRQHandler					/* 163*/
	def_irq_handler	Reserved164_IRQHandler					/* 164*/
	def_irq_handler	Reserved165_IRQHandler					/* 165*/
	def_irq_handler	Reserved166_IRQHandler					/* 166*/
	def_irq_handler	Reserved167_IRQHandler					/* 167*/
	def_irq_handler	Reserved168_IRQHandler					/* 168*/
	def_irq_handler	Reserved169_IRQHandler					/* 169*/
	def_irq_handler	Reserved170_IRQHandler					/* 170*/
	def_irq_handler	Reserved171_IRQHandler					/* 171*/
	def_irq_handler	Reserved172_IRQHandler					/* 172*/
	def_irq_handler	Reserved173_IRQHandler					/* 173*/
	def_irq_handler	Reserved174_IRQHandler					/* 174*/
	def_irq_handler	Reserved175_IRQHandler					/* 175*/
	def_irq_handler	Reserved176_IRQHandler					/* 176*/
	def_irq_handler	Reserved177_IRQHandler					/* 177*/
	def_irq_handler	Reserved178_IRQHandler					/* 178*/
	def_irq_handler	Reserved179_IRQHandler					/* 179*/
	def_irq_handler	Reserved180_IRQHandler					/* 180*/
	def_irq_handler	Reserved181_IRQHandler					/* 181*/
	def_irq_handler	Reserved182_IRQHandler					/* 182*/
	def_irq_handler	Reserved183_IRQHandler					/* 183*/
	def_irq_handler	Reserved184_IRQHandler					/* 184*/
	def_irq_handler	Reserved185_IRQHandler					/* 185*/
	def_irq_handler	Reserved186_IRQHandler					/* 186*/
	def_irq_handler	Reserved187_IRQHandler					/* 187*/
	def_irq_handler	Reserved188_IRQHandler					/* 188*/
	def_irq_handler	Reserved189_IRQHandler					/* 189*/
	def_irq_handler	Reserved190_IRQHandler					/* 190*/
	def_irq_handler	Reserved191_IRQHandler					/* 191*/
	def_irq_handler	Reserved192_IRQHandler					/* 192*/
	def_irq_handler	Reserved193_IRQHandler					/* 193*/
	def_irq_handler	Reserved194_IRQHandler					/* 194*/
	def_irq_handler	Reserved195_IRQHandler					/* 195*/
	def_irq_handler	Reserved196_IRQHandler					/* 196*/
	def_irq_handler	Reserved197_IRQHandler					/* 197*/
	def_irq_handler	Reserved198_IRQHandler					/* 198*/
	def_irq_handler	Reserved199_IRQHandler					/* 199*/
	def_irq_handler	Reserved200_IRQHandler					/* 200*/
	def_irq_handler	Reserved201_IRQHandler					/* 201*/
	def_irq_handler	Reserved202_IRQHandler					/* 202*/
	def_irq_handler	Reserved203_IRQHandler					/* 203*/
	def_irq_handler	Reserved204_IRQHandler					/* 204*/
	def_irq_handler	Reserved205_IRQHandler					/* 205*/
	def_irq_handler	Reserved206_IRQHandler					/* 206*/
	def_irq_handler	Reserved207_IRQHandler					/* 207*/
	def_irq_handler	Reserved208_IRQHandler					/* 208*/
	def_irq_handler	Reserved209_IRQHandler					/* 209*/
	def_irq_handler	Reserved210_IRQHandler					/* 210*/
	def_irq_handler	Reserved211_IRQHandler					/* 211*/
	def_irq_handler	Reserved212_IRQHandler					/* 212*/
	def_irq_handler	Reserved213_IRQHandler					/* 213*/
	def_irq_handler	Reserved214_IRQHandler					/* 214*/
	def_irq_handler	Reserved215_IRQHandler					/* 215*/
	def_irq_handler	Reserved216_IRQHandler					/* 216*/
	def_irq_handler	Reserved217_IRQHandler					/* 217*/
	def_irq_handler	Reserved218_IRQHandler					/* 218*/
	def_irq_handler	Reserved219_IRQHandler					/* 219*/
	def_irq_handler	Reserved220_IRQHandler					/* 220*/
	def_irq_handler	Reserved221_IRQHandler					/* 221*/
	def_irq_handler	Reserved222_IRQHandler					/* 222*/
	def_irq_handler	Reserved223_IRQHandler					/* 223*/
	def_irq_handler	Reserved224_IRQHandler					/* 224*/
	def_irq_handler	Reserved225_IRQHandler					/* 225*/
	def_irq_handler	Reserved226_IRQHandler					/* 226*/
	def_irq_handler	Reserved227_IRQHandler					/* 227*/
	def_irq_handler	Reserved228_IRQHandler					/* 228*/
	def_irq_handler	Reserved229_IRQHandler					/* 229*/
	def_irq_handler	Reserved230_IRQHandler					/* 230*/
	def_irq_handler	Reserved231_IRQHandler					/* 231*/
	def_irq_handler	Reserved232_IRQHandler					/* 232*/
	def_irq_handler	Reserved233_IRQHandler					/* 233*/
	def_irq_handler	Reserved234_IRQHandler					/* 234*/
	def_irq_handler	Reserved235_IRQHandler					/* 235*/
	def_irq_handler	Reserved236_IRQHandler					/* 236*/
	def_irq_handler	Reserved237_IRQHandler					/* 237*/
	def_irq_handler	Reserved238_IRQHandler					/* 238*/
	def_irq_handler	Reserved239_IRQHandler					/* 239*/
	def_irq_handler	Reserved240_IRQHandler					/* 240*/
	def_irq_handler	Reserved241_IRQHandler					/* 241*/
	def_irq_handler	Reserved242_IRQHandler					/* 242*/
	def_irq_handler	Reserved243_IRQHandler					/* 243*/
	def_irq_handler	Reserved244_IRQHandler					/* 244*/
	def_irq_handler	Reserved245_IRQHandler					/* 245*/
	def_irq_handler	Reserved246_IRQHandler					/* 246*/
	def_irq_handler	Reserved247_IRQHandler					/* 247*/
	def_irq_handler	Reserved248_IRQHandler					/* 248*/
	def_irq_handler	Reserved249_IRQHandler					/* 249*/
	def_irq_handler	Reserved250_IRQHandler					/* 250*/
	def_irq_handler	Reserved251_IRQHandler					/* 251*/
	def_irq_handler	Reserved252_IRQHandler					/* 252*/
	def_irq_handler	Reserved253_IRQHandler					/* 253*/
	def_irq_handler	Reserved254_IRQHandler					/* 254*/

    .end
