//
//  libav.m
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011. All rights reserved.
//

#import "libav.h"


@implementation libav

- (id)initVideoWithFile:(char *)filename {
	return [self initWithFile:filename:AVMEDIA_TYPE_VIDEO:0];
}

- (id)initAudioWithFile:(char *)filename {
	return [self initWithFile:filename:AVMEDIA_TYPE_AUDIO:0];
}

- (id)initWithFile:(char *)filename:(int)st:(int)streamNumber {
	
	self = [super init];
	
	CFIndex i;
	avStream = -1;
	
	streamType = st;
	
	if (self) {
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		if(av_open_input_file(&pFormatCtx, filename, avif, 0, NULL)!=0) 
#else
			if(avformat_open_input(&pFormatCtx, filename, avif, NULL)!=0) 
#endif
			{
				[self release];
				return nil;
			}
		
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		if(av_find_stream_info(pFormatCtx)<0) 
#else
			if(avformat_find_stream_info(pFormatCtx,NULL)<0) 
#endif
			{
				[self release];
				return nil; // Couldn't find stream information
			}
		
		
		for(i=0; i<pFormatCtx->nb_streams; i++) {
			if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
				frameRate.num = pFormatCtx->streams[i]->r_frame_rate.num;
				frameRate.den = pFormatCtx->streams[i]->r_frame_rate.den;
			}
			if(pFormatCtx->streams[i]->codec->codec_type==streamType)
			{
				// DEBUG: print out codec
				if (avStream == -1 && streamNumber == 0) {
					// May still be overridden by the -s option
					avStream=i;
				}
				if (streamNumber == i) {
					avStream=i;
					break;
				}
			}
		}
		if(avStream==-1) {
			NSLog(@"initWithFile: could not find an AV stream");
			[self release];
			return nil; // Didn't find the requested stream
		}
		
		pCodecCtx = pFormatCtx->streams[avStream]->codec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		
		if(pCodec==NULL) {
			NSLog(@"initWithFile: could not find codec");
			[self release];
			return nil; // Codec not found
		}
		
#if LIBAVCODEC_VERSION_MAJOR < 53 
		if(avcodec_open(pCodecCtx,pCodec)<0) 
#else
			if(avcodec_open2(pCodecCtx, pCodec,NULL)<0) 
#endif
			{
				NSLog(@"initWithFile: could not open codec");
				[self release];
				return nil; // Could not open codec
			}
		
		pFrame=avcodec_alloc_frame();
		if (pFrame == NULL) {
			NSLog(@"initWithFile: avmalloc pFrame");
			[self release];
			return nil;
		}
		
		return self;
	}
	return nil;
}

- (void)dumpFormat {	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	dump_format(pFormatCtx, 0, NULL, 0);
#else
	av_dump_format(pFormatCtx, 0, NULL, 0);
#endif
}

-(void)readFrame {
	
	av_read_frame(pFormatCtx, &packet);
	if(packet.stream_index==avStream)
	{
		frameCounter ++;
	}
	
}

-(void)decodeFrame {
	int frameFinished;
#if LIBAVCODEC_VERSION_MAJOR < 52
	 avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet->data, packet->size);
#else
	 avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
	
#endif
	if (frameFinished) {
#if LIBAVCODEC_VERSION_MAJOR < 52 
		av_freep(&packet);
#else
		av_free_packet(&packet);
#endif
	}
	
}

- (int)getFrameRateNum {
	return frameRate.num;
}
- (int)getFrameRateDen {
	return frameRate.den;
}

- (int)getSampleAspectNum {
	return pCodecCtx-> sample_aspect_ratio.num;
}
- (int)getSampleAspectDen {
	pCodecCtx-> sample_aspect_ratio.den;
}
- (int)getChromaSampling {
	return pCodecCtx->pix_fmt;
}
- (int)getHeight {
	return pCodecCtx->height;
}
- (int)getWidth {
	return pCodecCtx->width;
}

- (int)getFrameCounter {
	return frameCounter;
}

- (void)setIn:(int)fin {
	// validate
	frameIn = fin;
}

- (void)setOut:(int)fout {
	// validate
	frameOut = fout;
}


@end
