#include "app.h"
#include "delay.h"
#include "tcc.h"
#include "tc.h"
#include <math.h>

#define RUNTIME_MINUTES (60)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static const uint8_t flicker[] = {72, 74, 71, 77, 89, 112, 114, 122, 147, 129, 153, 162, 160, 172, 135, 99, 110, 102, 117, 151, 146, 139, 157, 137, 135, 161, 144, 131, 97, 96, 98, 138, 140, 118, 112, 81, 110, 132, 160, 143, 139, 116, 125, 181, 178, 173, 154, 99, 69, 52, 44, 75, 76, 95, 114, 109, 159, 177, 193, 179, 178, 173, 163, 186, 169, 156, 123, 117, 80, 53, 89, 76, 123, 166, 127, 146, 145, 133, 128, 141, 153, 151, 162, 150, 116, 136, 123, 142, 119, 130, 130, 78, 78, 58, 63, 93, 133, 142, 184, 187, 180, 185, 135, 152, 150, 153, 214, 213, 215, 201, 189, 140, 156, 121, 89, 92, 67, 82, 65, 82, 115, 94, 103, 80, 64, 121, 148, 152, 156, 146, 156, 198, 202, 191, 192, 148, 119, 85, 63, 115, 108, 163, 193, 153, 137, 129, 108, 147, 171, 133, 99, 77, 81, 122, 123, 139, 125, 100, 120, 114, 135, 117, 115, 92, 67, 101, 123, 152, 194, 159, 174, 149, 116, 154, 142, 139, 124, 104, 76, 64, 115, 143, 170, 216, 169, 135, 92, 78, 127, 105, 112, 88, 81, 140, 147, 138, 93, 77, 68, 103, 128, 137, 182, 174, 156, 129, 92, 91, 124, 140, 176, 161, 167, 170, 133, 151, 113, 82, 70, 76, 102, 123, 163, 147, 163, 176, 171, 204, 178, 178, 194, 162, 155, 124, 118, 107, 133, 159, 152, 188, 162, 170, 138, 145, 134, 106, 146, 130, 130, 108, 119, 104, 151, 202, 210, 187, 173, 168, 123, 146, 120, 131, 143, 157, 182, 140, 152, 106, 85, 84, 80, 89, 124, 133, 97, 98, 69, 59, 96, 126, 123, 124, 143, 153, 134, 120, 92, 62, 83, 129, 119, 118, 144, 126, 119, 154, 116, 93, 81, 41, 67, 81, 126, 128, 129, 150, 122, 130, 113, 73, 81, 75, 76, 134, 143, 155, 183, 145, 163, 152, 120, 128, 108, 143, 144, 143, 126, 115, 114, 101, 111, 102, 131, 144, 145, 146, 133, 128, 128, 130, 125, 153, 130, 126, 137, 87, 128, 126, 136, 158, 119, 100, 91, 82, 111, 143, 98, 146, 131, 93, 116, 63, 86, 80, 62, 84, 95, 125, 162, 186, 175, 202, 166, 139, 116, 84, 117, 121, 158, 189, 193, 208, 160, 130, 142, 117, 144, 145, 140, 174, 185, 196, 197, 163, 151, 160, 117, 97, 93, 78, 87, 122, 149, 127, 143, 151, 109, 150, 143, 138, 169, 153, 173, 170, 169, 133, 130, 104, 73, 73, 56, 58, 102, 137, 156, 158, 156, 159, 162, 165, 117, 111, 111, 136, 142, 129, 93, 88, 105, 126, 174, 200, 198, 185, 185, 166, 171, 177, 167, 125, 133, 160, 161, 195, 159, 117, 71, 60, 77, 106, 160, 191, 208, 201, 166, 129, 140, 123, 161, 163, 113, 121, 69, 85, 118, 109, 151, 149, 111, 128, 92, 98, 126, 138, 164, 152, 156, 135, 146, 146, 176, 187, 134, 128, 104, 119, 172, 185, 150, 146, 155, 129, 135, 144, 100, 93, 91, 36, 30, 55, 81, 129, 147, 131, 113, 66, 72, 61, 95, 123, 121, 162, 151, 179, 186, 167, 142, 126, 134, 151, 171, 131, 135, 115, 118, 166, 114, 114, 102, 106, 136, 175, 151, 152, 136, 138, 166, 133, 121, 98, 109, 124, 121, 92, 71, 88, 103, 136, 178, 150, 164, 126, 69, 73, 102, 117, 125, 137, 104, 120, 124, 139, 138, 158, 172, 170, 159, 127, 115, 105, 133, 115, 159, 113, 129, 135, 127, 128, 113, 164, 150, 174, 165, 150, 138, 156, 175, 143, 149, 135, 119, 107, 107, 94, 110, 165, 182, 198, 161, 119, 116, 123, 148, 142, 127, 90, 68, 99, 78, 110, 125, 131, 171, 195, 189, 155, 165, 152, 185, 191, 169, 145, 94, 105, 75, 80, 130, 154, 202, 184, 154, 149, 117, 165, 147, 122, 104, 79, 110, 104, 115, 115, 88, 117, 108, 134, 140, 121, 124, 127, 149, 165, 200, 188, 184, 193, 189, 173, 194, 144, 151, 168, 134, 180, 146, 100, 133, 142, 184, 211, 212, 201, 156, 146, 112, 78, 89, 113, 113, 131, 139, 117, 129, 121, 138, 126, 89, 115, 74, 83, 100, 115, 166, 155, 149, 90, 77, 115, 157, 183, 137, 139, 134, 128, 141, 93, 56, 41, 32, 41, 73, 70, 91, 94, 108, 138, 130, 124, 106, 123, 163, 211, 170, 158, 118, 73, 86, 90, 73, 111, 131, 143, 144, 94, 121, 98, 115, 120, 108, 79, 109, 158, 154, 190, 147, 103, 98, 68, 119, 133, 116, 112, 96, 91, 126, 165, 146, 138, 91, 91, 74, 117, 171, 134, 168, 163, 125, 138, 141, 133, 165, 204, 163, 183, 148, 141, 180, 160, 140, 136, 157, 146, 204, 199, 153, 177, 139, 124, 169, 139, 154, 158, 107, 129, 94, 63, 109, 111, 123, 160, 116, 106, 124, 113, 142, 112, 99, 119, 127, 170, 154, 149, 164, 155, 192, 196, 179, 135, 140, 142, 117, 173, 189, 174, 167, 170, 145, 111, 148, 111, 91, 114, 94, 114, 134, 157, 177, 189, 210, 166, 138, 133, 113, 171, 200, 197, 197, 177, 175, 133, 127, 117, 85, 113, 114, 127, 148, 125, 136, 109, 86, 96, 106, 100, 131, 173, 145, 181, 186, 162, 171, 170, 145, 121, 102, 102, 127, 121, 114, 115, 66, 104, 136, 151, 173, 191, 157, 148, 182, 131, 159, 153, 112, 125, 121, 73, 81, 73, 105, 106, 95, 109, 53, 70, 112, 121, 172, 169, 119, 139, 134, 122, 154, 148, 145, 148, 111, 64, 78, 120, 151, 152, 102, 91, 80, 97, 101, 110, 92, 124, 128, 113, 148, 93, 130, 122, 105, 109, 75, 74, 84, 100, 109, 136, 126, 130, 129, 95, 85, 99, 127, 174, 211, 181, 137, 116, 71, 60, 81, 52, 82, 99, 101, 96, 112, 98, 100, 113, 64, 125, 100, 121, 153, 104, 138, 126, 141, 175, 151, 140, 122, 128, 162, 201, 213, 197, 166, 112, 72, 90, 72, 122, 161, 167, 171, 148, 107, 79, 107, 79, 124, 142, 128, 140, 112, 61, 59, 82, 118, 169, 151, 120, 117, 117, 137, 161, 122, 128, 112, 139, 147, 150, 168, 137, 132, 80, 98, 87};

void app_init(void) {
}

void app_setup(void) {
    // red and white pins are floating, which means the NMOS output can float.
    // pull these lines low first thing.
    HAL_GPIO_WARM_WHITE_out();
    HAL_GPIO_COOL_WHITE_out();
    HAL_GPIO_WARM_WHITE_clr();
    HAL_GPIO_COOL_WHITE_clr();

    HAL_GPIO_BUTTON1_in();
    HAL_GPIO_BUTTON1_pullup();
    HAL_GPIO_BUTTON2_in();
    HAL_GPIO_BUTTON2_pullup();

    // configure LEDs as outputs
    HAL_GPIO_RED1_out();
    HAL_GPIO_RED2_out();
    HAL_GPIO_RED3_out();
    HAL_GPIO_GREEN1_out();
    HAL_GPIO_GREEN2_out();
    HAL_GPIO_GREEN3_out();

    // and drive them high to turn off all LEDs.
    HAL_GPIO_RED1_set();
    HAL_GPIO_RED2_set();
    HAL_GPIO_RED3_set();
    HAL_GPIO_GREEN1_set();
    HAL_GPIO_GREEN2_set();
    HAL_GPIO_GREEN3_set();

    // configure TCC0 to drive red/green LED pins
    tcc_init(0, GENERIC_CLOCK_2, TCC_PRESCALER_DIV1);
    HAL_GPIO_RED1_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_RED2_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_RED3_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_GREEN1_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_GREEN2_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_GREEN3_pmuxen(HAL_GPIO_PMUX_TCC_ALT);

    tcc_set_period(0, 256);
    tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    tcc_set_output_matrix(0, TCC_OUTPUT_MATRIX_MODULO_2);
    tcc_set_channel_polarity(0, 0, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_channel_polarity(0, 1, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_channel_polarity(0, 2, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_channel_polarity(0, 3, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_channel_polarity(0, 4, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_channel_polarity(0, 5, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_run_in_standby(0, true);

    tcc_set_cc(0, 0, 0);
    tcc_set_cc(0, 1, 0);
    tcc_set_cc(0, 2, 0);
    tcc_set_cc(0, 3, 0);
    tcc_enable(0);
}

const int maxLoops = 1400 * RUNTIME_MINUTES; // 1400 loops keeps the light on about a minute.
int loops = 1400 * RUNTIME_MINUTES;
int mode = 0;

bool app_loop(void) {
    static int i = 0;
    static int j = 0;
    static int k = 0;

    // In mode 0:
    // CC[0] controls all green LEDs
    // CC[1] controls all red LEDs

    // In mode 1:
    // CC[0] controls GREEN1 and GREEN3
    // CC[1] controls RED1 and RED3
    // CC[2] controls GREEN2
    // CC[3] controls RED2

    // In mode 2:
    // CC[0] and CC[1] control the color,
    // while the PATT register controls the brightness.
    switch (mode) {
        case 0:
        case 1:
            tcc_set_cc(0, 0, (flicker[i + 0] / 6) >> MAX(maxLoops / loops - 1, 0));
            tcc_set_cc(0, 1, (flicker[i + 0] / 2) >> MAX(maxLoops / loops - 1, 0));
            // in mode 0, these will have no effect since CC0 and CC1 control everything.
            tcc_set_cc(0, 2, (flicker[i + 2] / 6) >> MAX(maxLoops / loops - 1, 0));
            tcc_set_cc(0, 3, (flicker[i + 2] / 2) >> MAX(maxLoops / loops - 1, 0));

            i = (i + 1) % (sizeof(flicker) - 2);
            delay_ms(50);
            break;
        case 2:
            // we're moving too fast in this mode, disable naps.
            loops = maxLoops;
            if (i % 4 == 0) {
                TCC0->PATT.reg = (!!((flicker[j + 0] >> (7 - k)) > 0)) << TCC_PATT_PGE0_Pos |
                                 (!!((flicker[j + 0] >> (7 - k)) > 0)) << TCC_PATT_PGE1_Pos |
                                 (!!((flicker[j + 2] >> (7 - k)) > 0)) << TCC_PATT_PGE2_Pos |
                                 (!!((flicker[j + 2] >> (7 - k)) > 0)) << TCC_PATT_PGE3_Pos |
                                 (!!((flicker[j + 4] >> (7 - k)) > 0)) << TCC_PATT_PGE4_Pos |
                                 (!!((flicker[j + 4] >> (7 - k)) > 0)) << TCC_PATT_PGE5_Pos;
            } else {
                TCC0->PATT.reg = 0x000000FF;
            }
            while (TCC0->SYNCBUSY.bit.PATT);
            if (i++ >= 1024) {
                i = 0;
                j++;
            }
            if (k++ > 7) {
                k = 0;
                j = (j + 4) % (sizeof(flicker) - 4);
            }
    }

    if (HAL_GPIO_BUTTON1_read() == false) {
        mode = 0;
        loops = 0;
    }
    if (HAL_GPIO_BUTTON2_read() == false) {
        mode = (mode + 1) % 3;
        loops = maxLoops;
        i = 0;
        j = 0;
        tcc_set_cc(0, 0, 0);
        tcc_set_cc(0, 1, 0);
        tcc_set_cc(0, 2, 0);
        tcc_set_cc(0, 3, 0);
        tcc_disable(0);
        switch (mode) {
            case 0:
                TCC0->PATT.reg = 0;
                tcc_set_output_matrix(0, TCC_OUTPUT_MATRIX_MODULO_2);
                break;
            case 1:
                tcc_set_output_matrix(0, TCC_OUTPUT_MATRIX_MODULO_4);
                tcc_enable(0);
                break;
            case 2:
                tcc_set_output_matrix(0, TCC_OUTPUT_MATRIX_MODULO_2);
                break;
        }
        tcc_enable(0);
        delay_ms(500);
        tcc_set_cc(0, 0, 85);
        tcc_set_cc(0, 1, 255);
        tcc_set_cc(0, 2, 85);
        tcc_set_cc(0, 3, 255);
    }

    if (--loops <= 0) {
        HAL_GPIO_RED1_pmuxdis();
        HAL_GPIO_GREEN1_pmuxdis();
        HAL_GPIO_RED2_pmuxdis();
        HAL_GPIO_GREEN2_pmuxdis();
        HAL_GPIO_RED3_pmuxdis();
        HAL_GPIO_GREEN3_pmuxdis();
        HAL_GPIO_RED1_set();
        HAL_GPIO_RED2_set();
        HAL_GPIO_RED3_set();
        HAL_GPIO_GREEN1_set();
        HAL_GPIO_GREEN2_set();
        HAL_GPIO_GREEN3_set();
        tcc_disable(0);

        return true;
    }

    return false;
}
