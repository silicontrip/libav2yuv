
OPTFLAGS=-g
CFLAGS=$(OPTFLAGS) -I/usr/local/include -I/usr/local/include/mjpegtools
CC?=gcc
COCOAFLAGS=-framework QuartzCore -framework Foundation -framework AppKit
LDFLAGS=-L/usr/X11/lib  -L/usr/local/lib 
MJPEGFLAGS= -lmjpegutils
FFMPEG_FLAGS= -lswscale -lavcodec -lavformat -lavutil


OBJ=libav.o AVObject.o libyuv.o libav2yuvArguments.o chromaFilter.o test1.o edlListFilter.o

all: test1

libav.o: libav.m libav.h 

AVObject.o: AVObject.m AVObject.h

libyuv.o: libyuv.m libyuv.h

libav2yuvArguments.o: libav2yuvArguments.m libav2yuvArguments.h

chromaFilter.o: chromaFilter.m chromaFilter.h

edlListFilter.o: edlListFilter.m edlListFilter.h

libav2yuv: $(OBJ)
	$(CC) $(COCOAFLAGS) $(FFMPEG_FLAGS) $(MJPEGFLAGS) $(LDFLAGS) -o libav2yuv $(OBJ)

clean:
	rm $(OBJ) 
