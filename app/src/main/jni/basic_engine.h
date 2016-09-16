#ifndef BASIC_ENGINE_H
#define BASIC_ENGINE_H

#include <jni.h>

class BasicEngine{
public:
    virtual ~BasicEngine(){};
    virtual void createTrackerObject(int image_width, int image_height)=0;
    virtual void processFrame(unsigned char * _in, jint width, jint height)=0;
    virtual void drawFrame()=0;
};

#endif