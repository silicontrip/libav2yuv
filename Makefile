
OPTFLAGS=-g
CFLAGS=$(OPTFLAGS) -I/usr/local/include
CC?=gcc
COCOAFLAGS=-framework QuartzCore -framework Foundation -framework AppKit
LDFLAGS=-L/usr/X11/lib  -L/usr/local/lib 
MJPEGFLAGS= -lmjpegutils
FFMPEG_FLAGS= -lswscale -lavcodec -lavformat -lavutil



all: test1

libav.o: libav.m libav.h


test1: test1.o libav.o
	$(CC) $(COCOAFLAGS) $(FFMPEG_FLAGS) $(MJPEGFLAGS) $(LDFLAGS) -o test1 $< libav.o
