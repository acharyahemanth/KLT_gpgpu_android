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
#include "basic_engine.h"
#include "KLT_gpu_engine.h"
#include "ShaderReader.h"
#include "standalone_test_engine.h"

BasicEngine * g_basic_engine;
ShaderReader *shader_reader;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_MainActivity_createEngine
        (JNIEnv *env, jobject obj) {
    myLOGD("MainActivity createEngine()");
    g_basic_engine = new KLTGpuEngine();
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_StandaloneTestActivity_createEngine
        (JNIEnv *env, jobject obj) {
    myLOGD("StandaloneTestActivity createEngine()");
    g_basic_engine = new StandaloneTestEngine();
}


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

    g_basic_engine->createTrackerObject(width,height);
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_processFrameNative
        (JNIEnv *env, jobject obj, jbyteArray inPixels, jint width, jint height) {

    myLOGD("processFrameNative");
    if(g_basic_engine == NULL) {
        return;
    }

    jbyte *_in = env->GetByteArrayElements(inPixels, NULL);
    g_basic_engine->processFrame((unsigned char *)_in, width, height);
    env->ReleaseByteArrayElements(inPixels, _in, JNI_ABORT);
    return;
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_drawFrameNative
        (JNIEnv *env, jobject obj) {
    myLOGD("drawFrameNative");

    if (g_basic_engine == NULL)
        return;

    g_basic_engine->drawFrame();
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_MainActivity_toggleCPUGPUOperationNative
        (JNIEnv *env, jobject obj) {
    myLOGD("toggleCPUGPUOperationNative");

    if (g_basic_engine == NULL)
        return;

    ((KLTGpuEngine *)g_basic_engine)->switchGPUCPU();
}


JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_standaloneTestNative
        (JNIEnv *env, jobject obj) {
    myLOGD("standaloneTestNative");

    if (g_basic_engine == NULL)
        return;

    StandaloneTestEngine *ste = (StandaloneTestEngine*)g_basic_engine;
    ste->runStandaloneTest();
}

JNIEXPORT void JNICALL Java_hemanth_kltgpgpuandroid_JNICaller_debugGLNative
        (JNIEnv *env, jobject obj) {
    myLOGD("debugGLNative");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Render black
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);

    //Setup viewport to smallest texture
    glViewport(0,0,1280,720);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

#ifdef __cplusplus
}
#endif