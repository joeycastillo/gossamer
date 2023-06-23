/*
 * Copyright (c) 2015, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//-----------------------------------------------------------------------------
#include "samd51.h"

//-----------------------------------------------------------------------------
#define DUMMY __attribute__ ((weak, alias ("irq_handler_dummy")))

//-----------------------------------------------------------------------------
void irq_handler_reset(void);
void irq_handler_dummy(void);
void _exit(int status);

DUMMY void irq_handler_nmi(void);
DUMMY void irq_handler_hard_fault(void);
DUMMY void irq_handler_memory_management(void);
DUMMY void irq_handler_bus_fault(void);
DUMMY void irq_handler_usage_fault(void);
DUMMY void irq_handler_sv_call(void);
DUMMY void irq_handler_debug_monitor(void);
DUMMY void irq_handler_pend_sv(void);
DUMMY void irq_handler_sys_tick(void);

DUMMY void irq_handler_pm(void);
DUMMY void irq_handler_mclk(void);
DUMMY void irq_handler_oscctrl_0(void);
DUMMY void irq_handler_oscctrl_1(void);
DUMMY void irq_handler_oscctrl_2(void);
DUMMY void irq_handler_oscctrl_3(void);
DUMMY void irq_handler_oscctrl_4(void);
DUMMY void irq_handler_osc32kctrl(void);
DUMMY void irq_handler_supc_0(void);
DUMMY void irq_handler_supc_1(void);
DUMMY void irq_handler_wdt(void);
DUMMY void irq_handler_rtc(void);
DUMMY void irq_handler_eic_0(void);
DUMMY void irq_handler_eic_1(void);
DUMMY void irq_handler_eic_2(void);
DUMMY void irq_handler_eic_3(void);
DUMMY void irq_handler_eic_4(void);
DUMMY void irq_handler_eic_5(void);
DUMMY void irq_handler_eic_6(void);
DUMMY void irq_handler_eic_7(void);
DUMMY void irq_handler_eic_8(void);
DUMMY void irq_handler_eic_9(void);
DUMMY void irq_handler_eic_10(void);
DUMMY void irq_handler_eic_11(void);
DUMMY void irq_handler_eic_12(void);
DUMMY void irq_handler_eic_13(void);
DUMMY void irq_handler_eic_14(void);
DUMMY void irq_handler_eic_15(void);
DUMMY void irq_handler_freqm(void);
DUMMY void irq_handler_nvmctrl_0(void);
DUMMY void irq_handler_nvmctrl_1(void);
DUMMY void irq_handler_dmac_0(void);
DUMMY void irq_handler_dmac_1(void);
DUMMY void irq_handler_dmac_2(void);
DUMMY void irq_handler_dmac_3(void);
DUMMY void irq_handler_dmac_4(void);
DUMMY void irq_handler_evsys_0(void);
DUMMY void irq_handler_evsys_1(void);
DUMMY void irq_handler_evsys_2(void);
DUMMY void irq_handler_evsys_3(void);
DUMMY void irq_handler_evsys_4(void);
DUMMY void irq_handler_pac(void);
DUMMY void irq_handler_ramecc(void);
DUMMY void irq_handler_sercom0_0(void);
DUMMY void irq_handler_sercom0_1(void);
DUMMY void irq_handler_sercom0_2(void);
DUMMY void irq_handler_sercom0_3(void);
DUMMY void irq_handler_sercom1_0(void);
DUMMY void irq_handler_sercom1_1(void);
DUMMY void irq_handler_sercom1_2(void);
DUMMY void irq_handler_sercom1_3(void);
DUMMY void irq_handler_sercom2_0(void);
DUMMY void irq_handler_sercom2_1(void);
DUMMY void irq_handler_sercom2_2(void);
DUMMY void irq_handler_sercom2_3(void);
DUMMY void irq_handler_sercom3_0(void);
DUMMY void irq_handler_sercom3_1(void);
DUMMY void irq_handler_sercom3_2(void);
DUMMY void irq_handler_sercom3_3(void);
DUMMY void irq_handler_sercom4_0(void);
DUMMY void irq_handler_sercom4_1(void);
DUMMY void irq_handler_sercom4_2(void);
DUMMY void irq_handler_sercom4_3(void);
DUMMY void irq_handler_sercom5_0(void);
DUMMY void irq_handler_sercom5_1(void);
DUMMY void irq_handler_sercom5_2(void);
DUMMY void irq_handler_sercom5_3(void);
DUMMY void irq_handler_sercom6_0(void);
DUMMY void irq_handler_sercom6_1(void);
DUMMY void irq_handler_sercom6_2(void);
DUMMY void irq_handler_sercom6_3(void);
DUMMY void irq_handler_sercom7_0(void);
DUMMY void irq_handler_sercom7_1(void);
DUMMY void irq_handler_sercom7_2(void);
DUMMY void irq_handler_sercom7_3(void);
DUMMY void irq_handler_can0(void);
DUMMY void irq_handler_can1(void);
DUMMY void irq_handler_usb_0(void);
DUMMY void irq_handler_usb_1(void);
DUMMY void irq_handler_usb_2(void);
DUMMY void irq_handler_usb_3(void);
DUMMY void irq_handler_gmac(void);
DUMMY void irq_handler_tcc0_0(void);
DUMMY void irq_handler_tcc0_1(void);
DUMMY void irq_handler_tcc0_2(void);
DUMMY void irq_handler_tcc0_3(void);
DUMMY void irq_handler_tcc0_4(void);
DUMMY void irq_handler_tcc0_5(void);
DUMMY void irq_handler_tcc0_6(void);
DUMMY void irq_handler_tcc1_0(void);
DUMMY void irq_handler_tcc1_1(void);
DUMMY void irq_handler_tcc1_2(void);
DUMMY void irq_handler_tcc1_3(void);
DUMMY void irq_handler_tcc1_4(void);
DUMMY void irq_handler_tcc2_0(void);
DUMMY void irq_handler_tcc2_1(void);
DUMMY void irq_handler_tcc2_2(void);
DUMMY void irq_handler_tcc2_3(void);
DUMMY void irq_handler_tcc3_0(void);
DUMMY void irq_handler_tcc3_1(void);
DUMMY void irq_handler_tcc3_2(void);
DUMMY void irq_handler_tcc4_0(void);
DUMMY void irq_handler_tcc4_1(void);
DUMMY void irq_handler_tcc4_2(void);
DUMMY void irq_handler_tc0(void);
DUMMY void irq_handler_tc1(void);
DUMMY void irq_handler_tc2(void);
DUMMY void irq_handler_tc3(void);
DUMMY void irq_handler_tc4(void);
DUMMY void irq_handler_tc5(void);
DUMMY void irq_handler_tc6(void);
DUMMY void irq_handler_tc7(void);
DUMMY void irq_handler_pdec_0(void);
DUMMY void irq_handler_pdec_1(void);
DUMMY void irq_handler_pdec_2(void);
DUMMY void irq_handler_adc0_0(void);
DUMMY void irq_handler_adc0_1(void);
DUMMY void irq_handler_adc1_0(void);
DUMMY void irq_handler_adc1_1(void);
DUMMY void irq_handler_ac(void);
DUMMY void irq_handler_dac_0(void);
DUMMY void irq_handler_dac_1(void);
DUMMY void irq_handler_dac_2(void);
DUMMY void irq_handler_dac_3(void);
DUMMY void irq_handler_dac_4(void);
DUMMY void irq_handler_i2s(void);
DUMMY void irq_handler_pcc(void);
DUMMY void irq_handler_aes(void);
DUMMY void irq_handler_trng(void);
DUMMY void irq_handler_icm(void);
DUMMY void irq_handler_pukcc(void);
DUMMY void irq_handler_qspi(void);
DUMMY void irq_handler_sdhc0(void);
DUMMY void irq_handler_sdhc1(void);

extern int main(void);

extern void _stack_top(void);
extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

//-----------------------------------------------------------------------------
__attribute__ ((used, section(".vectors")))
void (* const vectors[])(void) =
{
    &_stack_top,                   // 0 - Initial Stack Pointer Value

    // Cortex-M4 handlers
    irq_handler_reset,              //  1 - Reset
    irq_handler_nmi,                //  2 - NMI
    irq_handler_hard_fault,         //  3 - Hard Fault
    irq_handler_memory_management,  //  4 - Memory Management
    irq_handler_bus_fault,          //  5 - Bus Fault
    irq_handler_usage_fault,        //  6 - Usage Fault
    0,                              //  7 - Reserved
    0,                              //  8 - Reserved
    0,                              //  9 - Reserved
    0,                              // 10 - Reserved
    irq_handler_sv_call,            // 11 - SVCall
    irq_handler_debug_monitor,      // 12 - Debug monitor
    0,                              // 13 - Reserved
    irq_handler_pend_sv,            // 14 - PendSV
    irq_handler_sys_tick,           // 15 - SysTick

    // Peripheral handlers
    irq_handler_pm,                 //   0 - Power Manager
    irq_handler_mclk,               //   1 - Main Clock
    irq_handler_oscctrl_0,          //   2 - OSCCTRL_XOSCFAIL_0, OSCCTRL_XOSCRDY_0
    irq_handler_oscctrl_1,          //   3 - OSCCTRL_XOSCFAIL_1, OSCCTRL_XOSCRDY_1
    irq_handler_oscctrl_2,          //   4 - OSCCTRL_DFLLLOCKC, OSCCTRL_DFLLLOCKF, OSCCTRL_DFLLOOB, OSCCTRL_DFLLRCS, OSCCTRL_DFLLRDY
    irq_handler_oscctrl_3,          //   5 - OSCCTRL_DPLLLCKF_0, OSCCTRL_DPLLLCKR_0, OSCCTRL_DPLLLDRTO_0, OSCCTRL_DPLLLTO_0
    irq_handler_oscctrl_4,          //   6 - OSCCTRL_DPLLLCKF_1, OSCCTRL_DPLLLCKR_1, OSCCTRL_DPLLLDRTO_1, OSCCTRL_DPLLLTO_1
    irq_handler_osc32kctrl,         //   7 - 32kHz Oscillators Control
    irq_handler_supc_0,             //   8 - SUPC_B12SRDY, SUPC_B33SRDY, SUPC_BOD12RDY, SUPC_BOD33RDY, SUPC_VCORERDY, SUPC_VREGRDY
    irq_handler_supc_1,             //   9 - SUPC_BOD12DET, SUPC_BOD33DET
    irq_handler_wdt,                //  10 - Watchdog Timer
    irq_handler_rtc,                //  11 - Real-Time Counter
    irq_handler_eic_0,              //  12 - External Interrupt Controller
    irq_handler_eic_1,              //  13 - External Interrupt Controller
    irq_handler_eic_2,              //  14 - External Interrupt Controller
    irq_handler_eic_3,              //  15 - External Interrupt Controller
    irq_handler_eic_4,              //  16 - External Interrupt Controller
    irq_handler_eic_5,              //  17 - External Interrupt Controller
    irq_handler_eic_6,              //  18 - External Interrupt Controller
    irq_handler_eic_7,              //  19 - External Interrupt Controller
    irq_handler_eic_8,              //  20 - External Interrupt Controller
    irq_handler_eic_9,              //  21 - External Interrupt Controller
    irq_handler_eic_10,             //  22 - External Interrupt Controller
    irq_handler_eic_11,             //  23 - External Interrupt Controller
    irq_handler_eic_12,             //  24 - External Interrupt Controller
    irq_handler_eic_13,             //  25 - External Interrupt Controller
    irq_handler_eic_14,             //  26 - External Interrupt Controller
    irq_handler_eic_15,             //  27 - External Interrupt Controller
    irq_handler_freqm,              //  28 - Frequency Measurement
    irq_handler_nvmctrl_0,          //  29 - Non-Volatile Memory Controller 0-7
    irq_handler_nvmctrl_1,          //  30 - Non-Volatile Memory Controller 8-10
    irq_handler_dmac_0,             //  31 - Direct Memory Access Controller
    irq_handler_dmac_1,             //  32 - Direct Memory Access Controller
    irq_handler_dmac_2,             //  33 - Direct Memory Access Controller
    irq_handler_dmac_3,             //  34 - Direct Memory Access Controller
    irq_handler_dmac_4,             //  35 - Direct Memory Access Controller
    irq_handler_evsys_0,            //  36 - Event System
    irq_handler_evsys_1,            //  37 - Event System
    irq_handler_evsys_2,            //  38 - Event System
    irq_handler_evsys_3,            //  39 - Event System
    irq_handler_evsys_4,            //  40 - Event System
    irq_handler_pac,                //  41 - Peripheral Access Controller
    0,                              //  42 - Reserved
    0,                              //  43 - Reserved
    0,                              //  44 - Reserved
    irq_handler_ramecc,             //  45 - RAM ECC
    irq_handler_sercom0_0,          //  46 - Serial Communication Interface 0
    irq_handler_sercom0_1,          //  47 - Serial Communication Interface 0
    irq_handler_sercom0_2,          //  48 - Serial Communication Interface 0
    irq_handler_sercom0_3,          //  49 - Serial Communication Interface 0
    irq_handler_sercom1_0,          //  50 - Serial Communication Interface 1
    irq_handler_sercom1_1,          //  51 - Serial Communication Interface 1
    irq_handler_sercom1_2,          //  52 - Serial Communication Interface 1
    irq_handler_sercom1_3,          //  53 - Serial Communication Interface 1
    irq_handler_sercom2_0,          //  54 - Serial Communication Interface 2
    irq_handler_sercom2_1,          //  55 - Serial Communication Interface 2
    irq_handler_sercom2_2,          //  56 - Serial Communication Interface 2
    irq_handler_sercom2_3,          //  57 - Serial Communication Interface 2
    irq_handler_sercom3_0,          //  58 - Serial Communication Interface 3
    irq_handler_sercom3_1,          //  59 - Serial Communication Interface 3
    irq_handler_sercom3_2,          //  60 - Serial Communication Interface 3
    irq_handler_sercom3_3,          //  61 - Serial Communication Interface 3
#ifdef ID_SERCOM4
    irq_handler_sercom4_0,          //  62 - Serial Communication Interface 4
    irq_handler_sercom4_1,          //  63 - Serial Communication Interface 4
    irq_handler_sercom4_2,          //  64 - Serial Communication Interface 4
    irq_handler_sercom4_3,          //  65 - Serial Communication Interface 4
#else
    0,                              //  62 - Reserved
    0,                              //  63 - Reserved
    0,                              //  64 - Reserved
    0,                              //  65 - Reserved
#endif
#ifdef ID_SERCOM5
    irq_handler_sercom5_0,          //  66 - Serial Communication Interface 5
    irq_handler_sercom5_1,          //  67 - Serial Communication Interface 5
    irq_handler_sercom5_2,          //  68 - Serial Communication Interface 5
    irq_handler_sercom5_3,          //  69 - Serial Communication Interface 5
#else
    0,                              //  66 - Reserved
    0,                              //  67 - Reserved
    0,                              //  68 - Reserved
    0,                              //  69 - Reserved
#endif
#ifdef ID_SERCOM6
    irq_handler_sercom6_0,          //  70 - Serial Communication Interface 6
    irq_handler_sercom6_1,          //  71 - Serial Communication Interface 6
    irq_handler_sercom6_2,          //  72 - Serial Communication Interface 6
    irq_handler_sercom6_3,          //  73 - Serial Communication Interface 6
#else
    0,                              //  70 - Reserved
    0,                              //  71 - Reserved
    0,                              //  72 - Reserved
    0,                              //  73 - Reserved
#endif
#ifdef ID_SERCOM7
    irq_handler_sercom7_0,          //  74 - Serial Communication Interface 7
    irq_handler_sercom7_1,          //  75 - Serial Communication Interface 7
    irq_handler_sercom7_2,          //  76 - Serial Communication Interface 7
    irq_handler_sercom7_3,          //  77 - Serial Communication Interface 7
#else
    0,                              //  74 - Reserved
    0,                              //  75 - Reserved
    0,                              //  76 - Reserved
    0,                              //  77 - Reserved
#endif
#ifdef ID_CAN0
    irq_handler_can0,               //  78 - Control Area Network 0
#else
    0,                              //  78 - Reserved
#endif
#ifdef ID_CAN1
    irq_handler_can1,               //  79 - Control Area Network 1
#else
    0,                              //  79 - Reserved
#endif
    irq_handler_usb_0,              //  80 - USB_EORSM_DNRSM, USB_EORST_RST, USB_LPMSUSP_DDISC, USB_LPM_DCONN, USB_MSOF, USB_RAMACER, USB_RXSTP_TXSTP_0, USB_RXSTP_TXSTP_1, USB_RXSTP_TXSTP_2, USB_RXSTP_TXSTP_3, USB_RXSTP_TXSTP_4, USB_RXSTP_TXSTP_5, USB_RXSTP_TXSTP_6, USB_RXSTP_TXSTP_7, USB_STALL0_STALL_0, USB_STALL0_STALL_1, USB_STALL0_STALL_2, USB_STALL0_STALL_3, USB_STALL0_STALL_4, USB_STALL0_STALL_5, USB_STALL0_STALL_6, USB_STALL0_STALL_7, USB_STALL1_0, USB_STALL1_1, USB_STALL1_2, USB_STALL1_3, USB_STALL1_4, USB_STALL1_5, USB_STALL1_6, USB_STALL1_7, USB_SUSPEND, USB_TRFAIL0_TRFAIL_0, USB_TRFAIL0_TRFAIL_1, USB_TRFAIL0_TRFAIL_2, USB_TRFAIL0_TRFAIL_3, USB_TRFAIL0_TRFAIL_4, USB_TRFAIL0_TRFAIL_5, USB_TRFAIL0_TRFAIL_6, USB_TRFAIL0_TRFAIL_7, USB_TRFAIL1_PERR_0, USB_TRFAIL1_PERR_1, USB_TRFAIL1_PERR_2, USB_TRFAIL1_PERR_3, USB_TRFAIL1_PERR_4, USB_TRFAIL1_PERR_5, USB_TRFAIL1_PERR_6, USB_TRFAIL1_PERR_7, USB_UPRSM, USB_WAKEUP
    irq_handler_usb_1,              //  81 - USB_SOF_HSOF
    irq_handler_usb_2,              //  82 - USB_TRCPT0_0, USB_TRCPT0_1, USB_TRCPT0_2, USB_TRCPT0_3, USB_TRCPT0_4, USB_TRCPT0_5, USB_TRCPT0_6, USB_TRCPT0_7
    irq_handler_usb_3,              //  83 - 83 USB_TRCPT1_0, USB_TRCPT1_1, USB_TRCPT1_2, USB_TRCPT1_3, USB_TRCPT1_4, USB_TRCPT1_5, USB_TRCPT1_6, USB_TRCPT1_7
#ifdef ID_GMAC
    irq_handler_gmac,               //  84 - Ethernet MAC
#else
    0,                              //  84 - Reserved
#endif
    irq_handler_tcc0_0,             //  85 - TCC0_CNT_A, TCC0_DFS_A, TCC0_ERR_A, TCC0_FAULT0_A, TCC0_FAULT1_A, TCC0_FAULTA_A, TCC0_FAULTB_A, TCC0_OVF, TCC0_TRG, TCC0_UFS_A
    irq_handler_tcc0_1,             //  86 - TCC0_MC_0
    irq_handler_tcc0_2,             //  87 - TCC0_MC_1
    irq_handler_tcc0_3,             //  88 - TCC0_MC_2
    irq_handler_tcc0_4,             //  89 - TCC0_MC_3
    irq_handler_tcc0_5,             //  90 - TCC0_MC_4
    irq_handler_tcc0_1,             //  91 - TCC0_MC_5
    irq_handler_tcc1_0,             //  92 - TCC1_CNT_A, TCC1_DFS_A, TCC1_ERR_A, TCC1_FAULT0_A, TCC1_FAULT1_A, TCC1_FAULTA_A, TCC1_FAULTB_A, TCC1_OVF, TCC1_TRG, TCC1_UFS_A
    irq_handler_tcc1_1,             //  93 - TCC1_MC_0
    irq_handler_tcc1_2,             //  94 - TCC1_MC_1
    irq_handler_tcc1_3,             //  95 - TCC1_MC_2
    irq_handler_tcc1_4,             //  96 - TCC1_MC_3
    irq_handler_tcc2_0,             //  97 - TCC2_CNT_A, TCC2_DFS_A, TCC2_ERR_A, TCC2_FAULT0_A, TCC2_FAULT1_A, TCC2_FAULTA_A, TCC2_FAULTB_A, TCC2_OVF, TCC2_TRG, TCC2_UFS_A
    irq_handler_tcc2_1,             //  98 - TCC2_MC_0
    irq_handler_tcc2_2,             //  99 - TCC2_MC_1
    irq_handler_tcc2_3,             // 100 - TCC2_MC_2
#ifdef ID_TCC3
    irq_handler_tcc3_0,             // 101 - TCC3_CNT_A, TCC3_DFS_A, TCC3_ERR_A, TCC3_FAULT0_A, TCC3_FAULT1_A, TCC3_FAULTA_A, TCC3_FAULTB_A, TCC3_OVF, TCC3_TRG, TCC3_UFS_A
    irq_handler_tcc3_1,             // 102 - TCC3_MC_0
    irq_handler_tcc3_2,             // 103 - TCC3_MC_1
#else
    0,                              // 101 - Reserved
    0,                              // 102 - Reserved
    0,                              // 103 - Reserved
#endif
#ifdef ID_TCC4
    irq_handler_tcc4_0,             // 104 - TCC4_CNT_A, TCC4_DFS_A, TCC4_ERR_A, TCC4_FAULT0_A, TCC4_FAULT1_A, TCC4_FAULTA_A, TCC4_FAULTB_A, TCC4_OVF, TCC4_TRG, TCC4_UFS_A
    irq_handler_tcc4_1,             // 105 - TCC4_MC_0
    irq_handler_tcc4_2,             // 106 - TCC4_MC_1
#else
    0,                              // 104 - Reserved
    0,                              // 105 - Reserved
    0,                              // 106 - Reserved
#endif
    irq_handler_tc0,                // 107 - Timer/Counter 0
    irq_handler_tc1,                // 108 - Timer/Counter 1
    irq_handler_tc2,                // 109 - Timer/Counter 2
    irq_handler_tc3,                // 100 - Timer/Counter 3
#ifdef ID_TC4
    irq_handler_tc4,                // 111 - Timer/Counter 4
#else
    0,                              // 111 - Reserved
#endif
#ifdef ID_TC5
    irq_handler_tc5,                // 112 - Timer/Counter 5
#else
    0,                              // 112 - Reserved
#endif
#ifdef ID_TC6
    irq_handler_tc6,                // 113 - Timer/Counter 6
#else
    0,                              // 113 - Reserved
#endif
#ifdef ID_TC7
    irq_handler_tc7,                // 114 - Timer/Counter 7
#else
    0,                              // 114 - Reserved
#endif
    irq_handler_pdec_0,             // 115 - PDEC_DIR_A, PDEC_ERR_A, PDEC_OVF, PDEC_VLC_A
    irq_handler_pdec_1,             // 116 - PDEC_MC_0
    irq_handler_pdec_2,             // 117 - PDEC_MC_1
    irq_handler_adc0_0,             // 118 - ADC0_OVERRUN, ADC0_WINMON
    irq_handler_adc0_1,             // 119 - ADC0_RESRDY
    irq_handler_adc1_0,             // 120 - ADC1_OVERRUN, ADC1_WINMON
    irq_handler_adc1_1,             // 121 - ADC1_RESRDY
    irq_handler_ac,                 // 122 - Analog Comparator
    irq_handler_dac_0,              // 123 - DAC_OVERRUN_A_0, DAC_OVERRUN_A_1, DAC_UNDERRUN_A_0, DAC_UNDERRUN_A_1
    irq_handler_dac_1,              // 124 - DAC_EMPTY_0
    irq_handler_dac_2,              // 125 - DAC_EMPTY_1
    irq_handler_dac_3,              // 126 - DAC_RESRDY_0
    irq_handler_dac_4,              // 127 - DAC_RESRDY_1
    irq_handler_i2s,                // 128 - Inter-IC Sound Interface
    irq_handler_pcc,                // 129 - Parallel Capture Controller
    irq_handler_aes,                // 130 - Advanced Encryption Standard
    irq_handler_trng,               // 131 - True Random Number Generator
    irq_handler_icm,                // 132 - Integrity Check Monitor
    irq_handler_pukcc,              // 133 - Public-Key Cryptography Controller
    irq_handler_qspi,               // 134 - Quad SPI interface
    irq_handler_sdhc0,              // 135 - SD/MMC Host Controller 0
#ifdef ID_SDHC1
    irq_handler_sdhc1,              // 136 - SD/MMC Host Controller 1
#else
    0,                              // 136 - Reserved
#endif
};

//-----------------------------------------------------------------------------
void irq_handler_reset(void)
{
    unsigned int *src, *dst;

    src = &_etext;
    dst = &_data;
    while (dst < &_edata)
        *dst++ = *src++;

    dst = &_bss;
    while (dst < &_ebss)
        *dst++ = 0;

    SCB->VTOR = (uint32_t)vectors;

    main();

    while (1);
}

//-----------------------------------------------------------------------------
void irq_handler_dummy(void)
{
    while (1);
}

//-----------------------------------------------------------------------------
void _exit(int status)
{
    (void)status;
    while (1);
}
