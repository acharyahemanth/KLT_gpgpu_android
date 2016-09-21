#include "standalone_test_engine.h"

StandaloneTestEngine::StandaloneTestEngine() {
}

StandaloneTestEngine::~StandaloneTestEngine() {
    if(klt)
        delete klt;
}

void StandaloneTestEngine::runStandaloneTest() {
    static bool is_standalone_test_run = false;
    if(is_standalone_test_run) {
        klt->drawFrame(back_img, 1280, 720, tracked_corners, error);
        return;
    }

    myLOGD(" -------------------- standaloneTestNative -----------------");
    cv::theRNG().state = 12345678;


    //Read in images----------
    cv::Mat source_gray = cv::imread("/mnt/sdcard/tryamble_debug/klt_source.jpg");
    back_img = source_gray.clone();
    if(source_gray.empty()){
        myLOGE("ERROR : Source image empty! ... Bailing out...");
        return;
    }
    else{
        cv::cvtColor(source_gray,source_gray,CV_BGR2GRAY);
        myLOGD("Read in source_gray of dims %d x %d",source_gray.rows,source_gray.cols);
    }
    cv::Mat query_gray = cv::imread("/mnt/sdcard/tryamble_debug/klt_query.jpg");
    if(query_gray.empty()){
        myLOGE("ERROR : Query image empty! ... Bailing out...");
        return;
    }
    else{
        cv::cvtColor(query_gray,query_gray,CV_BGR2GRAY);
        myLOGD("Read in query_gray of dims %d x %d",query_gray.rows,query_gray.cols);
    }


    //Instantiate the KLT object
    klt = new KLT_gpu(NUM_PYRAMID_LEVELS,SEARCH_WINDOW_SIZE,source_gray.cols, source_gray.rows);

    //Find input corners
    std::vector<cv::Point2f>input_corners;//(kps.size());
    cv::goodFeaturesToTrack(source_gray, input_corners, 10, 0.01, 10);

    //HACK!---
//    input_corners.clear();
//    input_corners.push_back(cv::Point2f(17,182));

    myLOGD("Found %d corners in source image",input_corners.size());
//    for(int i=0;i<input_corners.size();i++){
//        myLOGD("(%f,%f)",input_corners[i].x,input_corners[i].y);
//    }


    //Call KL tracker!
    klt->execute(source_gray, query_gray, input_corners, tracked_corners, error);
    myLOGD("KLT output is : ");
    float average_x_shift = 0;
    int average_shift_ctr = 0;
    for(int i=0;i<tracked_corners.size();i++){
        if(!error[i]) {
            myLOGD("(%f,%f) -> (%f,%f)", input_corners[i].x, input_corners[i].y,
                   tracked_corners[i].x, tracked_corners[i].y);
            cv::line(back_img, input_corners[i], tracked_corners[i], cv::Scalar(0, 255, 0));
            average_x_shift += tracked_corners[i].x - input_corners[i].x;
            average_shift_ctr++;
        }
    }

    //Write text on image
    myLOGD("Average x shift -> %f", average_x_shift / average_shift_ctr);

    //Render image
    klt->drawFrame(back_img, 1280, 720, tracked_corners, error);

    is_standalone_test_run = true;
    myLOGD("------------------------- DONE!!! ------------------");

}