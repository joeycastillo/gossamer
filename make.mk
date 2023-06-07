##############################################################################
BUILD = ./build
BIN = firmware

include $(TOP)/boards/$(BOARD)/board.mk

##############################################################################
.PHONY: all directory clean size

# OS detection, adapted from https://gist.github.com/sighingnow/deee806603ec9274fd47
DETECTED_OS :=
ifeq ($(OS),Windows_NT)
  DETECTED_OS = WINDOWS
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    DETECTED_OS = LINUX
  endif
  ifeq ($(UNAME_S),Darwin)
    DETECTED_OS = OSX
  endif
endif
$(if ${VERBOSE},$(info OS detected: $(DETECTED_OS)))

ifeq ($(OS), Windows_NT)
  MKDIR = gmkdir
else
  MKDIR = mkdir
endif

ifeq ($(DETECTED_OS), LINUX)
  MAKEFLAGS += -j `nproc`
endif
ifeq ($(DETECTED_OS), OSX)
  NPROCS = $(shell sysctl hw.ncpu  | grep -o '[0-9]\+')
  MAKEFLAGS += -j $(NPROCS)
endif
ifeq ($(DETECTED_OS), WINDOWS)
  MAKEFLAGS += -j $(NUMBER_OF_PROCESSORS)
endif

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
UF2 = python3 $(TOP)/utils/uf2/uf2conv.py
DFU_CONV = $(TOP)/utils/dfu/dx1elf2dfu
DFU_UTIL = dfu-util

CFLAGS += -W -Wall -Wextra -Wmissing-prototypes -Wmissing-declarations
CFLAGS += --std=gnu99 -Os
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -mcpu=cortex-m0plus -mthumb
CFLAGS += -MD -MP -MT $(BUILD)/$(*F).o -MF $(BUILD)/$(@F).d

LDFLAGS += -mcpu=cortex-m0plus -mthumb
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=$(TOP)/chips/$(CHIP)/linker/$(LDSCRIPT).ld
LDFLAGS += -specs=nosys.specs

LIBS += -lm

INCLUDES += \
  -I$(TOP)/common/ \
  -I$(TOP)/peripherals/ \
  -I$(TOP)/drivers/ \
  -I$(TOP)/boards/$(BOARD)/ \
  -I$(TOP)/chips/$(CHIP)/include/ \

SRCS += \
  $(TOP)/chips/$(CHIP)/startup_$(CHIP).c \
  $(TOP)/chips/$(CHIP)/system_$(CHIP).c \
  $(TOP)/main.c \
  $(TOP)/common/delay.c \
  $(TOP)/peripherals/adc.c \
  $(TOP)/peripherals/dac.c \
  $(TOP)/peripherals/dma.c \
  $(TOP)/peripherals/eic.c \
  $(TOP)/peripherals/i2c.c \
  $(TOP)/peripherals/i2s.c \
  $(TOP)/peripherals/opamp.c \
  $(TOP)/peripherals/ptc.c \
  $(TOP)/peripherals/rtc.c \
  $(TOP)/peripherals/spi.c \
  $(TOP)/peripherals/tc.c \
  $(TOP)/peripherals/tcc.c \
  $(TOP)/peripherals/uart.c \
  $(TOP)/drivers/gfx/gfx.c \
  $(TOP)/drivers/gfx/sh1107.c \
  $(TOP)/drivers/gfx/st77xx.c \
  $(TOP)/drivers/gfx/st7735.c \

ifdef TINYUSB_CDC
CFLAGS += -DAPP_USES_TINYUSB -DCFG_TUD_CDC=$(TINYUSB_CDC)
TINYUSB = 1
endif

ifdef TINYUSB_MSC
CFLAGS += -DAPP_USES_TINYUSB -DCFG_TUD_MSC=$(TINYUSB_MSC)
TINYUSB = 1
endif

ifdef TINYUSB_HID
CFLAGS += -DAPP_USES_TINYUSB -DCFG_TUD_HID=$(TINYUSB_HID)
TINYUSB = 1
endif

ifdef TINYUSB_MIDI
CFLAGS += -DAPP_USES_TINYUSB -DCFG_TUD_MIDI=$(TINYUSB_MIDI)
TINYUSB = 1
endif

ifdef TINYUSB_VENDOR
CFLAGS += -DAPP_USES_TINYUSB -DCFG_TUD_VENDOR=$(TINYUSB_VENDOR)
TINYUSB = 1
endif

ifdef TINYUSB
CFLAGS += -DAPP_USES_TINYUSB

INCLUDES += \
  -I$(TOP)/drivers/tinyusb/src/ \

SRCS += \
	$(TOP)/drivers/tinyusb/src/tusb.c \
	$(TOP)/drivers/tinyusb/src/common/tusb_fifo.c \
	$(TOP)/drivers/tinyusb/src/device/usbd.c \
	$(TOP)/drivers/tinyusb/src/device/usbd_control.c \
	$(TOP)/drivers/tinyusb/src/class/audio/audio_device.c \
	$(TOP)/drivers/tinyusb/src/class/cdc/cdc_device.c \
	$(TOP)/drivers/tinyusb/src/class/dfu/dfu_device.c \
	$(TOP)/drivers/tinyusb/src/class/dfu/dfu_rt_device.c \
	$(TOP)/drivers/tinyusb/src/class/hid/hid_device.c \
	$(TOP)/drivers/tinyusb/src/class/midi/midi_device.c \
	$(TOP)/drivers/tinyusb/src/class/msc/msc_device.c \
	$(TOP)/drivers/tinyusb/src/class/net/ecm_rndis_device.c \
	$(TOP)/drivers/tinyusb/src/class/net/ncm_device.c \
	$(TOP)/drivers/tinyusb/src/class/usbtmc/usbtmc_device.c \
	$(TOP)/drivers/tinyusb/src/class/video/video_device.c \
	$(TOP)/drivers/tinyusb/src/class/vendor/vendor_device.c \
  $(TOP)/drivers/tinyusb/src/portable/microchip/samd/dcd_samd.c \
  $(TOP)/peripherals/usb.c \

endif

DEFINES += \
  -DDONT_USE_CMSIS_INIT
