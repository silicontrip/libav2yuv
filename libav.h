//
//  libav.h
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>


@interface libav : NSObject {

	AVFormatContext *pFormatCtx;
	AVInputFormat *avif;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVPacket packet;
	
	int avStream;
	int frameCounter;
	int streamType;
	int frameIn;
	int frameOut;
	
	AVRational frameRate;
	AVRational sampleAspect;
	
}

- (id)initWithFile:(char *)filename:(int)streamType:(int)streamNumber;
- (id)initVideoWithFile:(char *)filename;
- (id)initAudioWithFile:(char *)filename;
- (void)dumpFormat;
- (void)readFrame;
- (void)decodeFrame;
- (int)getFrameRateNum;
- (int)getFrameRateDen;
- (int)getSampleAspectNum;
- (int)getSampleAspectDen;
- (int)getChromaSampling;
- (int)getHeight;
- (int)getWidth;
- (int)getFrameCounter;
- (void)setIn:(int)fin;
- (void)setOut:(int)fout;




@end
