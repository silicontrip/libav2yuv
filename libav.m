//
//  libav.m
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011. All rights reserved.
//

#import "libav.h"

@implementation libav


- (id)initWithFile:(NSString *)filename streamType:(int)st stream:(int)streamNumber {
	
	self = [self init];
	
	CFIndex i;
	avStream = -1;
	streamType = st;
	
	lavFileName = filename;
	
	if (self) {
		av_register_all();
		
		
		//open input file
		if ([self openInputFile:filename] < 0)
		{
			NSLog(@"initWithFile: avformat_open_input failed to open: %@",filename);
			[self release];
			return nil;
		}
		
		if([self findStreamInfo]<0)
		{
			NSLog(@"initWithFile: avformat_find_stream_info failed");
			[self release];
			return nil; // Couldn't find stream information
		}
		
		if ((avStream = [self findStream:streamNumber streamType:streamType]) == -1) {
			NSLog(@"initWithFile: couldn't find requested AV stream");
			[self release];
			return nil;
		}
		pCodecCtx = pFormatCtx->streams[avStream]->codec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		
		// open codec context
		if ([self openAVCodec] < 0)
		{
			NSLog(@"initWithFile: could not find or open codec");
			[self release];
			return nil; // Could not open codec
		}
		
		pFrame=avcodec_alloc_frame();
		
		[self setFrameRate:pFormatCtx->streams[avStream]->r_frame_rate];
		[self setSampleAspect:pCodecCtx->sample_aspect_ratio];
		[self setChromaSampling:pCodecCtx->pix_fmt];
		[self setHeight:pCodecCtx->height];
		[self setWidth:pCodecCtx->width];
		[self setSampleFormat:pCodecCtx->sample_fmt ];
		[self setSampleChannels:pCodecCtx->channels];

		
		return self;
	}
	return nil;
}

- (int)findStream:(int)streamNumber streamType:(int)st
{
	
	int i;
	
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		// set the frame rate for audio extraction (to convert Timecode into samples)
		if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			[self setFrameRate:pFormatCtx->streams[i]->r_frame_rate];
		}
		if((pFormatCtx->streams[i]->codec->codec_type==st) && (streamNumber == i || streamNumber == -1)) {
				return i;
		}
	}
	return -1;
	
}
- (int)findStreamInfo
{
	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	return av_find_stream_info(pFormatCtx); 
#else
	return avformat_find_stream_info(pFormatCtx,NULL);
#endif
}

- (int)openInputFile:(NSString *)filename
{
	
	pFormatCtx = nil;
	avif = nil;
	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	return av_open_input_file(&pFormatCtx, [filename UTF8String], avif, 0, NULL);
#else
	return avformat_open_input(&pFormatCtx, [filename UTF8String], avif, NULL); 
#endif
}

- (int)openAVCodec
{
	if(pCodec==NULL)
		return -1;
	
#if LIBAVCODEC_VERSION_MAJOR < 53 
	return avcodec_open(pCodecCtx,pCodec); 
#else
	return avcodec_open2(pCodecCtx, pCodec,NULL);
#endif
}



- (void)dumpFormat {	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	dump_format(pFormatCtx, 0, [[self getFilename] UTF8String], 0);
#else
	av_dump_format(pFormatCtx, 0, [[self getFilename] UTF8String], 0);
#endif
}

-(int)decodeNextAudio
{
	
	int bytes;
	int numSamples;
	
	if (sampleCounter > [self getSamplesOut])
		return -1;
		
		// loop until we are passed the frame in marker
		do {
			int gotFrame = 1;

			// loop until "gotFrame"
			do {
				// Find our specified stream
				do {
					bytes = av_read_frame(pFormatCtx, &packet);
					if (bytes < 0) 
						return bytes;
				} while (packet.stream_index != avStream) ;
				
				
				int len;
				
#if LIBAVFORMAT_VERSION_MAJOR  < 50
				len = avcodec_decode_audio2(pCodecCtx, aBuffer, &numBytes, packet.data, packet.size);
#elseif LIBAVFORMAT_VERSION_MAJOR  < 54
				len = avcodec_decode_audio3(pCodecCtx, aBuffer, &numBytes, &packet);
#else				
				gotFrame = 0;
				avcodec_get_frame_defaults(pFrame);

				len = avcodec_decode_audio4(pCodecCtx, pFrame, &gotFrame, &packet);
					if (gotFrame) {
				/* if a frame has been decoded, output it */
						bytes = av_samples_get_buffer_size(NULL, pCodecCtx->channels,
															  pFrame->nb_samples,
															  pCodecCtx->sample_fmt, 1);
						[self setSamplesPerSecond:pFrame->sample_rate];
					}
				
#endif
				
				if (len < 0)
					return len;
				//	NSLog(@"decoded: %d finished: %d",len,frameFinished);
				
			} while (!gotFrame);
			
			numSamples = bytes;
			
		} while (sampleCounter < [self getSamplesIn]);			
	return bytes;
}

-(int)decodeNextFrame
{
	
	int bytes;
	int frameFinished;
	
	if (frameCounter > [self getOut])
		return -1;
	
	// loop until we are passed the frame in marker
	do {
		// loop until "frameFinished"
		do {
			// Find our specified stream
			do {
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
