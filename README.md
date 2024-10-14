/// @mainpage

gossamer: a very lightweight firmware framework for SAMD and SAML chips
=======================================================================

Gossamer is an extremely lightweight framework for developing bare-metal firmware applications for the Microchip SAM D and SAM L series chips. It aims for consistency, simplicity and low power consumption.

Gossamer targets four chips at this time: the SAM D11, D21, L21 and L22. These chips are all strikingly similar: they all have Cortex M0+ cores as well as native USB and as familiar peripherals like ADC's, TCC's and SERCOMs. Alas, a lot of the interfaces to these peripherals have slight differences, and at startup, these chips have subtly different clock setups.

The primary goal of the project is to provide a simple environment for writing firmware for these chips. The secondary goal is to ensure that the code you write will be compatible across chips in the family. By papering over subtle differences in syntax and setup, you should be able to write code for one chip in the family (like the SAM D21), and then seamlessly compile it for another, like the SAM D11 if you realize you need to cut BOM costs, or the L21 if you decide that low power is your priority.

Not all devices have all the same peripherals. Some features, like the segment LCD, are specific to one chip like the SAM L22. Even some devices that do have the same peripherals have different configuration options; for example, the DAC peripheral has two 12-bit channels on the SAM L21, but only one 10-bit channel on the SAM D21 and D11. Still, many of the peripherals are very similar on the different platforms, and gossamer provides a thin layer of translation on top that should make it easy to move up and down the product line.

Clock Tree
----------

If you look at the data sheets for these chips, you'll see that some boot with a 1 MHz clock, and others with a 4 MHz clock. No generic clocks are defined, and if you're coming from the Atmel Start world, you'll recall that it was on you to invent a clock tree from sceatch. 

Gossamer simplifies this by giving you a consistent environment on all four platforms:

* GCLK0, the main system clock, is set to 8 MHz at boot. You can change this in `app_init` using the `set_cpu_frequency` function.
* GCLK1 is reserved for the 48 MHz DFLL clock, which is only configured if the USB peripheral is enabled.
* GCLK2 is a low-power, low-accuracy 32 kHz clock sourced from OSCULP32K.
* GCLK3 is a 1024 Hz clock, derived from the most accurate source available.

Some peripherals are automatically clocked from the source that makes the most sense (i.e. RTC from GCLK3, EIC from GCLK2). SERCOM peripherals are clocked from the main clock, but provide a configurable baud rate in their init functions. More versatile peripherals like the TC and TCC take a generic clock and prescaler factor as parameters in their init functions.

Application Framework
---------------------

Much like an Arduino sketch has setup and loop functions, a Gossamer application is implemented in three functions:

* `app_init` is called after the system clocks are initialized, and before anything else. This is an opportunity to change any low-level defaults like processor speed, and configure any additional system clocks and long-running peripherals like the RTC.
* `app_setup` is akin to the Arduino `setup()` function, and provides an opportunity to set up your application's initial state.
* `app_loop` is akin to the Arduino `loop()` function, with the difference that your application will return a boolean value indicating whether the application is prepared to enter STANDBY mode:
    * Returning `false` here ensures that your `app_loop` will be called again immediately.
    * Returning `true` will send the microcontroller into STANDBY mode, where it will remain until woken by an interrupt.

This last bit — entering standby mode as often as possible — is a key feature of gossamer. In standby mode, the CPU is halted, but peripherals set up to run in standby can still run, and wake the device on an interrupt like a button press, timer tick or even the receipt of data over a UART. 

This is a great way to conserve energy in battery-powered applications, and gossamer makes it easy.

Peripheral Interface
--------------------

Peripherals are generally thin wrappers around raw register reads and writes, with little more than synchronization logic tacked on. Wrapping the raw register twiddling in function calls allows us to paper over subtle implementation details and hand-wave away register layout differences, like the difference between waiting on `STATUS.bit.SYNCBUSY` vs `SYNCBUSY.reg`. This provides a consistent way to enable, disable and operate peripherals:

* `peripheral_init` enables the peripheral clocks and configures the peripheral, but does not enable it. Generally returns void, but for peripherals whose initialization can fail (i.e. due to a bad parameter), some init functions will return a boolean value: `true` if successful, `false` otherwise.
* `peripheral_enable` enables the peripheral (usually a `CTRLA.bit.ENABLE`, with associated `SYNCBUSY` wait). Some peripherals have multiple channels (DAC) or instances (SERCOM). In these cases, the enable function takes as a parameter which instance to enable.
* `peripheral_disable` disables the peripheral.
    * For peripherals with multiple channels, this disables the channel, unless all channels are disabled in which case it disables the whole peripheral.
    * For peripherals with multiple instances, this only disables the specified instance.

For example, this is how you might set up the RTC peripheral, which has predefined clock defaults:

```c
rtc_init();
rtc_enable();
```

This is how you might output an analog value on DAC channel 0 (the first DAC on the SAM L21, or the only DAC on D21):

```c
HAL_GPIO_A0_pmuxen(HAL_GPIO_PMUX_DAC);
dac_init();
dac_enable(0);
dac_set_analog_value(0, 512);
```

This is how you would set up Timer/Counter TC1 to overflow at 500 Hz (i.e. for driving a DMA transaction):

```c
tc_init(1, GENERIC_CLOCK_0, TC_PRESCALER_DIV256); // 8 MHz / 256 = 125,000
tc_set_counter_mode(1, TC_COUNTER_MODE_8BIT)
tc_set_wavegen(1, TC_WAVE_WAVEGEN_MFRQ); // Match frequency mode, overflow at CC0
tc_count8_set_cc(1, 0, 250); // 125,000 / 250 = 500 Hz
tc_enable(1);
```

For SERCOM peripherals, which have several instances, we provide a shortcut: if your board defines a `I2C_SERCOM`, `SPI_SERCOM`, or `UART_SERCOM` in its `pins.h`, you can use the `i2c_init`, `spi_init`, or `uart_init` functions without specifying the SERCOM instance. For example, if your board defines a `I2C_SERCOM`, you would set up the I2C peripheral like this:

```c
HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
i2c_init();
i2c_enable();
```

However, if your board does not define `I2C_SERCOM`, you would use the `i2c_init_instance` function instead:

```c
HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
i2c_init_instance(2);
i2c_enable();
```

`SPI_SERCOM` and `UART_SERCOM` work similarly, although you will also need to specify `SPI_SERCOM_DOPO` and `SPI_SERCOM_DIPO`, or `UART_SERCOM_TXPO` and `UART_SERCOM_RXPO` to give Gossamer a hint at your pinout.

Flashing Firmware
-----------------

Gossamer is designed to work with [Adafruit's UF2 bootloader](https://github.com/adafruit/uf2-samdx1) on SAM D21, L21 and L22 chips. For SAM D11 chips, it's designed to work with the [DFU bootloader](https://github.com/majbthrd/SAMDx1-USB-DFU-Bootloader). The UF2 bootloader is a drag-and-drop bootloader that presents itself as a USB drive, while the DFU bootloader is a USB DFU bootloader that can be flashed using the `dfu-util` command line tool.

Either way, gossamer papers over the differences: with a device in bootloader mode plugged in via USB, running `make && make install` builds your application and flashes it to a device.

Emscripten Support
------------------

Gossamer includes a set of "dummy" peripherals that allow your application to compile to WebAssembly using Emscripten. Note that other than the GPIO macros tracking basic pin levels, these peripherals don't actually _do_ anything, but this is an area of interest for future development.

Examples
--------

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
    // I2C pin mux
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
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

Current state and future plans
------------------------------

Gossamer is now stable enough for folks to use; in fact, it's the framework underlying [Second Movement](https://github.com/joeycastillo/second-movement), the new firmware for [Sensor Watch](https://www.sensorwatch.net). Having said that, not all peripherals are implemented, and not all of the ones that are implemented are complete. There are lots of features for which we don't have functions defined, and certain peripherals are only implemented on certain chips.

But hey: pull requests are welcome!
