#include "app.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tcc.h"
#include "delay.h"
#include <stdio.h>
#include "../clock/drivers/oso_lcd.h"

// Ignore this test, it's part of me hacking around with I2S stuff.

uint8_t txBuffer[] = {0xFA, 0x00, 0x00};
volatile uint64_t rxData;

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

    HAL_GPIO_D1_pmuxen(HAL_GPIO_PMUX_TCC_ALT);

    tcc_init(0, GENERIC_CLOCK_0, TCC_PRESCALER_DIV1);
    tcc_set_wavegen(0, TCC_WAVEGEN_MATCH_FREQUENCY);
    tcc_set_cc(0, 0, 256);
    tcc_set_cc(2, 0, 255);
    tcc_enable(0);

    gossamer_dma_job_t spi_rx, spi_tx;

    dma_init();
    dma_configure(&spi_tx,
                  TCC0_DMAC_ID_OVF,
                  DMA_TRIGGER_ACTION_BLOCK,
                  DMA_CONFIG_LOOP);
    dma_add_descriptor(&spi_tx,
                       (void *)txBuffer,
                       (void *)&SERCOM4->SPI.DATA,
                       7,
                       DMA_BEAT_SIZE_BYTE,
                       DMA_INCREMENT_SOURCE,
                       DMA_STEPSIZE_1,
                       DMA_STEPSEL_SOURCE);

    dma_configure(&spi_rx,
                  TCC0_DMAC_ID_OVF,
                  DMA_TRIGGER_ACTION_BLOCK,
                  DMA_CONFIG_LOOP);
    dma_add_descriptor(&spi_rx,
                       (void *)&SERCOM4->SPI.DATA,
                       (void *)&rxData,
                       7,
                       DMA_BEAT_SIZE_BYTE,
                       DMA_INCREMENT_DESTINATION,
                       DMA_STEPSIZE_1,
                       DMA_STEPSEL_DESTINATION);

    dma_start_job(&spi_rx);
    dma_start_job(&spi_tx);
}

bool app_loop(void) {
    char buf[7];
    uint8_t data;

    if ((rxData & 0xFFFF) != 65535) {
        sprintf(buf, "%05d", rxData & 0xFFFF);
        oso_lcd_print(buf);
    }

    return false;
}
