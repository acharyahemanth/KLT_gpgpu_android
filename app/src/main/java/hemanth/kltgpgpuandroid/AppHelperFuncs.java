package hemanth.kltgpgpuandroid;

import android.util.Log;

/**
 * Created by hemanth on 8/3/16.
 */
public class AppHelperFuncs {
    private static String logtag = "klt_gpgpu_debug";
    private static int screen_width, screen_height;
    private static MainActivity parent_activity_obj;

    public static void myLOGE(String s){
        Log.e(logtag, s);
    }
    public static void myLOGD(String s){
        Log.d(logtag, s);
    }
    public static void setScreenDims(int w, int h){screen_width = w; screen_height = h;};
    public static void setMainActivityReference(MainActivity p){parent_activity_obj = p;};
    public static int getScreenWidth(){return screen_width;};
    public static int getScreenHeight(){return screen_height;};
    public static MainActivity getParentActivityReference(){return parent_activity_obj;};
}
