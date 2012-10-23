
OPTFLAGS=-g
CFLAGS=$(OPTFLAGS) -I/usr/local/include -I/usr/local/include/mjpegtools
CC?=gcc
COCOAFLAGS=-framework QuartzCore -framework Foundation -framework AppKit
LDFLAGS=-L/usr/X11/lib  -L/usr/local/lib 
MJPEGFLAGS= -lmjpegutils
FFMPEG_FLAGS= -lswscale -lavcodec -lavformat -lavutil



all: test1

libav.o: libav.m libav.h AVObject.o

AVObject.o: AVObject.m AVObject.h

libyuv.o: libyuv.m libyuv.h

test1: test1.o libav.o libyuv.o 
	$(CC) $(COCOAFLAGS) $(FFMPEG_FLAGS) $(MJPEGFLAGS) $(LDFLAGS) -o test1 $< libav.o AVObject.o libyuv.o
