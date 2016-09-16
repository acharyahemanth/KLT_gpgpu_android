package hemanth.kltgpgpuandroid;

import android.content.Context;
import android.opengl.EGL14;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

/**
 * Created by hemanth on 8/29/16.
 */
public class MyGLSurfaceView extends GLSurfaceView {
    private MyGLRenderer mRenderer;
//    private native void destroySurfaceNative();

    public MyGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);

        try {
            setEGLContextClientVersion(3);
            mRenderer = new MyGLRenderer(context);
            setRenderer(mRenderer);
        } catch (Exception e) {
            AppHelperFuncs.myLOGE("GLES context creation failed!!");
        }

        // Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);//        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        AppHelperFuncs.myLOGD("myGLSurfaceView::surfaceDestroyed()");
        JNICaller.deleteObjectNative();
    }
}
