#include "LibavWaveWriter.h"


LibavWaveWriter::LibavWaveWriter() throw (AVException*)
{
	av_register_all();
	oc = avformat_alloc_context();
	libavfmt = av_guess_format("wav", NULL, NULL);

	if (libavfmt == NULL) {
		throw new AVException("Internal codec name not supported",FILE_ERROR);
	//	std::cerr << "internal codec name not supported\n";
		;
	}

	oc->oformat = libavfmt;

	AVCodec *codec = avcodec_find_encoder(libavfmt->audio_codec);
	if (!codec) {
		throw new AVException("Codec not supported",FILE_ERROR);

//		std::cerr << "codec not found\n";
	}
	
	audio_st = avformat_new_stream(oc, codec);

	if (!audio_st) {
		throw new AVException("Could not alloc stream",FILE_ERROR);

	//	std::cerr << "Could not alloc stream\n";
	}
	
	audio_st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
}

LibavWaveWriter::~LibavWaveWriter() 
{
	avcodec_close(audio_st->codec);
	int i;
	for (i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }
	avio_close(oc->pb);
    av_free(oc);
}	

int LibavWaveWriter::setOutputFilename(std::string filename) throw (AVException*)
{

	snprintf(oc->filename, sizeof(oc->filename), "%s", filename.c_str());

	if (avio_open(&oc->pb, filename.c_str(), AVIO_FLAG_WRITE) < 0) {
		throw new AVException("Could not open " + filename,IO_ERROR);
		// std::cerr << "Could not open "<<  filename << "\n";
		// return -1;
	}
	
	return 0;

}


int LibavWaveWriter::writeHeader(void) throw (AVException *)
{
	
	AVCodecContext *c = audio_st->codec;
	
	//	NSLog(@"avcodec_open2");
	if (avcodec_open2(c, NULL, NULL) < 0) {
		throw new AVException("Codec not supported",FILE_ERROR);
		std::cerr << "could not open codec\n";
		return -1;
	}
	
	/* Write the stream header, if any. */
	
	return   avformat_write_header(oc, NULL);
	
	
}

void LibavWaveWriter::setSampleFormat(AVSampleFormat fmt)
{ 
	
	// std::cerr <<"Setting format to: "<<av_get_sample_fmt_name(fmt) << "\n";
	
	audio_st->codec->sample_fmt = fmt; 
}

void LibavWaveWriter::setSampleChannels(int ch)
{ 
	// std::cerr<<"Channels: "<<ch<<"\n";
	
	audio_st->codec->channels = ch; 
}


void LibavWaveWriter::setSamplesPerSecond(int sps) { 
	
//	std::cerr << "Sample Rate: " << sps;
	
	audio_st->codec->sample_rate = sps; 
}

int LibavWaveWriter::writeFrameData(AVFrame *pFrame) throw (AVException*)
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
    if (!got_packet) {
		throw new AVException ("Unable to get packet",IO_ERROR);
      //  return -1;
	}
		
	pkt.stream_index = audio_st->index;
	//	NSLog(@"av_interleaved_write_frame");
	
    if ((status=av_interleaved_write_frame(oc, &pkt)) != 0) {
		throw new AVException ("Error while writing audio frame",IO_ERROR);

	//	std::cerr<<"Error while writing audio frame\n";
	//	return status;
    }
	//	NSLog(@"avcodec free frame");
	
    avcodec_free_frame(&frame);
	
}

