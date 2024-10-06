/*
 * Copyright (c) 2024 Joey Castillo
 * 
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
#define DUMMY __attribute__ ((weak, alias ("irq_handler_dummy")))

//-----------------------------------------------------------------------------
void irq_handler_reset(void);
void irq_handler_dummy(void);
void _exit(int status);

DUMMY void irq_handler_nmi(void);
DUMMY void irq_handler_hard_fault(void);
DUMMY void irq_handler_sv_call(void);
DUMMY void irq_handler_pend_sv(void);
DUMMY void irq_handler_sys_tick(void);

DUMMY void irq_handler_system(void);
DUMMY void irq_handler_wdt(void);
DUMMY void irq_handler_rtc(void);
DUMMY void irq_handler_eic(void);
DUMMY void irq_handler_nvmctrl(void);
DUMMY void irq_handler_dmac(void);
DUMMY void irq_handler_usb(void);
DUMMY void irq_handler_evsys(void);
DUMMY void irq_handler_sercom0(void);
DUMMY void irq_handler_sercom1(void);
DUMMY void irq_handler_sercom2(void);
DUMMY void irq_handler_sercom3(void);
DUMMY void irq_handler_sercom4(void);
DUMMY void irq_handler_sercom5(void);
DUMMY void irq_handler_tcc0(void);
DUMMY void irq_handler_tcc1(void);
DUMMY void irq_handler_tcc2(void);
DUMMY void irq_handler_tc0(void);
DUMMY void irq_handler_tc1(void);
DUMMY void irq_handler_tc2(void);
DUMMY void irq_handler_tc3(void);
DUMMY void irq_handler_tc4(void);
DUMMY void irq_handler_adc(void);
DUMMY void irq_handler_ac(void);
DUMMY void irq_handler_dac(void);
DUMMY void irq_handler_ptc(void);
DUMMY void irq_handler_aes(void);
DUMMY void irq_handler_trng(void);

extern int main(void);

void _stack_top(void) {};
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

    // Cortex-M0+ handlers
    irq_handler_reset,             // 1 - Reset
    irq_handler_nmi,               // 2 - NMI
    irq_handler_hard_fault,        // 3 - Hard Fault
    0,                             // 4 - Reserved
    0,                             // 5 - Reserved
    0,                             // 6 - Reserved
    0,                             // 7 - Reserved
    0,                             // 8 - Reserved
    0,                             // 9 - Reserved
    0,                             // 10 - Reserved
    irq_handler_sv_call,           // 11 - SVCall
    0,                             // 12 - Reserved
    0,                             // 13 - Reserved
    irq_handler_pend_sv,           // 14 - PendSV
    irq_handler_sys_tick,          // 15 - SysTick

    // Peripheral handlers
    irq_handler_system,            // 0 - System Controller
    irq_handler_wdt,               // 1 - Watchdog Timer
    irq_handler_rtc,               // 2 - Real-Time Counter
    irq_handler_eic,               // 3 - External Interrupt Controller
    irq_handler_nvmctrl,           // 4 - Non-Volatile Memory Controller
    irq_handler_dmac,              // 5 - Direct Memory Access Controller
    irq_handler_usb,               // 6 - Universal Serial Bus Controller
    irq_handler_evsys,             // 7 - Event System
    irq_handler_sercom0,           // 8 - Serial Communication Interface 0
    irq_handler_sercom1,           // 9 - Serial Communication Interface 1
    irq_handler_sercom2,           // 10 - Serial Communication Interface 2
    irq_handler_sercom3,           // 11 - Serial Communication Interface 3
    irq_handler_sercom4,           // 12 - Serial Communication Interface 4
    irq_handler_sercom5,           // 13 - Serial Communication Interface 5
    irq_handler_tcc0,              // 14 - Timer/Counter for Control 0
    irq_handler_tcc1,              // 15 - Timer/Counter for Control 1
    irq_handler_tcc2,              // 16 - Timer/Counter for Control 2
    irq_handler_tc0,               // 17 - Timer/Counter 0
    irq_handler_tc1,               // 18 - Timer/Counter 1
    irq_handler_tc2,               // 19 - Timer/Counter 2
    irq_handler_tc3,               // 20 - Timer/Counter 3
    irq_handler_tc4,               // 21 - Timer/Counter 4
    irq_handler_adc,               // 22 - Analog-to-Digital Converter
    irq_handler_ac,                // 23 - Analog Comparator
    irq_handler_dac,               // 24 - Digital-to-Analog Converter
    irq_handler_ptc,               // 25 - Peripheral Touch Controller
    irq_handler_aes,               // 26 - Advanced Encryption Standard
    irq_handler_trng,              // 27 - True Random Number Generator
};

//-----------------------------------------------------------------------------
void irq_handler_reset(void)
{
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
