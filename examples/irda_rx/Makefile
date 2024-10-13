# Keep this first line.
GOSSAMER_PATH = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=circuitplayground

# enable USB
TINYUSB_CDC=1

# Leave this line here.
include $(GOSSAMER_PATH)/make.mk

INCLUDES += \
  -I../drivers/tinyusb/src \

# Add all your source files to compile after the app.c line.
SRCS += \
  ./app.c \
  ./usb_descriptors.c \
  ./usb_cdc.c \

# Finally, leave this line at the bottom of the file.
include $(GOSSAMER_PATH)/rules.mk
