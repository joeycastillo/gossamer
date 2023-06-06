# Keep this first line.
TOP = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=feather_m0

# for USB example, use two CDC interfaces (we'll echo uppercase to one and lowercase to the other)
TINYUSB_CDC=2

# Leave this line here.
include $(TOP)/make.mk

SRCS += \
  ./app.c \
  ./usb_descriptors.c \

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
