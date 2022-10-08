#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma once

/** @brief Initializes a TCC instance and a waveform output channel for driving a piezo buzzer.
  *        Use this function if one leg of your piezo is tied to a pin, and the other to VCC or GND.
  * @details Note that this call does not deal with pins at all. You will need to set the pin to
  *          be an output and specify the correct pin mux for the TCC waveform output.
  * @param instance The TCC peripheral you wish to use, as numbered in the data sheet (or 0 if
  *                 the chip has only one TCC peripheral).
  * @param channel The waveform output channel (from 0 to 7) on which the buzzer is attached.
  */
void piezo_tcc_init(uint8_t instance, uint8_t channel);

/** @brief Initializes a TCC instance and a waveform output channel for driving a piezo buzzer
  *        differentially, which allows for louder operation. Use this function if each leg of
  *        your piezo buzzer is tied to a GPIO pin on the same TCC peripheral.
  * @details Note that this call does not deal with pins at all. You will need to set the pin to
  *          be an output and specify the correct pin mux for the TCC waveform output.
  * @param instance The TCC peripheral you wish to use, as numbered in the data sheet (or 0 if
  *                 the chip has only one TCC peripheral).
  * @param channel_p The waveform output channel (0 to 7) for the positive side of the buzzer.
  * @param channel_n The waveform output channel (0 to 7) for the negative side of the buzzer.
  */
void piezo_tcc_init_differential(uint8_t instance, uint8_t channel_p, uint8_t channel_n);

/** @brief Turns the piezo buzzer on at the provided frequency.
  * @param freq The frequency in hertz.
  * @note At this time, this function call assumes the CPU frequency is 8 MHz. This may be fixed
  *       at a later date.
  */
void piezo_tcc_on(uint32_t freq);

/** @brief Turns the piezo buzzer off.
  */
void piezo_tcc_off(void);
