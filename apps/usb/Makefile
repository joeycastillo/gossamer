# Keep this first line.
TOP = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=hallowing_m0
TINYUSB_CDC=2

# Set CFG_TUSB_DEBUG to 1, 2 or 3 to enable debug output.
CFLAGS += -DCFG_TUSB_DEBUG=0 -DCFG_TUSB_DEBUG_PRINTF=debug_printf

# Leave this line here.
include $(TOP)/make.mk

SRCS += \
  ./app.c \
  ./usb_descriptors.c \

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
