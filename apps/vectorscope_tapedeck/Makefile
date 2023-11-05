# Keep this first line.
TOP = ../..

# If your firmware targets a specific board, specify it here,
# or omit it and provide it on the command line (make BOARD=foo).
BOARD=vectorscope_touch

# Leave this line here.
include $(TOP)/make.mk

SRCS += \
  ./app.c \
  ./qtouch/touch.c \
  ./qtouch/hal_atomic.c \
  ./qtouch/utils_list.c \

LIBS += ./qtouch/lib/gcc/libqtm_acq_saml21_0x0026.a \
        ./qtouch/lib/gcc/libqtm_surface_cs_cm0p_0x0021.a \
        ./qtouch/lib/gcc/libqtm_touch_key_cm0p_0x0002.a

# Finally, leave this line at the bottom of the file.
include $(TOP)/rules.mk
