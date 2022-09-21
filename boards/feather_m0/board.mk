CHIP = samd21
LDSCRIPT = samd21j18

include $(TOP)/make.mk

INCLUDES += \
  -I$(TOP)/boards/feather_m0/ \

DEFINES += \
  -D__SAMD21G18A__ \
