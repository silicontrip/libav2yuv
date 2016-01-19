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
	
	virtual int decodeNextFrame(void) =0;
	virtual void dumpFormat(void) =0;
	virtual void open(void);
};

