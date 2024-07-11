/// @mainpage

gossamer: a very lightweight firmware framework for SAMD and SAML chips
=======================================================================

## WARNING: I am dramatically refactoring right now (Fall 2023) as I make use of this framework in a couple of real world projects. The API is subject to dramatic and breaking changes for the next few months as I work through what I want this to be. — Joey

# Original README

> gos·sa·mer noun  
> \ ˈgä-sə-mər \  
>  
> 1. a film of cobwebs floating in air in calm clear weather  
> 2. something light, delicate, or insubstantial

gossamer is an extremely lightweight framework for developing bare-metal firmware applications for the Microchip (neé Atmel) SAM D and SAM L series chips. it aims for consistency, simplicity and low power consumption.

**STILL UNDER ACTIVE DEVELOPMENT!** The API remains likely to change for the foreseeable future.

Gossamer targets four chips at this time: the SAM D11, D21, L21 and L22. These chips are all strikingly similar: they all have Cortex M0+ cores as well as native USB and as familiar peripherals like ADC's, TCC's and SERCOMs. Alas, a lot of the interfaces to these peripherals have slight differences, and at startup, these chips have subtly different clock setups.

The primary goal of the project is to provide a simple environment for making device firmware for gadgets with these chips. The secondary goal is to ensure that the code you write can be compatible across chips in the family. By papering over subtle differences in syntax and setup, you should be able to write code for one chip in the family (like the SAM D21), and then seamlessly compile it for another, like the SAM D11 if you realize you need to cut BOM costs, or the L21 if you decide that low power is your priority.

Not all devices have all the same peripherals. Some features, like the segment LCD, are specific to one chip like the SAM L22. Even some devices that do have the same peripherals have different configuration options; for example, the DAC peripheral has two 12-bit channels on the SAM L21, but only one 10-bit channel on the SAM D21 and D11. Still, many of the peripherals are very similar on the different platforms, and gossamer provides a thin layer of translation on top that should make it easy to move up and down the product line.

A consistent setup at boot
--------------------------

If you look at the data sheet, you'll see that some devices boot with a 1 MHz clock, and others with a 4 MHz clock. No generic clocks are defined, and if you're coming from the Atmel Start world, you'll recall that it was on you to invent a clock tree from sceatch. 

Gossamer simplifies this by giving you a consistent environment on all four platforms:

* GCLK0, the main system clock, is set to 8 MHz at boot. You can change this in `app_init` using the `set_cpu_frequency` function.
* GCLK1 is reserved for the 48 MHz DFLL clock, which is only configured if the USB peripheral is enabled.
* GCLK2 is a low-power, low-accuracy 32 kHz clock sourced from OSCULP32K.
* GCLK3 is a 1024 Hz clock, derived from the most accurate source available.

Some peripherals are automatically clocked from the source that makes the most sense (i.e. RTC from GCLK3, EIC from GCLK2). SERCOM peripherals are clocked from the main clock, but provide a configurable baud rate in theor init functions. More versatile peripherals like the TC and TCC take a generic clock and prescaler factor as parameters in their init functions.

Consistent interface to peripherals
-----------------------------------

Peripherals are generally thin wrappers around the actual hardware, and provide a consistent way to enable, disable and operate the peripherals. This allows us to paper over subtle implementation details and hand-wave away register layout differences, like the difference between waiting on `STATUS.bit.SYNCBUSY` vs `SYNCBUSY.reg`.

All peripherals (WIP) have a `peripheral_init` and a `peripheral_enable` function, as well as a `peripheral_disable` function.

* `peripheral_init` enables the peripheral clocks and configures the peripheral, but does not enable the peripheral itself. Generally returns void, but for peripherals whose initialization can fail (i.e. due to a bad parameter), some init functions will return a boolean value: `true` if successful, `false` otherwise.
* `peripheral_enable` enables the peripheral itself. Some peripherals have multiple channels; for example, the SAM L21 has two DAC channels. In these cases, the enable function takes as a parameter which instance to enable.
* `peripheral_disable` disables the peripheral. For peripherals with multiple channels, disables the channel, unless all channels are disabled in which case it disables the whole peripheral.

For example, this is how you might set up the RTC peripheral, which has predefined clock defaults:

```c
rtc_init();
rtc_enable();
rtc_set_date_time(date_time);
```

This is how you might output an analog value on DAC channel 0 (the first DAC on the SAM L21, or the only DAC on D21):

```c
HAL_GPIO_A0_pmuxen(HAL_GPIO_PMUX_B);
dac_init();
dac_enable(0);
dac_set_analog_value(0, 512);
```

This is how you would set up Timer/Counter TC1 to overflow at 500 Hz (i.e. for driving a DMA transaction):

```c
tc_init(1, GENERIC_CLOCK_0, TC_PRESCALER_DIV256); // 16 MHz / 256 = 62,500
tc_set_wavegen(1, TC_WAVE_WAVEGEN_MFRQ); // Match frequency mode, overflow at CC0
tc_count16_set_cc(1, 0, 125); // 62,500 / 125 = 500 Hz
tc_enable(1);
```

Consistent process for flashing firmware
----------------------------------------

All of these chips can function with a USB bootloader, and Gossamer is designed to work within that setup. It provides a consistent method for flashing code to a device: running `make && make install` builds your application and flashes it to a device.

* SAM D21, L21 and L22 chips are meant to use [Adafruit's UF2 bootloader](https://github.com/adafruit/uf2-samdx1); just double tap Reset to enter bootloader mode.
* SAM D11 chips can use the [DFU bootloader](https://github.com/majbthrd/SAMDx1-USB-DFU-Bootloader), although this is still a bit experimental.

Simple framework for writing firmware
-------------------------------------

Much like an Arduino sketch has setup and loop functions, a Gossamer application is implemented in three functions:

* `app_init` is called after the system clocks are initialized and before anything else. This is an opportunity to change any low-level defaults like processor speed, and configure any system clocks and long-running peripherals like the RTC.
* `app_setup` is akin to the Arduino `setup()` function, and provides an opportunity to set up your application's initial state.
* `app_loop` is akin to the Arduino `loop()` function, with the difference that your application will return a boolean value indicating whether the application is prepared to enter STANDBY mode:
    * Returning `false` here ensures that your `app_loop` will be called again immediately.
    * Returning `true` will send the microcontroller into STANDBY mode, where it will remain until woken by an interrupt. This is a fantastic way to build low power firmware.

Simple to get started!
----------------------

Boards are defined in the `/boards` directory, which sets up the target as well as predefined macros for all the pins on a given board. So, for example, this is how you get to blinky on any board with a built in LED:

```c
#include "app.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
}

bool app_loop(void) {
    HAL_GPIO_LED_toggle();
    delay_ms(500);
    return false;
}
```

A more complex app, which displays "Hello" on an [Oddly Specific LCD](https://www.adafruit.com/product/5581), makes use of the pin mux assignment macros and the I2C peripheral:

```c
#include "app.h"
#include "i2c.h"
#include "drivers/oso_lcd.h"

void app_init(void) {
}

void app_setup(void) {
    // I2C direction set
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    // I2C pin mux
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    // initialize the I2C peripheral
    i2c_init();
    i2c_enable();

    // display text on the LCD
    oso_lcd_begin();
    oso_lcd_fill(0);
    oso_lcd_print("hello");
}

bool app_loop(void) {
    return true;
}
```
