#include "Libav.h"

Libav::Libav(std::string filename, int st, int streamNumber)
{

	pictureBuffer = NULL;
	
	this->setIn(-1);
	this->setOut(-1);
	
	// default sane values
	this->setFrameRateNum(1);
	this->setFrameRateDen(1);
	this->setSampleAspectNum(1);
	this->setSampleAspectDen(1);
	
	pFormatCtx = NULL;
	
	avStream = -1;
	streamType = st;
	
	lavFileName =std::string(filename);
	
	av_register_all();

	if (this->openInputFile(filename) < 0)
	{
		std::cerr<<"initWithFile: avformat_open_input failed to open: "<<filename << "\n";
		// how do I deal with constructor errors.
	}

	if(this->findStreamInfo()<0)
	{
		std::cerr<<"initWithFile: avformat_find_stream_info failed\n";
		// another constructor error.
	}
	if ((avStream = this->findStream(streamNumber,streamType)) == -1) {
		std::cerr<<"initWithFile: couldn't find requested AV stream\n";
		// constructor error blah blah
	}

	// std::cerr << "Chosen stream: " << avStream << "\n";
	
	pCodecCtx = pFormatCtx->streams[avStream]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	
	// open codec context
	if (this->openAVCodec() < 0)
	{
		std::cerr<<"initWithFile: could not find or open codec\n";
	 // Could not open codec
	}
	
	pFrame=avcodec_alloc_frame();
	
//	std::cerr<<"framerate: " << pFormatCtx->streams[avStream]->r_frame_rate.num <<":" <<pFormatCtx->streams[avStream]->r_frame_rate.den << "\n";
//	std::cerr<<"dar: " << pCodecCtx->sample_aspect_ratio.num <<":" <<pCodecCtx->sample_aspect_ratio.den << "\n";

	this->setFrameRate(pFormatCtx->streams[avStream]->r_frame_rate);
	this->setSampleAspect(pCodecCtx->sample_aspect_ratio);
	
//	std::cerr<<"chroma: " << pCodecCtx->pix_fmt << "\n";
	
	this->setChromaSampling(pCodecCtx->pix_fmt);
	this->setHeight(pCodecCtx->height);
	this->setWidth(pCodecCtx->width);
	this->setSampleFormat(pCodecCtx->sample_fmt);
	this->setSampleChannels(pCodecCtx->channels);
}

Libav::~Libav()
{
	//std::cerr << ">> Libav Destructor\n";
	avcodec_free_frame(&pFrame);
	avcodec_close(pCodecCtx);
	if (pFormatCtx)
#if LIBAVFORMAT_VERSION_MAJOR  < 53
		av_close_input_file(pFormatCtx);
#else
		avformat_close_input(&pFormatCtx);
#endif

}

int Libav::findStream(int streamNumber, int st)
{
	
	int i;
	
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		// set the frame rate for audio extraction (to convert Timecode into samples)
		if (pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			this->setFrameRate(pFormatCtx->streams[i]->r_frame_rate);
		}
		if((pFormatCtx->streams[i]->codec->codec_type==st) && (streamNumber == i || streamNumber == -1)) {
			return i;
		}
	}
	return -1;
	
}

int Libav::findStreamInfo(void)
{
	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	return av_find_stream_info(pFormatCtx); 
#else
	return avformat_find_stream_info(pFormatCtx,NULL);
#endif
}

int Libav::openInputFile(std::string filename)
{
	
	pFormatCtx = NULL;
	// this is supposed to be passed in by command line options.
	avif = NULL;
	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	return av_open_input_file(&pFormatCtx, filename.c_str(), avif, 0, NULL);
#else
	return avformat_open_input(&pFormatCtx, filename.c_str(), avif, NULL); 
#endif
}


int Libav::openAVCodec(void)
{
	if(pCodec==NULL)
		return -1;
	
#if LIBAVCODEC_VERSION_MAJOR < 53 
	return avcodec_open(pCodecCtx,pCodec); 
#else
	return avcodec_open2(pCodecCtx, pCodec,NULL);
#endif
}

void Libav::dumpFormat(void) 
{	
#if LIBAVFORMAT_VERSION_MAJOR  < 53
	dump_format(pFormatCtx, 0, this->getFilename().c_str(), 0);
#else
	av_dump_format(pFormatCtx, 0, this->getFilename().c_str(), 0);
#endif
}

int Libav::decodeNextAudio(void)
{
	
	int bytes;
	int numSamples;
	
	int bytesPerSample = (this->getSampleSize() * this->getSampleChannels());			
	
	
	//NSLog(@"> [libav decodeNextAudio] sample counter: %d",sampleCounter);
	// there must be another way to initialise the audio decoder than this sampleCounter test.
	
	if (sampleCounter == 0)
	{
		pFrame=avcodec_alloc_frame();
		avcodec_get_frame_defaults(pFrame);		
	}
	
	// we don't know SPS until we decode the first frame. 
	// we cannot calculate SamplesOut without SPS.
	// what can we do here.  Assume that once the sampleCounter is above 0 the SPS will be set.
	
	//	NSLog (@"sampleCounter: %d  samplesOut: %d", sampleCounter, [self getSamplesOut]);
	
	if (sampleCounter > 0) 
		if (this->compareSamplesRange(sampleCounter) > 0)
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
				if (sampleCounter == 0)
					this->setSamplesPerSecond(iFrame->sample_rate);
			}
			
#endif
			//NSLog(@"decoded: %d finished: %d",len,gotFrame);
			
		} while (!gotFrame);
		
		
		int bytesPerSample = (this->getSampleSize() * this->getSampleChannels());			
		numSamples = bytes / bytesPerSample;
		int endCounter = sampleCounter + numSamples;
		pFrame->nb_samples =numSamples;
		//	NSLog(@"bytesPerSample: %d, numSamples: %d",bytesPerSample,numSamples);
		if (this->compareSamplesRange(endCounter) >=0) {
			if ((this->compareSamplesRange(sampleCounter)<0) && (this->compareSamplesRange(endCounter) >0))
			{
				// send partial frame
				//NSLog(@"partial case 1");
				
				pFrame->nb_samples =(this->getSamplesOut() - this->getSamplesIn()) ;
				avcodec_fill_audio_frame(pFrame, this->getSampleChannels(), this->getSampleFormat(),
										 iFrame->data[0] + (this->getSamplesIn() - sampleCounter) * bytesPerSample,
										 pFrame->nb_samples* bytesPerSample , 1);
			}
			else if ((this->compareSamplesRange(sampleCounter)<0) && (this->compareSamplesRange(endCounter) ==0))
			{
				// send partial frame
				//NSLog(@"partial case 2");
				
				pFrame->nb_samples =( sampleCounter + numSamples - this->getSamplesIn() + 1) ;
				avcodec_fill_audio_frame(pFrame, this->getSampleChannels(), this->getSampleFormat(),
										 iFrame->data[0] + (this->getSamplesIn() - sampleCounter) * bytesPerSample,
										 pFrame->nb_samples* bytesPerSample,1);
				
			} 
			else if ((this->compareSamplesRange(sampleCounter)==0) && (this->compareSamplesRange(endCounter) >0))
			{
				// send partial frame
				//NSLog(@"partial case 3");
				
				pFrame->nb_samples =( this->getSamplesOut() - sampleCounter);
				avcodec_fill_audio_frame(pFrame, this->getSampleChannels(), this->getSampleFormat(),
										 iFrame->data[0],
										 pFrame->nb_samples * bytesPerSample,1);
				
			}
			else 
			{
				// send entire frame.
				//NSLog(@"full case: %d",numSamples);
				
				//	NSLog(@"pic buffer: %x pframe->data[0]: %x",pictureBuffer,pFrame->data[0]);
				
				pFrame->nb_samples =numSamples;
				
				avcodec_fill_audio_frame(pFrame, this->getSampleChannels(), this->getSampleFormat(),
										 iFrame->data[0],bytes,1);
				//			memset(pFrame->data[0],127,bytes);
			}
		}
		
		avcodec_free_frame(&iFrame);
		av_free(iFrame);
		av_free_packet(&packet);
		
		
		sampleCounter += pFrame->nb_samples;
		
		//	NSLog(@"sample counter: %d",sampleCounter);
		
	} while (this->compareSamplesRange(sampleCounter) < 0);			
	//	NSLog(@"< [libav decodeNextAudio] %d %d ",pFrame->nb_samples , bytes) ;
	
	return pFrame->nb_samples;
}

int Libav::decodeNextFrame(void)
{
	
	int bytes;
	int frameFinished;
	
	if (this->compareRange(frameCounter) > 0)
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
		
	} while (this->compareRange(frameCounter) < 0);
	
	this->setInterlaced(pFrame->interlaced_frame);
	this->setInterlaceTopFieldFirst(pFrame->top_field_first);
	
#if LIBAVCODEC_VERSION_MAJOR < 52 
	av_freep(&packet);
#else
	av_free_packet(&packet);
#endif
	return bytes;
}

std::string Libav::getFilename(void) {
	return lavFileName;
}

