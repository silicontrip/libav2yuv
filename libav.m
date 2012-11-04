//
//  libav.m
//  
//
//  Created by Mark Heath on 15/11/11.
//  Copyright 2011. All rights reserved.
//

#import "libav.h"

#define BUFFER_SIZE 16384

@implementation libav


- (id)initWithFile:(NSString *)filename streamType:(int)st stream:(int)streamNumber {
	
	if ([self init] != nil ) {
	
		[self setIn:-1];
		[self setOut:-1];

		
	CFIndex i;
	avStream = -1;
	streamType = st;
	
	lavFileName = filename;
	
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
		// samples per second ???
		
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
	
	int bytesPerSample = ([self getSampleSize] * [self getSampleChannels]);			

	
	//NSLog(@"> [libav decodeNextAudio]");
	
	// there must be another way to initialise the audio decoder.
	// than this.
	if (sampleCounter == 0)
	{
		// NSLog(@"> [libav decodeNextAudio] allocating audio buffer");
		 pictureBuffer = av_malloc(BUFFER_SIZE );
		
		if (!pictureBuffer) 
		{
			NSLog(@"Unable to allocate buffer");
			// [self release];
			return -1;
		}
		pFrame=avcodec_alloc_frame();
		avcodec_get_frame_defaults(pFrame);
		
	}
		
	// we don't know SPS until we decode the first frame. 
	// we cannot calculate SamplesOut without SPS.
	// what can we do here.  Assume that once the sampleCounter is above 0 the SPS will be set.
	
//	NSLog (@"sampleCounter: %d  samplesOut: %d", sampleCounter, [self getSamplesOut]);
	
	if (sampleCounter > 0) 
		if ([self compareSamplesRange:sampleCounter] > 0)
			return -1;
		
		// loop until we are passed the frame in marker
		do {
		//	NSLog(@"[libav decodeNextAudio] before IN loop");

#if LIBAVFORMAT_VERSION_MAJOR  < 54
			;
#else
			AVFrame *iFrame;
#endif
			int gotFrame = 1;

			// loop until "gotFrame"
			do {
				//  NSLog(@"[libav decodeNextAudio] gotFrame");

				// Find our specified stream
				do {
					// NSLog(@"[libav decodeNextAudio] != avStream: %d",avStream);

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
				iFrame=avcodec_alloc_frame();
				avcodec_get_frame_defaults(iFrame);

				len = avcodec_decode_audio4(pCodecCtx, iFrame, &gotFrame, &packet);
	//			NSLog (@"avcodec_decode_audio4 len: %d gotFrame: %d",len,gotFrame);
				
					if (gotFrame) {
				/* if a frame has been decoded, output it */
						bytes = av_samples_get_buffer_size(NULL, pCodecCtx->channels,
															  iFrame->nb_samples,
															  pCodecCtx->sample_fmt, 1);
						
				//		NSLog (@"decode Audio sps: %d",iFrame->sample_rate);
				//		NSLog (@"audio samples decoded: %d",iFrame->nb_samples);
						
						[self setSamplesPerSecond:iFrame->sample_rate];
					}
				
#endif
				/*
				if (len < 0)
					return len;
				 */
				//	NSLog(@"decoded: %d finished: %d",len,gotFrame);
				
			} while (!gotFrame);
			
			int bytesPerSample = ([self getSampleSize] * [self getSampleChannels]);			
			numSamples = bytes / bytesPerSample;
			int endCounter = sampleCounter + numSamples;
			
		//	NSLog(@"bytesPerSample: %d, numSamples: %d",bytesPerSample,numSamples);
			
			if (([self compareSamplesRange:sampleCounter]<0) && ([self compareSamplesRange:endCounter] >1))
			{
				// send partial frame
		//		NSLog(@"partial case 1");

				pFrame->nb_samples =([self getSamplesOut] - [self getSamplesIn]) ;
				avcodec_fill_audio_frame(pFrame, [self getSampleChannels], [self getSampleFormat],
										 iFrame->data[0] + ([self getSamplesIn] - sampleCounter) * bytesPerSample,
										 pFrame->nb_samples* bytesPerSample , 1);
			}
			else if (([self compareSamplesRange:sampleCounter]<0) && ([self compareSamplesRange:endCounter] ==0))
			{
			// send partial frame
// NSLog(@"partial case 2");

				pFrame->nb_samples =( sampleCounter + numSamples - [self getSamplesIn]) ;
					   avcodec_fill_audio_frame(pFrame, [self getSampleChannels], [self getSampleFormat],

					   iFrame->data[0] + ([self getSamplesIn] - sampleCounter) * bytesPerSample,
					   pFrame->nb_samples* bytesPerSample,1);
				
			} 
			else if (([self compareSamplesRange:sampleCounter]==0) && ([self compareSamplesRange:endCounter] >0))
			{
			// send partial frame
			//	NSLog(@"partial case 3");

				pFrame->nb_samples =( [self getSamplesOut] - sampleCounter);
					   avcodec_fill_audio_frame(pFrame, [self getSampleChannels], [self getSampleFormat],

					   iFrame->data[0],
					   pFrame->nb_samples * bytesPerSample,1);
				
			}
			else 
			{
			// send entire frame.
			//	NSLog(@"full case: %d",numSamples);

			//	NSLog(@"pic buffer: %x pframe->data[0]: %x",pictureBuffer,pFrame->data[0]);
				
				pFrame->nb_samples =numSamples;

				avcodec_fill_audio_frame(pFrame, [self getSampleChannels], [self getSampleFormat],
										 iFrame->data[0],bytes,1);
	//			memset(pFrame->data[0],127,bytes);
			}
			
			sampleCounter += pFrame->nb_samples;
			
		} while ([self compareSamplesRange:sampleCounter] < 0);			
//	NSLog(@"< [libav decodeNextAudio]");

	return bytes;
}

-(int)decodeNextFrame
{
	
	int bytes;
	int frameFinished;
	
	if ([self compareRange:frameCounter] > 0)
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
		
	} while ([self compareRange:frameCounter] < 0);
	
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
