# Keep this first line.
GOSSAMER_PATH = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=feather_m0

# Leave this line here.
include $(GOSSAMER_PATH)/make.mk

SRCS += \
  ./app.c \
  $(GOSSAMER_PATH)/drivers/gfx/gfx.c \
  $(GOSSAMER_PATH)/drivers/gfx/sh1107.c \

# Finally, leave this line at the bottom of the file.
include $(GOSSAMER_PATH)/rules.mk
