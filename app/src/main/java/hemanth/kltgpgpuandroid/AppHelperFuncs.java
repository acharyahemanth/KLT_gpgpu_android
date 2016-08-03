package hemanth.kltgpgpuandroid;

import android.util.Log;

/**
 * Created by hemanth on 8/3/16.
 */
public class AppHelperFuncs {
    private static String logtag = "klt_gpgpu_debug";

    public static void myLOGE(String s){
        Log.e(logtag, s);
    }
    public static void myLOGD(String s){
        Log.d(logtag, s);
    }

}
