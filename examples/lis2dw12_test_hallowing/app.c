#include "app.h"
#include "i2c.h"
#include "usb.h"
#include "tusb.h"
#include "delay.h"
#include <ctype.h>
#include <stdarg.h>
#define LIS2DW_DATA_RATE_100_HZ 0b0101

#define LIS2DW_ADDRESS (0x19)
#define LIS2DW_REG_WHO_AM_I 0x0F
#define LIS2DW_REG_CTRL1 0x20
#define LIS2DW_CTRL1_VAL_ODR_100HZ (LIS2DW_DATA_RATE_100_HZ << 4)
#define LIS2DW_REG_CTRL2 0x21
#define LIS2DW_CTRL2_VAL_BOOT 0b10000000
#define LIS2DW_CTRL2_VAL_SOFT_RESET 0b01000000
#define LIS2DW_REG_CTRL3 0x22
#define LIS2DW_CTRL3_VAL_H_L_ACTIVE 0b00001000
#define LIS2DW_REG_CTRL4_INT1 0x23
#define LIS2DW_CTRL4_INT1_DRDY 0b00000001
#define LIS2DW_REG_CTRL5_INT2 0x24
#define LIS2DW_CTRL5_INT2_SLEEP_STATE 0b10000000
#define LIS2DW_CTRL5_INT2_SLEEP_CHG   0b01000000
#define LIS2DW_REG_CTRL7 0x3F
#define LIS2DW_CTRL7_VAL_INT2_ON_INT1 0b01000000
#define LIS2DW_CTRL7_VAL_INTERRUPTS_ENABLE 0b00100000

#define LIS2DW_WHO_AM_I_VAL 0x44    ///< Expected value of the WHO_AM_I register

static void cdc_task(void);

void watch_i2c_send(int16_t addr, uint8_t *buf, uint16_t length) {
    // printf("Sending %d bytes to address: 0x%02X\n", length, addr);
    // for (int i = 0; i < length; i++) {
    //     printf("0x%02X ", buf[i]);
    // }
    // printf("\n");
    i2c_write(addr, buf, length);
}

void watch_i2c_receive(int16_t addr, uint8_t *buf, uint16_t length) {
    // printf("Receiving %d bytes from address: 0x%02X\n", length, addr);
    i2c_read(addr, buf, length);
}

void watch_i2c_write8(int16_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    watch_i2c_send(addr, (uint8_t *)&buf, 2);
}

uint8_t watch_i2c_read8(int16_t addr, uint8_t reg) {
    uint8_t data;

    watch_i2c_send(addr, (uint8_t *)&reg, 1);
    watch_i2c_receive(addr, (uint8_t *)&data, 1);

    return data;
}

void app_init(void) {
    HAL_GPIO_D12_out();
    HAL_GPIO_D1_out();
    HAL_GPIO_D12_clr();
    HAL_GPIO_D1_clr();
}

void app_setup(void) {
    usb_init();
    usb_enable();

    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_D5_in();
    HAL_GPIO_D6_in();
    i2c_init();
    i2c_enable();
}

bool connected = false;
bool passed = true;

bool app_loop(void) {
    uint8_t whoami = watch_i2c_read8(LIS2DW_ADDRESS, LIS2DW_REG_WHO_AM_I);
    int int1 = 0;
    int int2 = 0;

    if (whoami == LIS2DW_WHO_AM_I_VAL && !connected) {
        printf("WHO_AM_I: 0x%02X\n", whoami);
        // we are connected. run the test
        connected = true;
        // Set the BOOT bit to retrieve the correct trimming parameters from NVM
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL2, LIS2DW_CTRL2_VAL_BOOT);
        // soft reset all control registers
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL2, LIS2DW_CTRL2_VAL_SOFT_RESET);
        // set data rate
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL1, LIS2DW_CTRL1_VAL_ODR_100HZ);
        // set the DRDY interrupt on INT1
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL4_INT1, LIS2DW_CTRL4_INT1_DRDY);
        // set the boot state interrupt on INT2
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL5_INT2, LIS2DW_CTRL5_INT2_SLEEP_STATE | LIS2DW_CTRL5_INT2_SLEEP_CHG);
        // retrieve the CTRL3 and CTRL7 configuration
        uint8_t ctrl3 = watch_i2c_read8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL3);
        // but tweak CTRL7 to enable interrupts
        uint8_t ctrl7 = watch_i2c_read8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL7 | LIS2DW_CTRL7_VAL_INTERRUPTS_ENABLE);
        // enable interrupts
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL7, ctrl7);
        delay_ms(100);
        // get state of INT1 and INT2
        int1 = HAL_GPIO_D5_read();
        int2 = HAL_GPIO_D6_read();
        printf("INT1: %d (expected 1), INT2: %d (expected 0)\n", int1, int2);
        if (int1 != 1) {
            printf("INT1 FAILED\n");
            passed = false;
        }
        if (int2 != 0) {
            printf("INT2 FAILED\n");
            passed = false;
        }
        // now flip the polarity of the interrupt
        watch_i2c_write8(LIS2DW_ADDRESS, LIS2DW_REG_CTRL3, ctrl3 | LIS2DW_CTRL3_VAL_H_L_ACTIVE);
        delay_ms(100);
        // get state of INT1 and INT2
        int1 = HAL_GPIO_D5_read();
        int2 = HAL_GPIO_D6_read();
        printf("INT1: %d (expected 0), INT2: %d (expected 1)\n", int1, int2);
        if (int1 != 0) {
            printf("INT1 FAILED\n");
            passed = false;
        }
        if (int2 != 1) {
            printf("INT2 FAILED\n");
            passed = false;
        }

        if (passed) {
            printf("PASSED!\n");
            HAL_GPIO_D12_set();
            HAL_GPIO_D1_clr();
            while (1) yield();
        } else {
            printf("FAILED\n");
            HAL_GPIO_D12_clr();
            HAL_GPIO_D1_set();
            while (1) yield();
        }
    }

    delay_ms(100);

    return false;
}

void yield(void) {
    tud_task();
    cdc_task();
}

// Size of the circular buffer. Must be a power of two.
#define CDC_WRITE_BUF_SZ  (1024)
// Macro function to perform modular arithmetic on an index.
// eg. (63 + 2) & (64 - 1) -> 1
#define CDC_WRITE_BUF_IDX(x)  ((x) & (CDC_WRITE_BUF_SZ - 1))
static char s_write_buf[CDC_WRITE_BUF_SZ] = {0};
static size_t s_write_buf_pos = 0;
static size_t s_write_buf_len = 0;

#define CDC_READ_BUF_SZ  (256)
#define CDC_READ_BUF_IDX(x)  ((x) & (CDC_READ_BUF_SZ - 1))
static char s_read_buf[CDC_READ_BUF_SZ] = {0};
static size_t s_read_buf_pos = 0;
static size_t s_read_buf_len = 0;

int _write(int file, char *ptr, int len) {
    (void) file;

    if (ptr == NULL || len <= 0) {
        return -1;
    }

    int bytes_written = 0;

    for (int i = 0; i < len; i++) {
        s_write_buf[s_write_buf_pos] = ptr[i];
        s_write_buf_pos = CDC_WRITE_BUF_IDX(s_write_buf_pos + 1);
        if (s_write_buf_len < CDC_WRITE_BUF_SZ) {
            s_write_buf_len++;
        }
        bytes_written++;
    }

    return bytes_written;
}

static void prv_handle_reads(void) {
    while (tud_cdc_available()) {
        int c = tud_cdc_read_char();
        if (c < 0) {
            continue;
        }
        s_read_buf[s_read_buf_pos] = c;
        s_read_buf_pos = CDC_READ_BUF_IDX(s_read_buf_pos + 1);
        if (s_read_buf_len < CDC_READ_BUF_SZ) {
            s_read_buf_len++;
        }
    }
}

static void prv_handle_writes(void) {
    if (s_write_buf_len > 0) {
        const size_t start_pos =
            CDC_WRITE_BUF_IDX(s_write_buf_pos - s_write_buf_len);
        for (size_t i = 0; i < (size_t) s_write_buf_len; i++) {
            const size_t idx = CDC_WRITE_BUF_IDX(start_pos + i);
            if (tud_cdc_available() > 0) {
                // If we receive data while doing a large write, we need to
                // fully service it before continuing to write, or the
                // stack will crash.
                prv_handle_reads();
            }
            if (tud_cdc_write_available()) {
                tud_cdc_write(&s_write_buf[idx], 1);
            }
            s_write_buf[idx] = 0;
            s_write_buf_len--;
        }
        tud_cdc_write_flush();
    }
}

void cdc_task(void) {
    prv_handle_reads();
    prv_handle_writes();
}
