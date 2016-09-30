#include "KLT_gpu_engine.h"

long long unsigned currentTimeInMilliseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

KLTGpuEngine::KLTGpuEngine(int screen_width, int screen_height) {
    new_cam_image_available = false;
    is_first_frame = true;
    mtx_camera.unlock();
    prev_framedraw_time = 0;
    average_fps = 0;
    fps_averaging_ctr = 0;
    run_on_gpu = true;
    start_tracking = false;
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

KLTGpuEngine::~KLTGpuEngine() {
    myLOGD("~KLTGpuEngine()");
    mtx_camera.lock();
    if(klt)
        delete klt;
    mtx_camera.unlock();
}

void KLTGpuEngine::createTrackerObject(int image_width, int image_height) {
    klt = new KLT_gpu(NUM_PYRAMID_LEVELS,SEARCH_WINDOW_SIZE,image_width, image_height);
}

void KLTGpuEngine::processFrame(unsigned char * _in, jint width,
                                jint height) {
    //YUV -> RGBA conversion
    cv::Mat yuv(height * 1.5, width, CV_8UC1, (uchar *) _in);

    mtx_camera.lock();
        cv::cvtColor(yuv, cam_image_for_back, CV_YUV2RGB_NV21, 3);
        cv::resize(cam_image_for_back, cam_image_for_back, cv::Size(0,0),0.5,0.5);//640x360
        new_cam_image_available = true;
    mtx_camera.unlock();
}

void KLTGpuEngine::drawFrame() {

    //If new image available, copy into back_image
    bool run_algo=false;
    mtx_camera.lock();
        if(new_cam_image_available){
            back_image = cam_image_for_back.clone();
            new_cam_image_available = false;
            run_algo = true;
        }
    mtx_camera.unlock();


    if(run_algo){
        cv::cvtColor(back_image, algo_image, CV_RGB2GRAY);
        if(is_first_frame){//If first frame, calculate input corners
            prev_corners.clear();
            cv::goodFeaturesToTrack(algo_image, prev_corners, NUM_CORNERS_TO_TRACK, 0.01, 10);
            myLOGD("First frame : Found %d corners!",prev_corners.size());
            prev_image = algo_image.clone();
            prev_framedraw_time = currentTimeInMilliseconds();
            is_first_frame = false;
        }
        else {//Run KL tracker and render image with corners painted
            std::vector<bool>error;
            std::vector<cv::Point2f>tracked_corners;
            if(start_tracking){
                if(run_on_gpu)
                    klt->execute(prev_image, algo_image, prev_corners, tracked_corners, error);
                else
                    klt->execute_ocv(prev_image, algo_image, prev_corners, tracked_corners, error);
            }
            paintDataOnFrame(back_image, tracked_corners.size());
            klt->drawFrame(back_image, screen_width, screen_height, tracked_corners, error);
            prev_image = algo_image.clone();
            prev_corners.clear();
            for(int i=0;i<tracked_corners.size();i++){
                if(!error[i]){
                    prev_corners.push_back(tracked_corners[i]);
                }
            }
            tracked_corners.clear();
            error.clear();
            myLOGD("KLT :: Tracked %d corners...",prev_corners.size());
        }
    }
}

void KLTGpuEngine::paintDataOnFrame(cv::Mat &img, unsigned int num_corners) {

    long long unsigned elapsed_time = currentTimeInMilliseconds() - prev_framedraw_time;
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    double fontScale = 1.3;
    int thickness = 3;
    std::stringstream ss;
    average_fps += 1/(elapsed_time*1e-3);
    fps_averaging_ctr++;

    if(run_on_gpu)
        ss << "GPU fps : " << average_fps / fps_averaging_ctr;
    else
        ss << "CPU fps : " << average_fps / fps_averaging_ctr;
    cv::putText(img, ss.str().c_str(), cv::Point(0,360), fontFace, fontScale, cv::Scalar(255,0,0));
    ss.str("");
    ss << "# corners : " << num_corners;
    cv::putText(img, ss.str().c_str(), cv::Point(0,360-20), fontFace, fontScale, cv::Scalar(255,0,0));


    prev_framedraw_time = currentTimeInMilliseconds();

}

void KLTGpuEngine::switchGPUCPU() {
    mtx_camera.lock();
        run_on_gpu = !run_on_gpu;
        average_fps = 0;
        fps_averaging_ctr = 0;
    mtx_camera.unlock();
}

void KLTGpuEngine::startTracking() {
    mtx_camera.lock();
        start_tracking = true;
        is_first_frame = true;
        average_fps = 0;
        fps_averaging_ctr = 0;
    mtx_camera.unlock();
}