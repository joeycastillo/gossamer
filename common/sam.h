#pragma once

#define HAL_GPIO_PMUX_EIC HAL_GPIO_PMUX_A
#define HAL_GPIO_PMUX_ADC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_AC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_PTC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_SERCOM HAL_GPIO_PMUX_C
#define HAL_GPIO_PMUX_SERCOM_ALT HAL_GPIO_PMUX_D
#define HAL_GPIO_PMUX_TC_TCC HAL_GPIO_PMUX_E

#if defined(__SAMD11C14A__) || defined(__ATSAMD11C14A__) || \
    defined(__SAMD11D14AM__) || defined(__ATSAMD11D14AM__) || \
    defined(__SAMD11D14AS__) || defined(__ATSAMD11D14AS__) || \
    defined(__SAMD11D14AU__) || defined(__ATSAMD11D14AU__)
#include "samd11.h"
#define HAL_GPIO_PMUX_DAC HAL_GPIO_PMUX_B
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
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
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H
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
#define HAL_GPIO_PMUX_TCC_ALT HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_G
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_SUPC HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_CCL HAL_GPIO_PMUX_I
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
#define HAL_GPIO_PMUX_RTC_PB01 HAL_GPIO_PMUX_E
#define HAL_GPIO_PMUX_COM HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_RTC HAL_GPIO_PMUX_F
#define HAL_GPIO_PMUX_GCLK HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_SUPC HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_AC_OUTPUT HAL_GPIO_PMUX_H
#define HAL_GPIO_PMUX_CCL HAL_GPIO_PMUX_I
#endif
