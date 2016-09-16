#ifndef KLTGPUENGINEH
#define KLTGPUENGINEH

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


class KLTGpuEngine : public BasicEngine{
public:
    KLTGpuEngine();
    ~KLTGpuEngine();
    void createTrackerObject(int image_width, int image_height);
    void processFrame(unsigned char * _in, jint width, jint height);
    void drawFrame();
private:
    KLT_gpu *klt;
    cv::Mat cam_image_for_back, cam_image_for_algo;
    std::mutex mtx_camera;
    bool new_cam_image_available, is_first_frame;
    cv::Mat back_image, algo_image, prev_image;
    std::vector<cv::Point2f>prev_corners;

    void paintDataOnFrame(cv::Mat &img, unsigned int num_corners);
    long long unsigned prev_framedraw_time, fps_averaging_ctr;
    float average_fps;

};

#endif