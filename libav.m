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

// think I might deprecate readFrame and decodeFrame before they even became production ready.
-(int)decodeNextFrameToYUV:(uint8_t **)m {
	
	int bytes;
	int frameFinished;
	
	if (frameCounter > [self getOut])
		return -1;
	
	do {
		// read frame
		do {
			
			bytes = av_read_frame(pFormatCtx, &packet);
			// or end of file
			// or outside of in and out
			
			while (packet.stream_index != avStream && bytes >=0 )
				bytes = av_read_frame(pFormatCtx, &packet);
			
			if (bytes >=0) {				
				int len;
				
#if LIBAVCODEC_VERSION_MAJOR < 52
				len = avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet->data, packet->size);
#else
				len = avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
#endif
				
				//	NSLog(@"decoded: %d finished: %d",len,frameFinished);
			}
		} while (!frameFinished && bytes >=0);
		
		frameCounter ++;
		
	} while (frameCounter < [self getIn]);
	
	if (bytes>=0) {
		[self setInterlaced:pFrame->interlaced_frame];
		[self setInterlaceTopFieldFirst:pFrame->top_field_first];
		
		// copy to yuv frame
		
		int y;
		int h = [self getHeight];
		int w = [self getWidth];
		int ch = [self getChromaHeight];
		int cw = [self getChromaWidth];
		for (y=0; y<h; y++) {
			//		mjpeg_debug ("copy %d bytes to: %x from: %x",w,dst[0]+y*w,(src->data[0])+y*src->linesize[0]);
			//	NSLog(@"memcpy0 %d %d %x %x",w,pFrame->linesize[0],m[0],pFrame->data[0]);
			
			
			
			memcpy(m[0]+y*w,(pFrame->data[0])+y*pFrame->linesize[0],w);
			if (y<ch) {
				
#ifdef DEBUG
				mjpeg_debug("copy %d bytes to: %x from: %x",cw,dst[1]+y*cw,(src->data[1])+y*src->linesize[1]);
#endif
				//	NSLog(@"memcpy1");
				
				memcpy(m[1]+y*cw,(pFrame->data[1])+y*pFrame->linesize[1],cw);
				//	NSLog(@"memcpy2");
				
				memcpy(m[2]+y*cw,(pFrame->data[2])+y*pFrame->linesize[2],cw);
			}
		}
		
		
		if (frameFinished) {
#if LIBAVCODEC_VERSION_MAJOR < 52 
			av_freep(&packet);
#else
			av_free_packet(&packet);
#endif
		}
	}
	return bytes;
}

- (NSString *)getFilename {
	return lavFileName;
}

@end
