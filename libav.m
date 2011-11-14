//
//  libav.m
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011 . All rights reserved.
//

#import "libav.h"


@implementation libav

- (id)initWithFile:(char *)filename:(int)streamType:(int)streamNumber {
	
	self = [super init];
	
	CFIndex i;
	avStream = -1;
	
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
		
		
		for(i=0; i<pFormatCtx->nb_streams; i++)
			if(pFormatCtx->streams[i]->codec->codec_type==streamType)
			{
				// DEBUG: print out codec
				//			fprintf (stderr,"Video Codec ID: %d (%s)\n",pFormatCtx->streams[i]->codec->codec_id ,pFormatCtx->streams[i]->codec->codec_name);
				if (avStream == -1 && st == 0) {
					// May still be overridden by the -s option
					avStream=i;
				}
				if (streamNumber == i) {
					avStream=i;
					break;
				}
			}
		
		if(avStream==-1) {
			NSLog(@"initWithFile: could not find an AV stream");
			[self release];
			return nil; // Didn't find the requested stream
		}
		
	pCodecCtx = pFormatCtx->streams[avStream]->codec
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
			
		
		return self;
		
	}
	
	- (void)dumpFormat {	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		dump_format(pFormatCtx, 0, NULL, 0);
#else
		av_dump_format(pFormatCtx, 0, NULL, 0);
#endif
	}
	
	@end
