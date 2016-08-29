/*
 * jni_shoe_algo.cc
 *
 *  Created on: 7 June 2015
 *      Author: AM
 */

//#include "shoe_engine.h"
#include <jni.h>
#include "mylog.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sstream>
#include "gl_setup.h"
#include "gl3stub.h"
#include "klt_gpu.h"
#include "ShaderReader.h"

//extern BasicEngine * g_basic_engine;
ShaderReader *shader_reader;
KLT_gpu *klt;

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_helloWorldNative
(JNIEnv *env, jobject obj) {
    cv::Mat a = 5*cv::Mat::ones(5,5,CV_8UC1);
    glm::mat4 b = glm::mat4();

    myLOGD("Hello world!");
    myLOGD("Value of cv mat a[0][0] is %d",a.at<uchar>(0,0));
    myLOGD("Value of glm mat b[0][0] is %f",b[0][0]);

}

std::string getFileName(std::string file_name) {
    // get just file name, remove path
    // assume filename is of the form <path><name>.<type>
    // path may be missing

    std::string::size_type slashIndex = file_name.find_last_of("/");

    std::string only_name;
    if (slashIndex == std::string::npos) {
        only_name = file_name.substr(0, std::string::npos);
    } else {
        only_name = file_name.substr(slashIndex + 1, std::string::npos);
    }

    return only_name;
}


JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_testAssetFolderReadNative(
        JNIEnv *env, jobject obj, jstring internal_data_path,
        jobject assetManager){
    myLOGD("testAssetFolderRead()");

    //Set internal storage directory path------------
    const char *c_internal_data_path;
    c_internal_data_path = env->GetStringUTFChars( internal_data_path , NULL ) ;
    std::string apkWorkspacePath = std::string(c_internal_data_path);//Set app internal data storage path
    env->ReleaseStringUTFChars(internal_data_path, c_internal_data_path);

    //Pass global reference to asset manager object, apkAssetManager ptr will be valid till this object is destroyed--------
    jobject assetManagerRef = (jclass) env->NewGlobalRef( assetManager );
    AAssetManager*  apkAssetManager = AAssetManager_fromJava(env, assetManager);

    //Extract file---
    std::string assetName = "test.fsh";
    std::string assetPath = apkWorkspacePath + "/" + assetName;

    // Open file
    AAsset* asset = AAssetManager_open(apkAssetManager, assetName.c_str(), AASSET_MODE_STREAMING);

    char buf[BUFSIZ];
    int nb_read = 0;
    std::stringstream ss;
    if (asset != NULL)
    {
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
        {
              ss.write(buf,nb_read);
        }
        myLOGD("Shader code read -> %s",ss.str().c_str());
        AAsset_close(asset);
        myLOGI("Asset extracted");
    }
    else
    {
        myLOGE("Asset not found: %s", assetName.c_str());
    }

//    shader_reader = new ShaderReader(env, assetManager, apkWorkspacePath);
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_setupShaderReaderNative(
        JNIEnv *env, jobject obj, jstring internal_data_path,
        jobject assetManager){
    myLOGD("setupShaderReaderNative()");

    //Set internal storage directory path------------
    const char *c_internal_data_path;
    c_internal_data_path = env->GetStringUTFChars( internal_data_path , NULL ) ;
    std::string apkWorkspacePath = std::string(c_internal_data_path);//Set app internal data storage path
    env->ReleaseStringUTFChars(internal_data_path, c_internal_data_path);

    shader_reader = new ShaderReader(env, assetManager, apkWorkspacePath);
}

JNIEXPORT jboolean JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_setupGLES3Native
        (JNIEnv *env, jobject obj) {
    myLOGD("MyGLRenderer_setupGLES3Native");
    bool is_init_success =false;
    const char* versionStr = (const char*) glGetString( GL_VERSION );
    if( strstr( versionStr, "OpenGL ES 3." ))// && gl3stubInit() )//HA : uncommenting gl3stubInit()... Need to investigate this guys role here...
    {
        is_init_success = true;
    }

    jboolean return_flag = (is_init_success == true) ? JNI_TRUE : JNI_FALSE;
    return return_flag;
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_loadResourcesNative(
        JNIEnv *env, jobject obj, jint xoffset, jint yoffset, jint width, jint height){
    myLOGD("loadResourcesNative");
    myLOGD("width : %d, height : %d",width, height);
    const int num_pyramid_levels = 3;
    const int search_window_size = 7;
    klt = new KLT_gpu(num_pyramid_levels,search_window_size,width, height);

//    myLOGD("Reading Shader...");
//    std::string shader_code = shader_reader->getShader("test.fsh");
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_processFrameNative
        (JNIEnv *env, jobject obj, jbyteArray inPixels, jint width, jint height) {

    myLOGD("processFrameNative");
    if(klt == NULL) {
        return;
    }

    //YUV -> RGBA conversion
    jbyte *_in = env->GetByteArrayElements(inPixels, NULL);
    cv::Mat yuv(height * 1.5, width, CV_8UC1, (uchar *) _in);
    cv::Mat cameraImageForBack;
    cv::cvtColor(yuv, cameraImageForBack, CV_YUV2RGBA_NV21, 4); //~20
//    cv::imwrite("/mnt/sdcard/tryamble_debug/backimg.png",cameraImageForBack);
//    cv::Mat luma(height, width, CV_8UC1, (uchar *) _in);
//    camera_image_small = luma.clone();


    klt->drawFrame(cameraImageForBack);

    env->ReleaseByteArrayElements(inPixels, _in, JNI_ABORT);
    return;
}


#ifdef __cplusplus
}
#endif