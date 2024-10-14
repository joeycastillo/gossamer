#pragma once

#if __EMSCRIPTEN__

typedef int generic_clock_generator_t;

#define HAL_GPIO_PMUX_EIC (0)
#define HAL_GPIO_PMUX_ADC (1)
#define HAL_GPIO_PMUX_AC (1)
#define HAL_GPIO_PMUX_PTC (1)
#define HAL_GPIO_PMUX_SERCOM (2)
#define HAL_GPIO_PMUX_SERCOM_ALT (3)

#define DMAC_CHCTRLB_TRIGACT_BLOCK_Val  (0)
#define DMAC_CHCTRLB_TRIGACT_BEAT_Val   (2)
#define DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val (3)

typedef int DmacDescriptor;

#else

#define HAL_GPIO_PMUX_EIC HAL_GPIO_PMUX_A
#define HAL_GPIO_PMUX_ADC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_AC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_PTC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_SERCOM HAL_GPIO_PMUX_C
#define HAL_GPIO_PMUX_SERCOM_ALT HAL_GPIO_PMUX_D

#if defined(__SAMD11C14A__) || defined(__ATSAMD11C14A__) || \
    defined(__SAMD11D14AM__) || defined(__ATSAMD11D14AM__) || \
    defined(__SAMD11D14AS__) || defined(__ATSAMD11D14AS__) || \
    defined(__SAMD11D14AU__) || defined(__ATSAMD11D14AU__)
#include "samd11.h"
#define HAL_GPIO_PMUX_DAC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_TC_TCC HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H

typedef enum {
    GENERIC_CLOCK_0 = GCLK_CLKCTRL_GEN_GCLK0_Val,
    GENERIC_CLOCK_1 = GCLK_CLKCTRL_GEN_GCLK1_Val,
    GENERIC_CLOCK_2 = GCLK_CLKCTRL_GEN_GCLK2_Val,
    GENERIC_CLOCK_3 = GCLK_CLKCTRL_GEN_GCLK3_Val,
    GENERIC_CLOCK_4 = GCLK_CLKCTRL_GEN_GCLK4_Val,
    GENERIC_CLOCK_5 = GCLK_CLKCTRL_GEN_GCLK5_Val,
} generic_clock_generator_t;
#endif

#if defined(__SAMD21E15A__) || defined(__ATSAMD21E15A__) || \
    defined(__SAMD21E16A__) || defined(__ATSAMD21E16A__) || \
    defined(__SAMD21E17A__) || defined(__ATSAMD21E17A__) || \
    defined(__SAMD21E18A__) || defined(__ATSAMD21E18A__) || \
    defined(__SAMD21G15A__) || defined(__ATSAMD21G15A__) || \
    defined(__SAMD21G16A__) || defined(__ATSAMD21G16A__) || \
    defined(__SAMD21G17A__) || defined(__ATSAMD21G17A__) || \
    defined(__SAMD21G17AU__) || defined(__ATSAMD21G17AU__) || \
    defined(__SAMD21G18A__) || defined(__ATSAMD21G18A__) || \
    defined(__SAMD21G18AU__) || defined(__ATSAMD21G18AU__) || \
    defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__) || \
    defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__) || \
    defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__) || \
    defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
#include "samd21.h"
#define HAL_GPIO_PMUX_DAC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_TC_TCC HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H

typedef enum {
    GENERIC_CLOCK_0 = GCLK_CLKCTRL_GEN_GCLK0_Val,
    GENERIC_CLOCK_1 = GCLK_CLKCTRL_GEN_GCLK1_Val,
    GENERIC_CLOCK_2 = GCLK_CLKCTRL_GEN_GCLK2_Val,
    GENERIC_CLOCK_3 = GCLK_CLKCTRL_GEN_GCLK3_Val,
    GENERIC_CLOCK_4 = GCLK_CLKCTRL_GEN_GCLK4_Val,
    GENERIC_CLOCK_5 = GCLK_CLKCTRL_GEN_GCLK5_Val,
    GENERIC_CLOCK_6 = GCLK_CLKCTRL_GEN_GCLK6_Val,
    GENERIC_CLOCK_7 = GCLK_CLKCTRL_GEN_GCLK7_Val,
    GENERIC_CLOCK_8 = GCLK_CLKCTRL_GEN_GCLK8_Val,
} generic_clock_generator_t;
#endif

#if defined(__SAML21E15B__) || defined(__ATSAML21E15B__) || \
    defined(__SAML21E16B__) || defined(__ATSAML21E16B__) || \
    defined(__SAML21E17B__) || defined(__ATSAML21E17B__) || \
    defined(__SAML21E18B__) || defined(__ATSAML21E18B__) || \
    defined(__SAML21G16B__) || defined(__ATSAML21G16B__) || \
    defined(__SAML21G17B__) || defined(__ATSAML21G17B__) || \
    defined(__SAML21G18B__) || defined(__ATSAML21G18B__) || \
    defined(__SAML21J16B__) || defined(__ATSAML21J16B__) || \
    defined(__SAML21J17B__) || defined(__ATSAML21J17B__) || \
    defined(__SAML21J17BU__) || defined(__ATSAML21J17BU__) || \
    defined(__SAML21J18B__) || defined(__ATSAML21J18B__) || \
    defined(__SAML21J18BU__) || defined(__ATSAML21J18BU__)
#include "saml21.h"
#define HAL_GPIO_PMUX_DAC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_OPAMP HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_TC_TCC HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_SUPC HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_CCL HAL_GPIO_PMUX_I

typedef enum {
    GENERIC_CLOCK_0 = GCLK_PCHCTRL_GEN_GCLK0_Val,
    GENERIC_CLOCK_1 = GCLK_PCHCTRL_GEN_GCLK1_Val,
    GENERIC_CLOCK_2 = GCLK_PCHCTRL_GEN_GCLK2_Val,
    GENERIC_CLOCK_3 = GCLK_PCHCTRL_GEN_GCLK3_Val,
    GENERIC_CLOCK_4 = GCLK_PCHCTRL_GEN_GCLK4_Val,
    GENERIC_CLOCK_5 = GCLK_PCHCTRL_GEN_GCLK5_Val,
    GENERIC_CLOCK_6 = GCLK_PCHCTRL_GEN_GCLK6_Val,
    GENERIC_CLOCK_7 = GCLK_PCHCTRL_GEN_GCLK7_Val,
    GENERIC_CLOCK_8 = GCLK_PCHCTRL_GEN_GCLK8_Val,
} generic_clock_generator_t;
#endif

#if defined(__SAML22G16A__) || defined(__ATSAML22G16A__) || \
    defined(__SAML22G17A__) || defined(__ATSAML22G17A__) || \
    defined(__SAML22G18A__) || defined(__ATSAML22G18A__) || \
    defined(__SAML22J16A__) || defined(__ATSAML22J16A__) || \
    defined(__SAML22J17A__) || defined(__ATSAML22J17A__) || \
    defined(__SAML22J18A__) || defined(__ATSAML22J18A__) || \
    defined(__SAML22N16A__) || defined(__ATSAML22N16A__) || \
    defined(__SAML22N17A__) || defined(__ATSAML22N17A__) || \
    defined(__SAML22N18A__) || defined(__ATSAML22N18A__)
#include "saml22.h"
#define HAL_GPIO_PMUX_SLCD HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_TC_TCC HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_RTC_PB01_IN2 HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_RTC HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_SUPC HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_CCL HAL_GPIO_PMUX_I

typedef enum {
    GENERIC_CLOCK_0 = GCLK_PCHCTRL_GEN_GCLK0_Val,
    GENERIC_CLOCK_1 = GCLK_PCHCTRL_GEN_GCLK1_Val,
    GENERIC_CLOCK_2 = GCLK_PCHCTRL_GEN_GCLK2_Val,
    GENERIC_CLOCK_3 = GCLK_PCHCTRL_GEN_GCLK3_Val,
    GENERIC_CLOCK_4 = GCLK_PCHCTRL_GEN_GCLK4_Val,
} generic_clock_generator_t;
#endif

#if defined(__SAMD51G18A__) || defined(__ATSAMD51G18A__) || \
    defined(__SAMD51G19A__) || defined(__ATSAMD51G19A__) || \
    defined(__SAMD51J18A__) || defined(__ATSAMD51J18A__) || \
    defined(__SAMD51J19A__) || defined(__ATSAMD51J19A__) || \
    defined(__SAMD51J20A__) || defined(__ATSAMD51J20A__) || \
    defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__) || \
    defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__) || \
    defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__) || \
    defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
#include "samd51.h"
#define HAL_GPIO_PMUX_TC HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_TCC HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_COM_ALT HAL_GPIO_PMUX_I
#define HAL_GPIO_PMUX_I2S HAL_GPIO_PMUX_J
#define HAL_GPIO_PMUX_PCC HAL_GPIO_PMUX_K
#define HAL_GPIO_PMUX_GMAC HAL_GPIO_PMUX_L
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_M
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_M
#define HAL_GPIO_PMUX_CCL HAL_GPIO_PMUX_N

typedef enum {
    GENERIC_CLOCK_0 = GCLK_PCHCTRL_GEN_GCLK0_Val,
    GENERIC_CLOCK_1 = GCLK_PCHCTRL_GEN_GCLK1_Val,
    GENERIC_CLOCK_2 = GCLK_PCHCTRL_GEN_GCLK2_Val,
    GENERIC_CLOCK_3 = GCLK_PCHCTRL_GEN_GCLK3_Val,
    GENERIC_CLOCK_4 = GCLK_PCHCTRL_GEN_GCLK4_Val,
    GENERIC_CLOCK_5 = GCLK_PCHCTRL_GEN_GCLK5_Val,
    GENERIC_CLOCK_6 = GCLK_PCHCTRL_GEN_GCLK6_Val,
    GENERIC_CLOCK_7 = GCLK_PCHCTRL_GEN_GCLK7_Val,
    GENERIC_CLOCK_8 = GCLK_PCHCTRL_GEN_GCLK8_Val,
    GENERIC_CLOCK_9 = GCLK_PCHCTRL_GEN_GCLK9_Val,
    GENERIC_CLOCK_10 = GCLK_PCHCTRL_GEN_GCLK10_Val,
    GENERIC_CLOCK_11 = GCLK_PCHCTRL_GEN_GCLK11_Val,
} generic_clock_generator_t;
#endif

#endif // __EMSCRIPTEN__