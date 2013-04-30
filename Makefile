OPTFLAGS=-g
CFLAGS=$(OPTFLAGS) -I/usr/local/include -I/usr/local/include/mjpegtools -I/opt/local/include -D__STDC_CONSTANT_MACROS
CC=g++
LDFLAGS=-L/usr/X11/lib  -L/usr/local/lib 
MJPEGFLAGS= -lmjpegutils
FFMPEG_FLAGS= -lswscale -lavcodec -lavformat -lavutil

OBJ=AVObject.o ChromaFilter.o LibavWaveWriter.o EdlListFilter.o Libav.o Libyuv.o Libav2yuvArguments.o libav2yuv.o DissolveTransition.o AVException.o Transition.o AVYUVAdaptor.o

all: libav2yuv

%.o:%.cpp
	$(CC) $(CFLAGS) -c $^

libav2yuv: $(OBJ) 
	$(CC) $(COCOAFLAGS) $(FFMPEG_FLAGS) $(MJPEGFLAGS) $(LDFLAGS) -o libav2yuv $(OBJ)

clean:
	rm -f $(OBJ) 
