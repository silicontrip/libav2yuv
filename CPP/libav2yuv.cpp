
#import "Libav.h"
#import "Libyuv.h"
#import "Libav2yuvArguments.h"
#import "AVObject.h"
#import "ChromaFilter.h"
#import "EdlListFilter.h"
#import "LibavWaveWriter.h"


// wave writer
void processAudio (Libav2yuvArguments options, std::vector<AVObject> edlList)
{
	
	AVObject lav = edlList.front();
	
	//libavWaveWriter *nfh=[[libavWaveWriter alloc] init];
	
	//NSLog(@"libavWaveWriter initWithFormat");
	
	LibavWaveWriter nfh;
	nfh.setSampleFormat(lav.getSampleFormat());
	nfh.setSampleChannels(lav.getSampleChannels());
	
	nfh.setSamplesPerSecond(lav.getSamplesPerSecond());
	
	if (options.hasOutFile()){
		nfh.setOutputFilename(options.getOutFile());
	} 
	
	
	// need to decode the first frame to get the Samples per second.
	
	if (lav.decodeNextAudio() >= 0) {
		
		nfh.setSamplesPerSecond(lav.getSamplesPerSecond());
		
		nfh.writeHeader();
		nfh.writeFrameData(lav.getAVFrame());
		
		std::vector<AVObject>::iterator audio;
		for (audio=edlList.begin(); audio != edlList.end(); ++audio)
			
			//for (AVObject *audio in edlList) {
			
			//	if (audio != nil) {
			audio->dumpFormat();
		
		
		int sampleSize = audio->getSampleSize() * audio->getSampleChannels();
		
		std::cerr<<"sample size: " <<audio->getSampleSize() <<"x"<< audio->getSampleChannels() << " = " << sampleSize << "\n";
		
		while (audio->decodeNextAudio() >=0)
		{
			nfh.writeFrameData(audio->getAVFrame());
			
			//	[audio freeAVFrame];
		}
		//NSLog(@"audio release");
		
		delete &audio;
	} else {
		std::cerr<< "Couldn't initialise audio\n";
	}
	
	//NSLog(@"nfh release");
	
	// close file
	// delete nfh;	
	
}

void processVideo (Libav2yuvArguments options, std::vector<AVObject> edlList)
{
	
	AVObject lav = edlList.front();
	
	lav.dumpFormat();
	
	//	NSLog(@"%dx%d by %d:%d at %d:%d",[lav getWidth],[lav getHeight],[lav getSampleAspectNum],[lav getSampleAspectDen],[lav getFrameRateNum],[lav getFrameRateDen]);
	
	Libyuv yuv;
	
	//	 NSLog(@"libyuv alloc");
	yuv.setWidth(lav.getWidth()); 
	yuv.setHeight(lav.getHeight()); 
	yuv.setSampleAspectAVRational(lav.getSampleAspect());
	yuv.setFrameRateAVRational(lav.getFrameRate());
	yuv.setChromaSamplingFromAV(lav.getChromaSampling());
	
	if (options.getExtensions())
	{
		yuv.setExtensions(1);
	}
	if (options.hasAspect()) {
		yuv.setSampleAspect(options.getAspect());
	}
	if (options.hasFrameRate()) {
		yuv.setFrameRate(options.getFrameRate());
	}
	if (options.hasChroma()){
		yuv.setChromaSampling(options.getChroma());
	}		
	if (options.hasOutFile()){
		yuv.setOutputFilename(options.getOutFile());
	}
	// need to decode the first frame to get the interlace type
	// NSLog(@"lav decodeNextFrame");
	
	lav.decodeNextFrame();
	//	 NSLog(@"yuv setYUVFrameDataWithAVFrame");
	
	yuv.setYUVFrameDataWithAVFrame(lav.getAVFrame());
	//	NSLog(@"yuv setInterlaceAndOrder");
	
	yuv.setInterlaceAndOrder(lav.getIsInterlaced(), lav.getInterlaceTopFieldFirst());
	//interlace flag is not available until the first frame is decoded.
	//need to get the interlace flags before this. So we can use a generator.
	//	 NSLog(@"yuv writeHeader");
	
	yuv.writeHeader();
	yuv.write();
	// [lav release];
	
	for (std::vector<AVObject>::iterator video=edlList.begin(); video != edlList.end(); ++video)
	{
		//for (AVObject *video in edlList) {
		//	if (video != nil) {
		
		while (video->decodeNextFrame() >=0)
		{
			yuv.setYUVFrameDataWithAVFrame(video->getAVFrame());
			yuv.write();
		}
		// delete video;
	}
	
	// delete yuv;	
	
}

int main(int argc, char *argv[])
{
	
	std::vector<AVObject> edlList;
	//pool = [[NSAutoreleasePool alloc] init];
	//edlList = [NSMutableArray arrayWithCapacity:1];
	int streamMode = AVMEDIA_TYPE_VIDEO;
	
	Libav2yuvArguments options(argc,argv);
	
	if (options.getAudio()) {
		streamMode = AVMEDIA_TYPE_AUDIO;
	}	
	// std::cerr<<"scan arguments\n";
	
	if (options.getArguments().size() > 0) {
		std::cerr<<"argument size " << options.getArguments().size() << "\n";
		std::vector<std::string>::iterator argument;
		
		for (argument=options.getArguments().begin(); argument != options.getArguments().end(); ++argument)
		{	
			
			std::cerr<<  "argument name: " << *argument << "\n";
			
			// for (NSString *argument in [options getArguments]) {
			AVObject lav;
			if (argument->substr(argument->size()-4,4) == ".edl") {
				lav = EdlListFilter(*argument, streamMode);
			} else  {	
				lav = Libav(*argument,streamMode,-1);
			}
			//if (lav != nil) 
			//	{
			if (options.getConvert()) 
			{
				ChromaFilter chromaConverter(lav,options.getChroma());
				//	if (chromaConverter != nil) {
				//[edlList addObject:chromaConverter];
				edlList.push_back(chromaConverter);
				//	}
			} else {
				edlList.push_back(lav);
			}
			
		}	
	}
	// 	NSLog(@"EDL list count: %d",[edlList count]);
	
	if (edlList.size() > 0 ) 
	{
		if (streamMode == AVMEDIA_TYPE_VIDEO) {
			processVideo(options,edlList);
		} else {
			// decode audio
			processAudio(options,edlList);
		}
	}
	
	
}
