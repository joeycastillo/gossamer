// Ported from Adafruit_GFX
// See LICENSE for license text

#include "st77xx.h"
#include "delay.h"

static void spiWrite(uint8_t b) {
    hwspi._spi->transfer(b);
}

static void sendCommand(uint8_t commandByte, uint8_t *dataBytes, uint8_t numDataBytes) {
    SPI_BEGIN_TRANSACTION();
    if (_cs >= 0)
        SPI_CS_LOW();

    SPI_DC_LOW();          // Command mode
    spiWrite(commandByte); // Send the command byte

    SPI_DC_HIGH();
    for (int i = 0; i < numDataBytes; i++)
    {
        spiWrite(*dataBytes); // Send the data bytes
        dataBytes++;
    }

    if (_cs >= 0)
        SPI_CS_HIGH();
    SPI_END_TRANSACTION();
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
        sendCommand(cmd, addr, numArgs);
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
