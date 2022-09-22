# Keep this first line. It establishes the project root.
TOP = ../..

# Next include the makefile rules for the board you are targeting.
include $(TOP)/boards/feather_m0/board.mk

# Add all your source files to compile after the app.c line.
SRCS += \
  ./app.c \
  ./drivers/oso_lcd.c \

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
