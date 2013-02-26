#pragma once
#include "AVobject.h"
class Transition :
	public AVObject
{
protected:
	AVObject * fromAV;
	AVObject * toAV;

	int frameCounter;
	
	unsigned int frames;
public:
	Transition(AVObject *f, AVObject *t, unsigned int fr);
	~Transition(void);

	void setAVFrom (AVObject *);
	void setAVTo (AVObject *);
	void setFrames (unsigned int);
	
	abstract int decodeNextFrame(void);
	abstract void dumpFormat(void);
};

