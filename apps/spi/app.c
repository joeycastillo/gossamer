#include "app.h"
#include "i2c.h"
#include "spi.h"
#include "delay.h"
#include <stdio.h>
#include "../clock/drivers/oso_lcd.h"

// simple test with a BME280 breakout board.
// wire to SPI pins on a Feather M0, CS on pin 1 (TX), and an
// Oddly Specific LCD on the I2C pins.
// Should display a frequently changing byte of raw temperature data.

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);

    HAL_GPIO_SCK_out();
    HAL_GPIO_MOSI_out();
    HAL_GPIO_MISO_in();
    HAL_GPIO_D1_out();
    HAL_GPIO_D1_set();
    HAL_GPIO_SCK_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_MOSI_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_MISO_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    spi_init(SPI_MODE_0, 8000000);
    spi_enable();
    HAL_GPIO_D1_clr();
    spi_transfer(0xe0);
    spi_transfer(0xb6);
    HAL_GPIO_D1_set();
    HAL_GPIO_D1_clr();
    spi_transfer(0x74);
    spi_transfer(0b00100111);
    HAL_GPIO_D1_set();
    delay_ms(10);

    i2c_init();
    i2c_enable();
    oso_lcd_begin();
    oso_lcd_fill(0);
}

bool app_loop(void) {
    char buf[7];
    uint8_t data;

    oso_lcd_set_indicator(OSO_LCD_INDICATOR_DATA);
    HAL_GPIO_D1_clr();
    spi_transfer(0xfa);
    data = spi_transfer(0x00);
    data = spi_transfer(0x00);
    HAL_GPIO_D1_set();
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_DATA);

    sprintf(buf, "td: %02x", data);
    oso_lcd_print(buf);

    return false;
}
