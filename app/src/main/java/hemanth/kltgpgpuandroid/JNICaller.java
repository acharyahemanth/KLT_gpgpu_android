package hemanth.kltgpgpuandroid;

import android.content.res.AssetManager;

/**
 * Created by hemanth on 8/29/16.
 */
public class JNICaller {
    public static native void drawFrameNative();
    public static native void loadResourcesNative(int viewPortXOffset, int viewPortYOffset, int viewPortWidth, int viewPortHeight);
    public static native boolean setupGLES3Native();

    public static native void helloWorldNative();
    public static native void testAssetFolderReadNative(String pathToInternalDir, AssetManager am);
    public static native void setupShaderReaderNative(String pathToInternalDir, AssetManager am);
    public static native void processFrameNative(byte [] data, int width, int height);
    public static native void debugGLNative();

}
