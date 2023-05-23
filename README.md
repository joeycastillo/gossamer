/// @mainpage

gossamer: a very lightweight app framework for SAMD and SAML chips
==================================================================

> gos·sa·mer noun  
> \ ˈgä-sə-mər \  
>  
> 1. a film of cobwebs floating in air in calm clear weather  
> 2. something light, delicate, or insubstantial

gossamer is an extremely lightweight framework for developing bare-metal firmware applications for the Microchip (neé Atmel) SAM D and SAM L series chips. it aims for consistency, simplicity and ease of use.

**STILL UNDER ACTIVE DEVELOPMENT!** The API remains likely to change for the foreseeable future.

Consistency
-----------

Gossamer targets four chips at this time: the SAM D11, D21, L21 and L22. These chips are all strikingly similar: they all have Cortex M0+ cores as well as native USB and as familiar peripherals like ADC's, TCC's and SERCOMs. Alas, a lot of the interfaces to these peripherals have slight differences, and at startup, these chips have subtly different clock setups.

Gossamer simplifies this by giving you a consistent environment on all four platforms:

* At startup, the main system clock GCLK0 is set to 8 MHz.
* GCLK2 is a low-power, low-accuracy 32 kHz clock sourced from OSCULP32K.
* GCLK3 is a 1024 Hz clock, derived from the most accurate source available.

Peripherals are automatically clocked from the source that makes the most sense (i.e. RTC from GCLK3, EIC from GCLK2) and provide sensible defaults like single-ended ADC measurement and 400 kHz I2C. Still, since Gossamer is so close to bare metal, it's easy to override these settings, including the CPU clock speed.

Peripherals are generally thin wrappers around the actual hardware, and provide a consistent way to enable, disable and operate the peripherals. This allows us to paper over subtle implementation details and hand-wave away register layout differences, like the difference between waiting on `STATUS.bit.SYNCBUSY` vs `SYNCBUSY.reg`.

Gossamer also provides a consistent method for flashing code to a device: running `make && make install` builds your application and flashes it to a device using a USB bootloader: 

* SAM D21, L21 and L22 chips are meant to use [Adafruit's UF2 bootloader](https://github.com/adafruit/uf2-samdx1); just double tap Reset to enter bootloader mode.
* SAM D11 chips can use the [DFU bootloader](https://github.com/majbthrd/SAMDx1-USB-DFU-Bootloader), although this is still a bit experimental.

Simplicity
----------

Much like an Arduino sketch has setup and loop functions, a Gossamer application is implemented in three functions:

* `app_init` is called after the system clocks are initialized and before anything else. This is an opportunity to change any low-level defaults like processor speed, and configure any system clocks and long-running peripherals like the RTC.
* `app_setup` is akin to the Arduino `setup()` function, and provides an opportunity to set up your application's initial state.
* `app_loop` is akin to the Arduino `loop()` function, with the difference that your application will return a boolean value indicating whether the appli:
    * Returning `false` here ensures that your `app_loop` will be called again immediately.
    * Returning `true` will send the microcontroller into STANDBY mode, where it will remain until woken by an interrupt. This is a fantastic way to build low power firmware.

Ease of Use
-----------

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

    // display text on the LCD
    oso_lcd_begin();
    oso_lcd_fill(0);
    oso_lcd_print("hello");
}

bool app_loop(void) {
    return true;
}
```
