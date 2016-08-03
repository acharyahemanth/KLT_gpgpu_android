/*
 * jni_shoe_algo.cc
 *
 *  Created on: 7 June 2015
 *      Author: AM
 */

//#include "shoe_engine.h"
#include <jni.h>
#include "../mylog.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


//extern BasicEngine * g_basic_engine;

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_MainActivity_helloWorldNative
(JNIEnv *env, jobject obj) {
    cv::Mat a = 5*cv::Mat::ones(5,5,CV_8UC1);
    glm::mat4 b = glm::mat4();

    myLOGD("Hello world!");
    myLOGD("Value of cv mat a[0][0] is %d",a.at<uchar>(0,0));
    myLOGD("Value of glm mat b[0][0] is %f",b[0][0]);
}

#ifdef __cplusplus
}
#endif