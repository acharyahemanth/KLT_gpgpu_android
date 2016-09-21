#ifndef STANDALONETESTENGINEH
#define STANDALONETESTENGINEH

#include "basic_engine.h"
#include "klt_gpu.h"
#include <jni.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <mutex>


//Constants---
#define NUM_PYRAMID_LEVELS 3
#define SEARCH_WINDOW_SIZE 21


class StandaloneTestEngine : public BasicEngine{
public:
    StandaloneTestEngine();
    ~StandaloneTestEngine();
    void createTrackerObject(int image_width, int image_height){};
    void processFrame(unsigned char * _in, jint width, jint height){};
    void drawFrame(){};
    void runStandaloneTest();
private:
    KLT_gpu *klt;
    std::vector<cv::Point2f>tracked_corners;
    std::vector<bool>error;
    cv::Mat back_img;

};

#endif