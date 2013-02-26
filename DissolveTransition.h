extern "C" {
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <iostream>

#include "AVObject.h"
#include "Transition.h"

class DissolveTransition : public Transition {
public: 
        DissolveTransition(AVObject *f, AVObject *t, unsigned int fr) : Transition(f,t,fr) { };
        ~DissolveTransition(void) { };
        int decodeNextFrame(void);
        void dumpFormat(void);

	
};
