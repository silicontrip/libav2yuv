#include "ChromaFilter.h"

ChromaFilter::ChromaFilter(AVObject s) 
{
	setAVSource (s);
}

ChromaFilter::ChromaFilter(AVObject s, int convertMode) 
{
	this->setAVSource (s);
	this->setChromaSamplingFromY4M(convertMode);
}

void ChromaFilter::setAVSource (AVObject s)
{
	source = s;
	
	this->setFrameRate(s.getFrameRate());
	this->setSampleAspect(s.getSampleAspect());
	this->setHeight(s.getHeight());
	this->setWidth(s.getWidth());
	this->setInterlaced(s.getIsInterlaced());
	this->setInterlaceTopFieldFirst(s.getInterlaceTopFieldFirst());
}	

void ChromaFilter::setAVChroma(PixelFormat convertMode)
{

	this->setChromaSampling(convertMode);
	this->allocFrame();

	imgConvertCtx = sws_getContext(this->getWidth(), this->getHeight(), source.getChromaSampling(), 
								   this->getWidth(), this->getHeight(), convertMode, SWS_BICUBIC, NULL, NULL, NULL); 

}

void ChromaFilter::setY4MChroma(int convertMode)
{
	
	this->setChromaSamplingFromY4M(convertMode);
	this->allocFrame();
	
	imgConvertCtx = sws_getContext(this->getWidth(), this->getHeight(), source.getChromaSampling(), 
								   this->getWidth(), this->getHeight(), this->getChromaSampling(), SWS_BICUBIC, NULL, NULL, NULL); 
	
}

int ChromaFilter::decodeNextFrame(void)
{

	AVFrame *pFrameSource;
	
	source.decodeNextFrame();
	pFrameSource = source.getAVFrame();
	
	return sws_scale(imgConvertCtx,  (const uint8_t * const *)((AVPicture *)pFrameSource)->data,
					 pFrameSource->linesize, 0, this->getHeight(),pFrame->data, pFrame->linesize);
	
}

void ChromaFilter::dumpFormat(void)
{
	std::cerr << "chroma conversion filter: " << this->getChromaSamplingName() << "\n";
	source.dumpFormat();
}
