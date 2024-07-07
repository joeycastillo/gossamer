# Keep this first line.
TOP = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=straightedge_l22

# Leave this line here.
include $(TOP)/make.mk

SRCS += \
  ./app.c \
  ./qtouch/touch.c \
  ./qtouch/hal_atomic.c \
  ./qtouch/utils_list.c \

LIBS += ./qtouch/lib/gcc/libqtm_acq_saml22_0x0028.a \
        ./qtouch/lib/gcc/libqtm_touch_key_cm0p_0x0002.a

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
