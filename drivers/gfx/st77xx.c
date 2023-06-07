// Ported from Adafruit_GFX
// See LICENSE for license text

#include "st77xx.h"
#include "spi.h"
#include "pins.h"
#include "delay.h"

static void spiWrite(uint8_t b) {
    spi_transfer(b);
}

void st77xx_send_command(uint8_t commandByte, uint8_t *dataBytes, uint8_t numDataBytes) {
    HAL_GPIO_TFT_CS_clr();

    HAL_GPIO_TFT_DC_clr();          // Command mode
    spiWrite(commandByte); // Send the command byte

    HAL_GPIO_TFT_DC_set();
    for (int i = 0; i < numDataBytes; i++)
    {
        spiWrite(*dataBytes); // Send the data bytes
        dataBytes++;
    }

    HAL_GPIO_TFT_CS_set();
}

void st77xx_display_init(const uint8_t *addr)
{
    uint8_t numCommands, cmd, numArgs;
    uint16_t ms;

    numCommands = *(addr++); // Number of commands to follow
    while (numCommands--)
    {                                // For each command...
        cmd = *(addr++);             // Read command
        numArgs = *(addr++);         // Number of args to follow
        ms = numArgs & ST_CMD_DELAY; // If hibit set, delay follows args
        numArgs &= ~ST_CMD_DELAY;    // Mask out delay bit
        st77xx_send_command(cmd, (uint8_t *)addr, numArgs);
        addr += numArgs;

        if (ms)
        {
            ms = *(addr++); // Read post-command delay time (ms)
            if (ms == 255)
                ms = 500; // If 255, delay for 500 ms
            delay_ms(ms);
        }
    }
}
