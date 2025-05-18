////< @file eic.h
/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @addtogroup eic External Interrupt Controller
 * @brief Functions for configuring and using the External Interrupt Controller.
 * @{
 */ 

///@brief An enum defining the types of interrupt trigger you wish to scan for.
typedef enum eic_interrupt_trigger_t {
    INTERRUPT_TRIGGER_NONE = 0,
    INTERRUPT_TRIGGER_RISING,
    INTERRUPT_TRIGGER_FALLING,
    INTERRUPT_TRIGGER_BOTH,
} eic_interrupt_trigger_t;

typedef void (*eic_cb_t)(uint8_t channel);

/** @brief Initializes the external interrupt controller, but does not enable it.
  */
void eic_init(void);

/** @brief Enables the external interrupt controller.
  */
void eic_enable(void);

/**
 * @brief Checks if the external interrupt controller is currently enabled.
 * @returns true if the external interrupt controller is enabled, false if it is not.
 */
bool eic_is_enabled(void);

/** @brief Configures an external interrupt on one of the external interrupt pins.
  * @details This function configures the interrupt channel with the specified trigger, but it does not
  *          set pin direction, mux or pull; you must set up the pin yourself. This function also does
  *          not enable an interrupt or event generation. You must call eic_enable_interrupt() or
  *          eic_enable_event() to either wake the processor or generate an event for a peripheral.
  * @param pin The external interrupt pin you wish to configure — use HAL_GPIO_xxx_PIN() macro.
  * @param trigger The condition on which you wish to trigger: rising, falling or both.
  * @param filten A boolean indicating whether to enable filtering. This causes the EIC to sample the
  *               pin three times (clocked by the EIC clock) and apply a majority vote filter.
  * @return the EIC channel number associated with the pin, or -1 if the pin is not an interrupt pin.
  * @note Be sure to check your pin multiplexing table to ensure that you do not have multiple pins
  *       assigned to the same interrupt channel. Also note that the NMI pin is not currently supported.
  */
int8_t eic_configure_pin(const uint8_t pin, eic_interrupt_trigger_t trigger, bool filten);

/** @brief Enables an interrupt on the given interrupt channel.
  * @param pin The external interrupt pin.
  */
bool eic_enable_interrupt(const uint8_t pin);

/** @brief Disables the interrupt on the given interrupt channel.
  * @param pin The external interrupt pin.
  */
bool eic_disable_interrupt(const uint8_t pin);

/** @brief Enables an interrupt on the given interrupt channel.
  * @param pin The external interrupt pin.
  */
bool eic_enable_event(const uint8_t pin);

/** @brief Disables the interrupt on the given interrupt channel.
  * @param pin The external interrupt pin.
  */
bool eic_disable_event(const uint8_t pin);

/** @brief Configures an external interrupt callback.
  * @details You only get the one callback, so if you need to handle multiple interrupts, you'll need
  *          to check the EIC->INTFLAG register in your callback to determine which interrupt fired.
  * @param callback The function that will be called when the interrupt fires.
  * @note This function is called from the interrupt handler, so it should be as short as possible.
  */
void eic_configure_callback(eic_cb_t callback);

/** @brief Disables the external interrupt controller.
  */
void eic_disable(void);

/** @} */
