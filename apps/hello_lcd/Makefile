# Keep this first line.
TOP = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=feather_d11_soic

# Leave this line here.
include $(TOP)/make.mk

# Add all your source files to compile after the app.c line.
SRCS += \
  ./app.c \
  ../clock/drivers/oso_lcd.c \

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
