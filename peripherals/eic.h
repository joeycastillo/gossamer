/** 
 * @file eic.h
 * @brief External Interrupt Controller
 */
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

///@brief An enum defining the types of interrupt trigger you wish to scan for.
typedef enum eic_interrupt_trigger {
    INTERRUPT_TRIGGER_NONE = 0,
    INTERRUPT_TRIGGER_RISING,
    INTERRUPT_TRIGGER_FALLING,
    INTERRUPT_TRIGGER_BOTH,
} eic_interrupt_trigger;

typedef void (*eic_cb_t)(void);

/** @brief Enables the external interrupt controller.
  */
void eic_init(void);

/** @brief Configures an external interrupt on one of the external interrupt channels
  * @details Note that this call does not deal with pins at all. Your external interrupt pin needs
  *          to already be configured as an input, and assigned to the EIC peripheral function.
  *          With that done, consult the pin mux table in the data sheet to see which EIC channel
  *          is associated with that pin, and pass that channel number into this function.
  * @param channel The external interrupt channel associated with the pin (consult the data sheet).
  * @param trigger The condition on which you wish to trigger: rising, falling or both.
  */
void eic_configure_channel(const uint8_t channel, eic_interrupt_trigger trigger);

/** @brief Configures an external interrupt callback.
  * @param callback The function that will be called when the interrupt fires.
  */
void eic_configure_callback(eic_cb_t callback);
