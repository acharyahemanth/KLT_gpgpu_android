package hemanth.kltgpgpuandroid;

import android.app.Activity;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

import java.io.IOException;
import java.util.List;

/**
 * Created by hemanth on 8/3/16.
 */
public class CameraClass {
    Activity parentActivity;
    Camera.PreviewCallback mCamPreviewCallback;
    final int mPreviewWidth = 1280;
    final int mPreviewHeight = 720;
    private static Camera mCamera=null;
    private int camFacing = Camera.CameraInfo.CAMERA_FACING_BACK;
    private static CameraHandlerThread mThread = null;
    private static Camera cam = null;
    private byte preview_buff[] = null;
    private SurfaceTexture testSurfaceTexture;

    public CameraClass(Activity parentActivity, Camera.PreviewCallback camPreviewCallback) {
        mCamPreviewCallback = camPreviewCallback;
        initCam();
    }

    private void initCam() {
        if (mCamera != null)
            return;

        try {
            mCamera = openCamera(camFacing);
        } catch (Exception e) {
            AppHelperFuncs.myLOGE("Unable to open camera!");
            System.err.println(e.getMessage());
            return;
        }
        AppHelperFuncs.myLOGD("Camera opened...");
        setupCamera();
    }

    //Opens camera on a new thread
    private Camera openCamera(int camFacing) {
        int cameraCount = 0;
        Camera cam = null;
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        cameraCount = Camera.getNumberOfCameras();
        for (int camIdx = 0; camIdx < cameraCount; camIdx++) {
            Camera.getCameraInfo(camIdx, cameraInfo);
            if (cameraInfo.facing == camFacing) {

                if (mThread == null) {
                    mThread = new CameraHandlerThread();
                }

                cam = null;
                synchronized (mThread) {
                    cam = mThread.openCamera(camIdx);
                }
            }
        }

        return cam;
    }

    private static class CameraHandlerThread extends HandlerThread {
        Handler mHandler = null;

        CameraHandlerThread() {
            super("CameraHandlerThread");
            start();
            mHandler = new Handler(getLooper());
        }

        synchronized void notifyCameraOpened() {
            notify();
        }

        Camera openCamera(final int camIdx) {
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    cam = null;
                    try {
                        cam = Camera.open(camIdx);
                    } catch (RuntimeException e) {
                        AppHelperFuncs.myLOGE("Camera failed to open: " + e.getLocalizedMessage());
                    }

                    notifyCameraOpened();
                }
            });
            try {
                wait();
            }
            catch (InterruptedException e) {
                AppHelperFuncs.myLOGD("wait was interrupted");
            }
            return cam;
        }
    }

    private void setupCamera() {
        setupCameraParams();
        setupCameraCallback();
    }

    private void setupCameraParams() {

        Camera.Parameters param;
        param = mCamera.getParameters();

        //Set 720p resolution---
        param.setPreviewSize(mPreviewWidth, mPreviewHeight);
        AppHelperFuncs.myLOGD("Setting preview resolution to " + param.getPreviewSize().width + "x" + param.getPreviewSize().height);

        //Set Continous Focus
        param = setFocusMode(param);

        mCamera.setParameters(param);
    }

    private Camera.Parameters setFocusMode(Camera.Parameters param) {
        boolean isContinousPictureFocusAvailable, isContinousVideoFocusAvailable;
        isContinousPictureFocusAvailable = isContinousVideoFocusAvailable = false;
        List<String> focusModes = param.getSupportedFocusModes();
        for (String str : focusModes) {
            if (str.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                isContinousPictureFocusAvailable = true;
            } else if (str.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
                isContinousVideoFocusAvailable = true;
            }
        }
        if(isContinousPictureFocusAvailable){
            AppHelperFuncs.myLOGD("Setting Focus Mode to FOCUS_MODE_CONTINOUS_PICTURE");
            param.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }
        else if(isContinousVideoFocusAvailable){
            AppHelperFuncs.myLOGD("Setting Focus Mode to FOCUS_MODE_CONTINUOUS_VIDEO");
            param.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }

        return param;
    }


    void setupCameraCallback() {
        //Setup callback ------------
        //mCamera.setPreviewCallback(preview_callback);

        //Setup callback with buffer------------
        if (preview_buff == null) {
            preview_buff = new byte[(int) (mPreviewWidth * mPreviewHeight * 1.5)];
        }
        mCamera.addCallbackBuffer(preview_buff);
        mCamera.setPreviewCallbackWithBuffer(mCamPreviewCallback);

        try {
            testSurfaceTexture = new SurfaceTexture(10);
            mCamera.setPreviewTexture(testSurfaceTexture);
        } catch (IOException e1) {
            AppHelperFuncs.myLOGE(e1.getMessage());
        }
    }

    public void startCamera(){
        try {
            AppHelperFuncs.myLOGD("Starting preview...");
            mCamera.startPreview();
        } catch (Exception e) {
            // check for exceptions
            AppHelperFuncs.myLOGD("Unable to start Preview!");
        }
    }

    public void stopCamera() {
        if (mCamera != null) {
            // stop preview and release camera
            AppHelperFuncs.myLOGD("Stopping Camera...");
            mCamera.stopPreview();
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }

}
