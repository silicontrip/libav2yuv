
#import "AVObject.h"

@implementation AVObject 

- (int)getFrameRateNum {
	return frameRate.num;
}
- (int)getFrameRateDen {
	return frameRate.den;
}

- (int)getSampleAspectNum {
	return sampleAspect.num;
}
- (int)getSampleAspectDen {
	return sampleAspect.den;
}
- (int)getChromaSampling {
	return frameChromaSampling;
}
- (int)getHeight {
	return frameHeight;
}
- (int)getWidth {
	return frameWidth;
}

-(int)getChromaHeight {
	//dependent on chroma
	return frameHeight;
}


-(int)getChromaWidth {
	//dependent on chroma
	return frameWidth;
}



- (int)getFrameCounter {
	return frameCounter;
}

- (int) getIn {
	return frameIn;
}

- (int) getOut {
	return frameOut;
}

- (void)setIn:(int)fin {
	// validate
	if (fin >= 0)
		frameIn = fin;
}

- (void)setOut:(int)fout {
	// validate
	if (fout >= 0)
		frameOut = fout;
}

- (void)setFrameRate:(AVRational)rational {
	if (rational.den>0 && rational.num>0)
	{
		frameRate.num = rational.num;
		frameRate.den = rational.den;
	}
}

- (void)setFrameRateNum:(int)num {
	if (num > 0)
		frameRate.num = num;
}

- (void)setFrameRateDen:(int)den {
	if (den > 0)
		frameRate.den = den;
}

- (void)setSampleAspect:(AVRational)rational {
	if (rational.den>0 && rational.num>0)
	{
		sampleAspect.num = rational.num;
		sampleAspect.den = rational.den;
	}
}

- (void)setSampleAspectNum:(int)num {
	if (num > 0)
		sampleAspect.num = num;
}

- (void)setSampleAspectDen:(int)den {
	if (den > 0)
		sampleAspect.den = den;
}

- (void)setChromaSampling:(int)samp {
	frameChromaSampling = samp;
}

- (void)setHeight:(int)hei {
	frameHeight = hei;
}

- (void)setWidth:(int)wid {
	frameWidth = wid;
}

@end