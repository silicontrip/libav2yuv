
#import "libavWaveWriter.h"

@implementation libavWaveWriter

- (id)init
{
	
//	NSLog(@"av_register_all");
	
    av_register_all();
//	NSLog(@"avformat_alloc_context");

    oc = avformat_alloc_context();
	
//	NSLog(@"av_guess_format");
	libavfmt = av_guess_format("wav", NULL, NULL);

	if (libavfmt == NULL) {
		NSLog(@"internal codec name not supported");
		return nil;
	}
	
	oc->oformat = libavfmt;

	
	//audio_st = add_audio_stream(oc, libavfmt->audio_codec);
	
//	NSLog(@"avcodec_find_encoder");

	AVCodec *codec = avcodec_find_encoder(libavfmt->audio_codec);
	if (!codec) {
        NSLog(@"codec not found\n");
		return nil;
    }
// NSLog(@"avformat_new_stream");

	audio_st = avformat_new_stream(oc, codec);
	
	if (!audio_st) {
        NSLog(@"Could not alloc stream\n");
		return nil;
    }
	
	audio_st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
	
	return self;

}

- (id)initWithFormat:(int)fmt channels:(int)ch samplesPerSecond:(int)sps
{

	if ([self init] != nil) 
	{
		[self setSamplesPerSecond:sps];
		[self setSampleChannels:ch];
		[self setSampleFormat:fmt];
		
		return self;
	}
	return nil;
}

- (int)setOutputFilename:(NSString *)filename
{

	//NSLog(@"write to file: %@",filename);
	
	snprintf(oc->filename, sizeof(oc->filename), "%s", [filename UTF8String]);

	if (avio_open(&oc->pb, [filename UTF8String], AVIO_FLAG_WRITE) < 0) {
		NSLog(@"Could not open '%s'\n", filename);
		return -1;
	}
	
}


- (int)writeHeader {
	
	AVCodecContext *c = audio_st->codec;
	
	//	NSLog(@"avcodec_open2");
	if (avcodec_open2(c, NULL, NULL) < 0) {
		NSLog(@"could not open codec");
		return -1;
	}
	
	/* Write the stream header, if any. */

	return   avformat_write_header(oc, NULL);
}
	
- (void)setSamplesPerSecond:(int)sps { 

	NSLog(@"Sample Rate: %d",sps);
	
	audio_st->codec->sample_rate = sps; 
}
- (void)setSampleChannels:(int)ch { 
	NSLog(@"Channels: %d",ch);

	audio_st->codec->channels = ch; 
}
- (void)setSampleFormat:(int)fmt { 

	NSLog(@"Setting format to: %s",av_get_sample_fmt_name(fmt));
	
	audio_st->codec->sample_fmt = fmt; 
}

- (int)writeFrameData:(AVFrame *)pFrame
{
	AVCodecContext *c = audio_st->codec;
	AVPacket pkt = { 0 }; // data and size must be 0;
    int got_packet;
    AVFrame *frame = avcodec_alloc_frame();
	int status;
	
	frame->nb_samples = pFrame->nb_samples;
	frame->data[0] = pFrame->data[0];
//	NSLog(@"write samples: %d",pFrame->nb_samples);
	
	/*
	avcodec_fill_audio_frame(frame, c->channels, c->sample_fmt,
                             (uint8_t *)pFrame->data[0],
                             pFrame->nb_samples *
                             av_get_bytes_per_sample(c->sample_fmt) *
                             c->channels, 1);
	*/
//	NSLog(@"encode audio 2");

	avcodec_encode_audio2(c, &pkt, frame, &got_packet);
    if (!got_packet)
        return -1;

	pkt.stream_index = audio_st->index;
//	NSLog(@"av_interleaved_write_frame");

    if ((status=av_interleaved_write_frame(oc, &pkt)) != 0) {
        NSLog(@"Error while writing audio frame\n");
		return status;
    }
//	NSLog(@"avcodec free frame");

    avcodec_free_frame(&frame);
	
}

- (void)dealloc 
{

	avcodec_close(audio_st->codec);
	int i;
	for (i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }
	avio_close(oc->pb);
    av_free(oc);
	
	[super dealloc];
}

@end
