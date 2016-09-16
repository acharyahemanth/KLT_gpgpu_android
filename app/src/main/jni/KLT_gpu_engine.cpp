#include "KLT_gpu_engine.h"

KLTGpuEngine::KLTGpuEngine() {
    new_cam_image_available = false;
    is_first_frame = true;
    mtx_camera.unlock();
}

KLTGpuEngine::~KLTGpuEngine() {
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
            cv::goodFeaturesToTrack(algo_image, prev_corners, 100, 0.01, 10);
            myLOGD("First frame : Found %d corners!",prev_corners.size());
            prev_image = algo_image.clone();
            is_first_frame = false;
        }
        else {//Run KL tracker and render image with corners painted
            std::vector<bool>error;
            std::vector<cv::Point2f>tracked_corners;
            klt->execute(prev_image, algo_image, prev_corners, tracked_corners, error);
//            klt->execute_ocv(prev_image, algo_image, prev_corners, tracked_corners, error);
            klt->drawFrame(back_image, 1280, 720, tracked_corners, error);
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