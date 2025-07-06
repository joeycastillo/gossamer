##############################################################################
BUILD = ./build
BIN = firmware

# Only include board.mk if BOARD is set and not cleaning
ifeq (,$(filter clean,$(MAKECMDGOALS)))
  ifdef BOARD
    include $(GOSSAMER_PATH)/boards/$(BOARD)/board.mk
  endif
endif

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

INCLUDES += \
  -I$(GOSSAMER_PATH)/common/ \

ifeq ($(DETECTED_OS), LINUX)
  MAKEFLAGS += -j $(shell nproc)
endif
ifeq ($(DETECTED_OS), OSX)
  NPROCS = $(shell sysctl hw.ncpu  | grep -o '[0-9]\+')
  MAKEFLAGS += -j $(NPROCS)
endif
ifeq ($(DETECTED_OS), WINDOWS)
  MAKEFLAGS += -j $(NUMBER_OF_PROCESSORS)
endif

ifndef EMSCRIPTEN
# Settings and flags for building on hardware

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
UF2 = python3 $(GOSSAMER_PATH)/utils/uf2/uf2conv.py
DFU_CONV = $(GOSSAMER_PATH)/utils/dfu/dx1elf2dfu
DFU_UTIL = dfu-util

CFLAGS += -W -Wall -Wextra -Wmissing-prototypes -Wmissing-declarations
CFLAGS += --std=c99 -Os
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -mcpu=cortex-m0plus -mthumb
CFLAGS += -MD -MP -MT $(BUILD)/$(*F).o -MF $(BUILD)/$(@F).d

LDFLAGS += -mcpu=cortex-m0plus -mthumb
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=$(GOSSAMER_PATH)/chips/$(CHIP)/linker/$(LDSCRIPT).ld
LDFLAGS += -specs=nosys.specs

LIBS += -lm

INCLUDES += \
  -I$(GOSSAMER_PATH)/peripherals/ \
  -I$(GOSSAMER_PATH)/chips/$(CHIP)/include/ \

SRCS += \
  $(GOSSAMER_PATH)/chips/$(CHIP)/startup_$(CHIP).c \
  $(GOSSAMER_PATH)/chips/$(CHIP)/system_$(CHIP).c \
  $(GOSSAMER_PATH)/common/delay.c \
  $(GOSSAMER_PATH)/peripherals/adc.c \
  $(GOSSAMER_PATH)/peripherals/dac.c \
  $(GOSSAMER_PATH)/peripherals/dma.c \
  $(GOSSAMER_PATH)/peripherals/eic.c \
  $(GOSSAMER_PATH)/peripherals/evsys.c \
  $(GOSSAMER_PATH)/peripherals/i2c.c \
  $(GOSSAMER_PATH)/peripherals/i2s.c \
  $(GOSSAMER_PATH)/peripherals/opamp.c \
  $(GOSSAMER_PATH)/peripherals/ptc.c \
  $(GOSSAMER_PATH)/peripherals/rtc.c \
  $(GOSSAMER_PATH)/peripherals/sercom.c \
  $(GOSSAMER_PATH)/peripherals/slcd.c \
  $(GOSSAMER_PATH)/peripherals/spi.c \
  $(GOSSAMER_PATH)/peripherals/tc.c \
  $(GOSSAMER_PATH)/peripherals/tcc.c \
  $(GOSSAMER_PATH)/peripherals/uart.c \

else
# Settings and flags for building with Emscripten

BUILD = ./build-sim
CC = emcc

INCLUDES += \
  -I$(GOSSAMER_PATH)/dummy/peripherals/ \
  -I$(GOSSAMER_PATH)/dummy/chips/$(CHIP)/include/ \

SRCS += \
  $(GOSSAMER_PATH)/chips/$(CHIP)/startup_$(CHIP).c \
  $(GOSSAMER_PATH)/dummy/system_$(CHIP).c \
  $(GOSSAMER_PATH)/dummy/emscripten_loop.c \
  $(GOSSAMER_PATH)/dummy/peripherals/adc.c \
  $(GOSSAMER_PATH)/dummy/peripherals/dac.c \
  $(GOSSAMER_PATH)/dummy/peripherals/dma.c \
  $(GOSSAMER_PATH)/dummy/peripherals/eic.c \
  $(GOSSAMER_PATH)/dummy/peripherals/evsys.c \
  $(GOSSAMER_PATH)/dummy/peripherals/i2c.c \
  $(GOSSAMER_PATH)/dummy/peripherals/i2s.c \
  $(GOSSAMER_PATH)/dummy/peripherals/opamp.c \
  $(GOSSAMER_PATH)/dummy/peripherals/ptc.c \
  $(GOSSAMER_PATH)/dummy/peripherals/rtc.c \
  $(GOSSAMER_PATH)/dummy/peripherals/sercom.c \
  $(GOSSAMER_PATH)/dummy/peripherals/slcd.c \
  $(GOSSAMER_PATH)/dummy/peripherals/spi.c \
  $(GOSSAMER_PATH)/dummy/peripherals/tc.c \
  $(GOSSAMER_PATH)/dummy/peripherals/tcc.c \
  $(GOSSAMER_PATH)/dummy/peripherals/uart.c \
  $(GOSSAMER_PATH)/dummy/peripherals/usb.c \

endif

INCLUDES += \
  -I$(GOSSAMER_PATH)/drivers/ \
  -I$(GOSSAMER_PATH)/boards/$(BOARD)/ \

SRCS += \
  $(GOSSAMER_PATH)/main.c \

ifndef EMSCRIPTEN
# Do not compile tinyusb for Emscripten

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
  -I$(GOSSAMER_PATH)/drivers/tinyusb/src/ \

SRCS += \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/tusb.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/common/tusb_fifo.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/device/usbd.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/device/usbd_control.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/audio/audio_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/cdc/cdc_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/dfu/dfu_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/dfu/dfu_rt_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/hid/hid_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/midi/midi_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/msc/msc_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/net/ecm_rndis_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/net/ncm_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/usbtmc/usbtmc_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/video/video_device.c \
	$(GOSSAMER_PATH)/drivers/tinyusb/src/class/vendor/vendor_device.c \
  $(GOSSAMER_PATH)/drivers/tinyusb/src/portable/microchip/samd/dcd_samd.c \
  $(GOSSAMER_PATH)/peripherals/usb.c \

endif

endif # End of tinyusb exclusion

DEFINES += \
  -DDONT_USE_CMSIS_INIT

# TIMESET = X
# if TIMESET is set to one of the following options, it will generate definitions that capture the build date and time:
#  year = BUILD_YEAR is defined
#  day = BUILD_YEAR, BUILD_MONTH and BUILD_DAY are defined
#  minute = BUILD_YEAR, BUILD_MONTH, BUILD_DAY, BUILD_HOUR and BUILD_MINUTE are defined
ifdef TIMESET
ifeq ($(DETECTED_OS), WINDOWS)
CURRENT_YEAR := $(shell powershell -Command "[System.DateTime]::Now.Year - 2020")
CURRENT_MONTH := $(shell powershell -Command "[System.DateTime]::Now.Month")
CURRENT_DAY := $(shell powershell -Command "[System.DateTime]::Now.Day")
CURRENT_HOUR := $(shell powershell -Command "[System.DateTime]::Now.Hour")
CURRENT_MINUTE := $(shell powershell -Command "[System.DateTime]::Now.Minute")
else
# Unix/Linux/macOS
CURRENT_YEAR := $(shell echo $$(($(shell date +"%Y") - 2020)))
CURRENT_MONTH := $(shell date +"%-m")
CURRENT_DAY := $(shell date +"%-d")
CURRENT_HOUR := $(shell date +"%-H")
CURRENT_MINUTE := $(shell date +"%-M")
endif
ifeq ($(TIMESET), year)
CFLAGS += -DBUILD_YEAR=$(CURRENT_YEAR)
else ifeq ($(TIMESET), day)
CFLAGS += -DBUILD_YEAR=$(CURRENT_YEAR)
CFLAGS += -DBUILD_MONTH=$(CURRENT_MONTH)
CFLAGS += -DBUILD_DAY=$(CURRENT_DAY)
else ifeq ($(TIMESET), minute)
CFLAGS += -DBUILD_YEAR=$(CURRENT_YEAR)
CFLAGS += -DBUILD_MONTH=$(CURRENT_MONTH)
CFLAGS += -DBUILD_DAY=$(CURRENT_DAY)
CFLAGS += -DBUILD_HOUR=$(CURRENT_HOUR)
CFLAGS += -DBUILD_MINUTE=$(CURRENT_MINUTE)
else
$(error TIMESET must be year, day, or minute if used.)
endif
endif

GIT_HASH := $(shell git rev-parse --short HEAD | cut -c1-6 || echo 0)
ifdef GIT_HASH
CFLAGS += -DBUILD_GIT_HASH=\"$(GIT_HASH)\"
else
CFLAGS += -DBUILD_GIT_HASH=\"noHash\"
endif
