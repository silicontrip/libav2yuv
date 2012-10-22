//
//  libav.h
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/NSString.h>

#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#import "AVObject.h"

@interface libav : AVObject {

	AVFormatContext *pFormatCtx;
	AVInputFormat *avif;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVPacket packet;
	
	NSString *lavFileName;
	
	int avStream;
	int streamType;
		
}

- (id)initWithFile:(char *)filename:(int)streamType:(int)streamNumber;
- (id)initVideoWithFile:(char *)filename;
- (id)initAudioWithFile:(char *)filename;
- (void)dumpFormat;
- (int)readFrame;
- (void)decodeFrame;
//- (void)decodeNextFrame;
- (NSString *)getFilename;




@end
