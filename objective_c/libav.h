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
	AVPacket packet;
	
	NSString *lavFileName;
	
	int avStream;
	int streamType;
		
	int16_t		*aBuffer;
}

- (id)initWithFile:(NSString *)filename streamType:(int)streamType stream:(int)streamNumber;
- (int)openInputFile:(NSString *)filename;
- (int)findStream:(int)streamNumber streamType:(int)streamType;
- (int)findStreamInfo;
- (int)openAVCodec;

- (void)dumpFormat;
// - (int)readFrame;
// - (void)decodeFrame;
- (NSString *)getFilename;

@end
