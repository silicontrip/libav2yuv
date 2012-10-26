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
	
	pFormatCtx = nil;
	avif = nil;
	CFIndex i;
	avStream = -1;
	streamType = st;
	
	lavFileName = [[NSString alloc] initWithUTF8String:filename];
	
	if (self) {
		av_register_all();
		
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		if(av_open_input_file(&pFormatCtx, filename, avif, 0, NULL)!=0) 
#else
			if(avformat_open_input(&pFormatCtx, filename, avif, NULL)!=0) 
#endif
			{
				NSLog(@"initWithFile: avformat_open_input failed to open: %s",filename);
				[self release];
				return nil;
			}
		
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		if(av_find_stream_info(pFormatCtx)<0) 
#else
			if(avformat_find_stream_info(pFormatCtx,NULL)<0) 
#endif
			{
				NSLog(@"initWithFile: avformat_find_stream_info failed");
				
				[self release];
				return nil; // Couldn't find stream information
			}
		
		
		for(i=0; i<pFormatCtx->nb_streams; i++) {
			if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
				[self setFrameRate:pFormatCtx->streams[i]->r_frame_rate];
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
			NSLog(@"initWithFile: avcodec_alloc_frame pFrame");
			[self release];
			return nil;
		}
		
		[self setSampleAspect:pCodecCtx->sample_aspect_ratio];
		[self setChromaSampling:pCodecCtx->pix_fmt];
		[self setHeight:pCodecCtx->height];
		[self setWidth:pCodecCtx->width];
		
		return self;
	}
	return nil;
}

- (void)dumpFormat {	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	dump_format(pFormatCtx, 0, [[self getFilename] UTF8String], 0);
#else
	av_dump_format(pFormatCtx, 0, [[self getFilename] UTF8String], 0);
#endif
}

-(int)decodeNextFrame
{
	
	int bytes;
	int frameFinished;
	
	if (frameCounter > [self getOut])
		return -1;
	
	do {
		do {
			
			do {
			//	NSLog(@"read frame until match");
				bytes = av_read_frame(pFormatCtx, &packet);
				if (bytes < 0) 
					return bytes;
			
			} while (packet.stream_index != avStream) ;
			
		//	NSLog (@"Decode frame until framefinished");
			
			int len;
			// I'm not sure when avcodec_decode_video2 became available, if your version of libav doesn't
			// recognise avcodec_decode_video2 then change the VERSION MAJOR here
			// and email mjpeg0@silicontrip.org
			
#if LIBAVCODEC_VERSION_MAJOR < 52
			len = avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet->data, packet->size);
#else
			len = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
#endif
			
			if (len < 0)
				return len;
			//	NSLog(@"decoded: %d finished: %d",len,frameFinished);
			
		} while (!frameFinished);
	//	NSLog (@"Skip until frameCounter(%d) >= in(%d) ",frameCounter,[self getIn]);

		frameCounter ++;
		
	} while (frameCounter < [self getIn]);
	
	[self setInterlaced:pFrame->interlaced_frame];
	[self setInterlaceTopFieldFirst:pFrame->top_field_first];
	
#if LIBAVCODEC_VERSION_MAJOR < 52 
	av_freep(&packet);
#else
	av_free_packet(&packet);
#endif
	return bytes;
}




- (NSString *)getFilename {
	return lavFileName;
}

@end
