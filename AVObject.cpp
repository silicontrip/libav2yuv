#include "AVObject.h"

AVObject::AVObject()
{
	
	this->setIn(-1);
	this->setOut(-1);
	pictureBuffer = NULL;
	this->setInterlaced(false);
	
	this->setHeight(0);
	this->setWidth(0);
	frameRate.num=0;
	frameRate.den=0;
	sampleAspect.num=0;
	sampleAspect.den=(0);
	
	frameCounter = 0;
	
	this->setChromaSampling(PIX_FMT_NONE);
	
	this->setColour(16,128,128);
	
}

AVObject::AVObject(int samples, int ch, AVSampleFormat sf, int sps) throw (AVException*)
{
	
	// AUDIO
	
	pFrame=avcodec_alloc_frame();
	avcodec_get_frame_defaults(pFrame);
	
	this->setSampleChannels(ch);
	this->setSampleFormat(sf);
	this->setSamplesPerSecond(sps);
	
	bufferSize = samples;
	
	pictureBuffer = (uint8_t *)av_malloc(samples *
										 av_get_bytes_per_sample(sf) *
										 ch); 
	
	if (!pictureBuffer) 
		throw new AVException("Unable to allocate Audio buffer",MEMORY_ALLOCATION_ERROR);
	
	avcodec_fill_audio_frame(pFrame, ch, sf,
							 pictureBuffer,
							 samples * av_get_bytes_per_sample(sf) * ch,
							 1);
	
	memset(	pictureBuffer,0,samples *
		   av_get_bytes_per_sample(sf) *
		   ch);
	
}

AVObject::AVObject(PixelFormat ch, int h, int w) throw (AVException*)
{
	this->setChromaSampling(ch);
	this->setHeight(h);
	this->setWidth(w);
	
	this->setIn(-1);
	this->setOut(-1);
	
	frameCounter = 0;
	
	pictureBuffer = NULL;
	this->allocFrame();
	
}

void AVObject::allocFrame(void) throw (AVException*)
{
	
	pFrame=avcodec_alloc_frame();
	
	if (pFrame)
	{
		if (hasWidth() && hasHeight() && hasChromaSampling()) 
		{
			
			PixelFormat ch = this->getChromaSampling();
			int h = this->getHeight();
			int w = this->getWidth();
			
			// std::cerr << "Alloc: " << w << "x" << h << " pix: " << this->getChromaSampling() << " (" << ch << ")\n";
			
			int size = avpicture_get_size(ch, w, h);
			if (pictureBuffer)
				av_free(pictureBuffer);
			
			pictureBuffer = (uint8_t *)av_malloc(size);
			if (pictureBuffer) {
				avpicture_fill((AVPicture *)pFrame, pictureBuffer, ch, w, h);
			} else {
				av_free(pFrame);
				pFrame = NULL;
				throw new AVException("Unable to allocate picture buffer",MEMORY_ALLOCATION_ERROR);
			}
		} else {
			// std::cerr << "Not enough parameters to allocate picture buffer\n";
		}
	}
}

std::string AVObject::getSamplingAspectAsString(void)
{
	std::stringstream out (std::stringstream::out);
	
	out << sampleAspect.num << ":" << sampleAspect.den;
	
	return out.str();	
	
}


std::string AVObject::getFrameRateAsString(void)
{
	std::stringstream out (std::stringstream::out);
	out << frameRate.num << ":" << frameRate.den;
	return out.str();	
}

bool AVObject::hasFrameRate(void)
{
	return frameRate.num != 0 && frameRate.den != 0;	
}

AVRational AVObject::getFrameRate(void)
{
	return frameRate;
}

int AVObject::getFrameRateNum(void)
{
	return frameRate.num;
}

int AVObject::getFrameRateDen(void)
{
	return frameRate.den;
}

bool AVObject::hasSampleAspect(void)
{
	return sampleAspect.num != 0 && sampleAspect.den != 0;	
}


AVRational AVObject::getSampleAspect(void)
{
	return sampleAspect;
}

int AVObject::getSampleAspectNum(void)
{
	return sampleAspect.num;
}

int AVObject::getSampleAspectDen(void) 
{
	return sampleAspect.den;
}

bool AVObject::hasChromaSampling(void) {
	return frameChromaSampling != PIX_FMT_NONE;
}

const char * AVObject::getChromaSamplingName(void)
{
	return av_get_pix_fmt_name(this->getChromaSampling());
}

PixelFormat AVObject::getChromaSampling(void)
{
	return frameChromaSampling;
}

int AVObject::getInterlace(void)
{
	
	if (getIsInterlaced()) {
		if (getInterlaceTopFieldFirst()) 
		{
			return Y4M_ILACE_TOP_FIRST;
		} 
		return Y4M_ILACE_BOTTOM_FIRST;
	}
	return Y4M_ILACE_NONE;
}

bool AVObject::getIsInterlaced(void)
{
	return isInterlaced;
}

bool AVObject::getInterlaceTopFieldFirst(void)
{
	// this field only makes sense if interlace is set
	if (this->getIsInterlaced()) 
		return interlaceTopFieldFirst;
	return 0;
}


bool AVObject::hasHeight(void)
{
	return frameHeight != 0;
}

int AVObject::getHeight(void)
{
	return frameHeight;
}

bool AVObject::hasWidth(void)
{
	return frameWidth != 0;
}


int AVObject::getWidth(void)
{
	return frameWidth;
}

int AVObject::getChromaHeight(void)
{
	//dependent on chroma
	switch (this->getChromaSampling()) {
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUVJ420P:
		case PIX_FMT_YUVA420P:
		case PIX_FMT_YUV440P:
		case PIX_FMT_YUVJ440P:
		case PIX_FMT_YUV420P16LE:
		case PIX_FMT_YUV420P16BE:
		case PIX_FMT_NV12:
		case PIX_FMT_NV21:
			return frameHeight>>1;
			break;
		case PIX_FMT_YUV410P:
			return frameHeight >>2;
			break;
		default:
			return frameHeight;
			break;
	}
}


int AVObject::getChromaWidth(void)
{
	//dependent on chroma
	switch (this->getChromaSampling()) {
		case PIX_FMT_YUV420P:
		case PIX_FMT_YUYV422:
		case PIX_FMT_YUV422P:
		case PIX_FMT_YUVJ420P:
		case PIX_FMT_YUVJ422P:
		case PIX_FMT_UYVY422:
		case PIX_FMT_YUVA420P:
		case PIX_FMT_YUV420P16LE:
		case PIX_FMT_YUV420P16BE:
		case PIX_FMT_YUV422P16LE:
		case PIX_FMT_YUV422P16BE:
		case PIX_FMT_NV12:
		case PIX_FMT_NV21:
			return frameWidth>>1;
			break;
		case PIX_FMT_YUV410P:
		case PIX_FMT_YUV411P:
		case PIX_FMT_UYYVYY411:
			return frameWidth >>2;
			break;
		default:
			return frameHeight;
			break;
	}
}



int AVObject::getFrameCounter(void)
{
	return frameCounter;
}

int AVObject::getIn(void)
{
	//return getFrameRateNum() * secondsIn.num / getFrameRateDen() + secondsIn.den;
	return frameIn;
}

int AVObject::getOut(void)
{
	return frameOut;

//	return getFrameRateNum() * secondsOut.num / getFrameRateDen() + secondsOut.den;
}

int AVObject::compareRange(int fr)
{
	
	if (!hasIn() && !hasOut()) 
		return 0;
	
	// getting off by one frame errors here.
	// hopefully fixed by trial and error.
	
	if (hasIn() && fr < getIn()) 
		return -1;
	if (hasOut() && fr >= getOut())
		return 1;
	// there should be no other conditions.
	
	//	if (((fr >= frameIn) || (frameIn == -1)) && ((fr <= frameOut) || (frameOut == -1)))
	return 0;
	
}

bool AVObject::hasIn (void)
{
	return frameIn != -1;
	//return secondsIn.den != -1;
}

bool AVObject::hasOut(void)
{
	return frameOut != -1;

//	return secondsOut.den != -1;
}

int AVObject::compareSamplesRange(int fr)
{
	
	if (!hasIn() && !hasOut()) 
		return 0;
	
	double spf = this->getSamplesPerFrame();
	
	int samplesIn= this->getIn() * spf;
	int samplesOut= this->getOut() * spf;
	
	//NSLog(@"Compare samples Range: %d (%d - %d) ",fr,samplesIn,samplesOut);
	
	
	if (fr <  samplesIn && hasIn()) 
		return -1;
	if (fr >= samplesOut && hasOut())
		return 1;
	
	return 0;
	
}

int64_t AVObject::getSamplesIn(void)
{
	return getIn() * this->getSamplesPerFrame();
}

int64_t AVObject::getSamplesOut(void)
{
	return getOut() * this->getSamplesPerFrame();
}

int AVObject::getSampleSize(void)
{ 
	return av_get_bytes_per_sample(sampleFormat); 
}

int AVObject::getSampleChannels(void)
{ 
	return sampleChannels; 
}

AVSampleFormat AVObject::getSampleFormat(void)
{
	return sampleFormat; 
}
int AVObject::getSamplesPerSecond(void)
{
	return samplesPerSecond; 
}

double AVObject::getSamplesPerFrame(void)
{ 
	return 1.0 * samplesPerSecond * this->getFrameRateDen() / this->getFrameRateNum(); 
}


void AVObject::setColour(uint8_t y, uint8_t u, uint8_t v)
{
	colour_y = y;
	colour_u = u;
	colour_v = v;
	
	// create picture buffer here...
}

void AVObject::setIn(int fin)
{
	//secondsIn.num = 0;
	//secondsIn.den = fin;
	frameIn = fin;
}

void AVObject::setOut(int fout)
{
	//secondsOut.num = 0;
	//secondsOut.den = fout;
	
	frameOut = fout;
}

/*
void AVObject::setIn(AVRational fin)
{
	
	//std::cerr<<"set in avrational: " << fin.num <<"." << fin.den << "\n";
	
	secondsIn.num = fin.num;
	secondsIn.den = fin.den;
	// frameIn = fin;
}

void AVObject::setOut(AVRational fout)
{
	
	//	std::cerr<<"set out avrational: " << fout.num <<"." << fout.den << "\n";
	
	secondsOut.num = fout.num;
	secondsOut.den = fout.den;
	
	// frameOut = fout;
}
*/

void AVObject::setSampleChannels(int ch)
{
	sampleChannels = ch;
}

void AVObject::setSampleFormat(AVSampleFormat fmt)
{
	sampleFormat = fmt;
}


void AVObject::setSamplesPerSecond(int sps)
{
	samplesPerSecond = sps;
}

std::string AVObject::getInTimecode(void)
{
	if (this->getIn() >= 0) 
		return this->FramesToTC(this->getIn());
	return "null";
}

std::string AVObject::getOutTimecode(void)
{
	if (this->getOut() >= 0) 
		return this->FramesToTC(this->getOut());
	return "null";
}


std::string AVObject::FramesToTC(int frames)
{
	
	int frn = this->getFrameRateNum();
	int frd = this->getFrameRateDen();
	
	std::stringstream out (std::stringstream::out);
	
	if ( frn % frd )
	{
		// non integer frame rate
		
		// non drop frame rate
		// round up to nearest integer frame rate.
		// that's right time code != actual play time.
		frn += frd - (frn % frd);
		
		// SMPTE algorithm for 29.97
		// 2 skipped frames per minute, excluding the 10 minute divisible ones.
		
		/*
		 int smpted = fc / 17982;
		 int smptem = fc % 17982;
		 frames +=  18*smpted + 2*((smptem - 2) / 1798);
		 */
		
		// not calculating drop frame at this point
	}
	
	//	std::cerr << frd << "*" << frames << "/" << frn << "\n";
	
	int h = frd * frames / frn / 3600;
	int m = (frd * frames / frn / 60) % 60;
	int s = (frd * frames / frn) % 60;
	int f = frames % (frn / frd);
	
	
	out << "" <<  h  << ":" << std::setfill('0') << std::setw(2) << m << ":" << s << ":" << f;
	return out.str();
}


// This method isn't dependent on having a framerate set.
/*
AVRational AVObject::TCtoSecondsFrames(std::string timecode) throw (AVException*)
{
	bool drop = timecode.find(";") != std::string::npos;
	bool dropFrame = true;
	
	int frn = this->getFrameRateNum();
	int frd =  this->getFrameRateDen();
	
	AVRational secframe;
	// std::cerr <<  "drop find: " << drop << "\n";
	
	if (!drop) 
	{
		// need looser comparison
		if ( 1.0 * frn / frd == 30000.0 / 1001.0) {
			frn = 30;
			frd = 1;
		}
		dropFrame = false;
	}
	
	std::string newTimeCode;
	
	if (drop)
		newTimeCode = timecode.replace(drop,1,":");
	else 
		newTimeCode = timecode;
	
	// NSString *newTimecode = [timecode stringByReplacingOccurrencesOfString:@";" withString:@":"];
	// NSArray *digits = [newTimecode componentsSeparatedByString:@":"];
	
	
	// no real simple way of splitting a string
	// without using external libraries or additional code.
	std::vector<int> digits;
	std::stringstream ss(newTimeCode);
	std::string s;
	
	while (getline(ss, s, ':')) {
		std::stringstream digitin(s);
		int digit;
		digitin >> digit; 
		//	std::cerr << "digit: " << s << "\n";
		digits.push_back(digit);
	}
	
	int frames = digits.back();
	
	//	std::cerr << "frames: " << frames << "\n";
	
	//	int frames = [[digits lastObject] intValue];
	int seconds;
	seconds = 0;
	
	if (digits.size() >1) {
		
		int i;
		for (i=0 ; i<digits.size()-1; i++)
		{
			//		std::cerr << "digit at: " << i << " = " << digits.at(i) <<"\n";
			
			seconds = (seconds * 60) + digits.at(i);
		}
	}
	
	secframe.num = seconds;
	secframe.den = frames;
	
	return secframe;
}
*/

struct timecodeStruct AVObject::TCtoStruct(std::string timecode) 
{
	struct timecodeStruct tcs;
	std::string newTimeCode;

	tcs.f = 0;
	tcs.s = 0;
	tcs.m = 0;
	tcs.h = 0;
	
	int drop  = timecode.find(";");
	tcs.df = drop != std::string::npos;
	
	if (drop)
		newTimeCode = timecode.replace(drop,1,":");
	else 
		newTimeCode = timecode;
	
	// no real simple way of splitting a string
	// without using external libraries or additional code.
	std::vector<int> digits;
	std::stringstream ss(newTimeCode);
	std::string s;
	
	while (getline(ss, s, ':')) {
		std::stringstream digitin(s);
		int digit;
		digitin >> digit; 
		//	std::cerr << "digit: " << s << "\n";
		digits.push_back(digit);
	}
	
	int point = digits.size() -1;
	
	tcs.f = digits.at(point--);
	if (point>=0)
		tcs.s = digits.at(point--);
	if (point>=0)
		tcs.m = digits.at(point--);
	if (point>=0)
		tcs.h = digits.at(point--);
	
	return tcs;
	
}

int AVObject::StructToFrames (struct timecodeStruct tc) throw (AVException *)
{

	int num = getFrameRateNum();
	int den = getFrameRateDen();
	int dropFrames = 0;
	
	int sec = tc.h * 3600 + tc.m * 60 + tc.s;
	
	if (num % den)
	{
		num += den - (num % den);

		if (tc.df) 
		{
			if ( fabs(29.97 - (1.0 * num / den )) < 0.001){
				// TODO: this only works for 29.97
				// there is only drop code algorithm for 30000/1001
				
				int totalMinutes = 60 * tc.h + tc.m;
				// 2 skipped frames per minute, excluding the 10 minute divisible ones.
				dropFrames  =  2 * (totalMinutes - totalMinutes / 10);
			} else {
				throw new AVException ("Unknown drop frame framerate: ",TIMECODE_PARSER_ERROR);
			}
			
		}
	}
	return sec * num / den + tc.f + dropFrames;
	
}

int AVObject::TCtoFrames(std::string timecode) throw (AVException*)
{
	return this->StructToFrames (this->TCtoStruct(timecode));
}


void AVObject::setInTimecode(std::string sin) throw (AVException*)
{
	this->setIn(this->TCtoFrames(sin));
}

void AVObject::setOutTimecode(std::string sout) throw (AVException*) 
{
	this->setOut(this->TCtoFrames(sout));
}

void AVObject::setInOutTimecode(std::string tc) throw (AVException*)
{
	
	std::stringstream ss(tc);
	std::string s;
	
	getline(ss, s, '-');
	this->setIn(this->TCtoFrames(s));
	getline(ss, s, '-');
	this->setOut(this->TCtoFrames(s));
	
}

void AVObject::setFrameRate(AVRational rational) throw (AVException*)
{	
	
	try {
		
		this->setFrameRateNum(rational.num);
		this->setFrameRateDen(rational.den);
		
	} catch (AVException *e) {
		std::cerr << e->getMessage() << " " << rational.num << ":" << rational.den << "\n";
	}
}

void AVObject::setFrameRateNum(int num) throw (AVException*)
{
//	if (num <= 0)
//		throw new AVException ("Invalid Frame rate",INVALID_ARGUMENT);
	frameRate.num = num;
}

void AVObject::setFrameRateDen(int den) throw (AVException*)
{
//	if (den <= 0)
//		throw new AVException ("Invalid Frame rate",INVALID_ARGUMENT);
	frameRate.den = den;
}

void AVObject::setSampleAspect(AVRational rational) throw (AVException*)
{
	if (rational.den>0 && rational.num>0)
	{
		sampleAspect.num = rational.num;
		sampleAspect.den = rational.den;
	}
}


void AVObject::setSampleAspectNum(int num) throw (AVException*)
{
	if (num <= 0)
		throw new AVException ("Invalid Aspect ratio",INVALID_ARGUMENT);
	
	sampleAspect.num = num;
}

void AVObject::setSampleAspectDen(int den) throw (AVException*)
{
	if (den <= 0)
		throw new AVException ("Invalid Aspect ratio",INVALID_ARGUMENT);
	
	sampleAspect.den = den;
}

void AVObject::setChromaSampling(PixelFormat samp)
{
	frameChromaSampling = samp;
}

void AVObject::setChromaSamplingFromY4M(int y4mChroma) throw (AVException*)
{
	
	switch (y4mChroma) {
		case Y4M_CHROMA_420MPEG2: this->setChromaSampling(PIX_FMT_YUV420P); break;
		case Y4M_CHROMA_422: this->setChromaSampling(PIX_FMT_YUV422P); break;
		case Y4M_CHROMA_444 : this->setChromaSampling(PIX_FMT_YUV444P); break;
		case Y4M_CHROMA_411: this->setChromaSampling(PIX_FMT_YUV411P); break;
		case Y4M_CHROMA_420JPEG: this->setChromaSampling(PIX_FMT_YUVJ420P); break;
			
		default:
			throw new AVException("AV: Unsupported Chroma: " + y4mChroma, UNSUPPORTED_CHROMA);
			//	std::cerr << "AV: Unsupported Chroma: " << y4mChroma << "\n";
			break;
	}
	
}

void AVObject::setInterlaced(bool i)
{	
	isInterlaced = i;
}

void AVObject::setInterlaceTopFieldFirst(bool tff)
{
	interlaceTopFieldFirst = tff;
}


void AVObject::setHeight(int hei)
{
	frameHeight = hei;
}

void AVObject::setWidth(int wid) 
{
	frameWidth = wid;
}


AVFrame * AVObject::getAVFrame(void)
{ 
	return pFrame; 
}

AVFrame * AVObject::decodeAndGetNextAVFrame(void)
{ 
	this->decodeNextFrame(); 
	return this->getAVFrame(); 
}

void AVObject::dumpFormat(void)
{
	std::cerr << "Generator AVObject: "  << this->getWidth() << "x" << this->getHeight() << " FPS:" << this->getFrameRateAsString() << " chroma: " << this->getChromaSampling() << " IN:" << this->getInTimecode() << " OUT:" <<this->getOutTimecode() << "\n";
	
}

int AVObject::decodeNextAudio(void)
{
	// how to handle the in and out.
	
	if (sampleCounter > this->getSamplesOut())
		return -1;
	
	// do
	int samples = bufferSize / this->getSampleSize() * this->getSampleChannels();
	if ((sampleCounter < this->getSamplesIn()) && (sampleCounter + samples > this->getSamplesOut()))
	{
		// send partial frame
		// copy sampleBuffer + [self getSamplesIn] - sampleCounter to (sampleCounter + samples) - [self getSamplesOut]
		// bytes = [self getSamplesOut] - [self getSamplesIn]
	}
	else if ((sampleCounter < this->getSamplesIn()) && (sampleCounter + samples >=this->getSamplesIn()))
	{
		// send partial frame
	} 
	else if ((sampleCounter <= this->getSamplesOut()) && (sampleCounter + samples > this->getSamplesOut()))
	{
		// send partial frame
	}
	else 
	{
		// send entire frame.
	}
	// sampleCount += samples. 
}

int AVObject::decodeNextFrame(void)
{
	if (this->compareRange(frameCounter) < 0)
		frameCounter = getIn();
	
	// I only need to do this once.
	if (this->compareRange(frameCounter) == 0) {
		
		memset(pFrame->data[0],colour_y,this->getHeight()*pFrame->linesize[0]);
		memset(pFrame->data[1],colour_u,this->getChromaHeight()*pFrame->linesize[1]);
		memset(pFrame->data[2],colour_v,this->getChromaHeight()*pFrame->linesize[2]);
		frameCounter++;
		
		return this->getHeight()*pFrame->linesize[0] +
		this->getHeight()*pFrame->linesize[1] +
		this->getHeight()*pFrame->linesize[2];
	}
	return -1;
	
}


void AVObject::freeAVFrame(void)
{ 
	av_free(pFrame); 
	pFrame = NULL;
}

AVObject::~AVObject() {
	
	// std::cerr << ">> AVObject destructor\n";
	
	if (pFrame)
		avcodec_free_frame(&pFrame);
	
	if (pictureBuffer)
		av_free(pictureBuffer);
	
#if LIBAVFORMAT_VERSION_MAJOR  < 54
	if (aBuffer)
		av_free(aBuffer);
#endif
}

