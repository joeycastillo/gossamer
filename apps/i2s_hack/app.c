#include "app.h"
#include "i2c.h"
#include "spi.h"
#include "dma.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>
#include "../clock/drivers/oso_lcd.h"

// WIP, hacky attempt to read an I2S mic with a SERCOM in SPI mode.
// Wire BCLK to SCK, LRCL to MOSI, DOUT to MISO.
// Oddly Specific LCD on the I2C pins.
// DOES NOT CURRENTLY WORK. Seems to only display one sample and not recieve any more,
// even though the scope trace shows data on the DOUT line...

void app_init(void) {
}

uint8_t txBuffer[] = {0x7F, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00};
volatile uint64_t rxData;

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);

    HAL_GPIO_SCK_out();
    HAL_GPIO_MOSI_out();
    HAL_GPIO_MISO_in();
    HAL_GPIO_SCK_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_MOSI_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_MISO_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    spi_init(SPI_MODE_2, 8000000);
    spi_enable();

    i2c_init();
    i2c_enable();
    oso_lcd_begin();
    oso_lcd_fill(0);

    gossamer_dma_job_t spi_rx, spi_tx;

    dma_init();
    dma_configure(&spi_tx,
                  SERCOM4_DMAC_ID_TX,
                  DMA_TRIGGER_ACTION_BEAT,
                  DMA_CONFIG_LOOP);
    dma_add_descriptor(&spi_tx,
                       (void *)txBuffer,
                       (void *)&SERCOM4->SPI.DATA,
                       sizeof(txBuffer),
                       DMA_BEAT_SIZE_BYTE,
                       DMA_INCREMENT_SOURCE,
                       DMA_STEPSIZE_1,
                       DMA_STEPSEL_SOURCE);

    dma_configure(&spi_rx,
                  SERCOM4_DMAC_ID_RX,
                  DMA_TRIGGER_ACTION_BEAT,
                  DMA_CONFIG_LOOP);
    dma_add_descriptor(&spi_rx,
                       (void *)&SERCOM4->SPI.DATA,
                       (void *)&rxData,
                       sizeof(rxData),
                       DMA_BEAT_SIZE_BYTE,
                       DMA_INCREMENT_DESTINATION,
                       DMA_STEPSIZE_1,
                       DMA_STEPSEL_DESTINATION);

    dma_start_job(&spi_rx);
    dma_start_job(&spi_tx);
}

bool app_loop(void) {
    char buf[32];
    // uint16_t data = (rxData >> 24) & 0xFFFF;
    // sprintf(buf, "%043x ", data);
    sprintf(buf, "%llu", rxData);
    oso_lcd_print(buf);

    return false;
}
