#include "Transition.h"


Transition::Transition(AVObject *f, AVObject *t, unsigned int fr)
{
	this->setFrames(fr);
	
	this->setAVFrom(f);
	this->setAVTo(t);
		
	frameCounter = 0;
	
	this->allocFrame();
}


Transition::~Transition(void)
{
	delete fromAV;
	delete toAV;
}

void Transition::setAVFrom (AVObject *s)
{
	fromAV = s;
	
	this->setFrameRate(s->getFrameRate());
	this->setSampleAspect(s->getSampleAspect());
	this->setHeight(s->getHeight());
	this->setWidth(s->getWidth());
	this->setChromaSampling(s->getChromaSampling());

	// we don't get interlace information until the first frame is decoded.
	
	/*
	 this->setInterlaced(s->getIsInterlaced());
	 this->setInterlaceTopFieldFirst(s->getInterlaceTopFieldFirst());
	 */
}	

void Transition::setAVTo (AVObject *s)
{
	toAV = s;
}	

void Transition::setFrames(unsigned int f)
{
	frames = f;
}

void Transition::open (void) {
	fromAV->open();
	toAV->open();
}
