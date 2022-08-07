TARGET_LIB = libpsplib.a
SRC = source
OBJS = $(SRC)/lodepng.o \
	   $(SRC)/pspl_gfx.o \
	   $(SRC)/pspl_gfx_drawing.o \
	   $(SRC)/pspl_gfx_texture.o \
	   $(SRC)/pspl_input.o \
	   $(SRC)/pspl_debug.o \

INCDIR = 
CFLAGS = -O2 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak