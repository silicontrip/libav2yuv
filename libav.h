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
	
	int frameCounter;
	
}

- (void)initWithFile:(char *)filename;
- (void)dumpFormat;
- (void)readFrame;
- (int)getFrameRateNum;
- (int)getFrameRateDen;
- (int)getSampleAspectNum;
- (int)getSampleAspectDen;
- (int)getChromaSampling;
- (int)getHeight;
- (int)getWidth;



@end
