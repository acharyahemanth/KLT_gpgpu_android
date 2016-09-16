package hemanth.kltgpgpuandroid;

import android.content.res.AssetManager;
import android.graphics.Point;
import android.hardware.Camera;
import android.os.Handler;
import android.os.Message;
import android.support.v4.view.WindowCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Display;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity {
    CameraClass mCameraObject;
    MyGLSurfaceView mGLView=null;
    UIHandler ui_handler=null;
    private static native void createEngine();
    private static native void toggleCPUGPUOperationNative();
    private static native void startTrackingNative();
    ImageView startButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //Set Navbar and ActionBar properties
        supportRequestWindowFeature(WindowCompat.FEATURE_ACTION_BAR_OVERLAY);
        setImmersiveSticky(getWindow());
        View decorView = getWindow().getDecorView();
        decorView.setOnSystemUiVisibilityChangeListener
                (new View.OnSystemUiVisibilityChangeListener() {
                    @Override
                    public void onSystemUiVisibilityChange(int visibility) {
                        setImmersiveSticky(getWindow());
                    }
                });

        //Set screen dimensions
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getRealSize(size);
        int screenWidth = size.x;
        int screenHeight = size.y;
        AppHelperFuncs.setScreenDims(screenWidth,screenHeight);
        AppHelperFuncs.myLOGD("Screen size : w/h : " + screenWidth + " " + screenHeight);


        //Instantiate camera
        mCameraObject = new CameraClass(this, newCamPreviewCallback);

        //Instantiate native object
        createEngine();

        setContentView(R.layout.activity_main);
        startButton = (ImageView) findViewById(R.id.start_button);
        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                startTrackingNative();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.toggle_cpu_gpu) {
            toggleCPUGPUOperationNative();
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {
        super.onResume();

        ui_handler = new UIHandler(this);
        AppHelperFuncs.setUIHandler(ui_handler);

        JNICaller.helloWorldNative();

        String pathToInternalDir = getApplicationContext().getFilesDir().getAbsolutePath();
        AssetManager am = getApplicationContext().getAssets();
//        JNICaller.testAssetFolderReadNative(pathToInternalDir, am);
        JNICaller.setupShaderReaderNative(pathToInternalDir, am);

        //Start camera
        mCameraObject.initAndStartCamera();
    }

    @Override
    protected void onPause() {
        super.onPause();

        AppHelperFuncs.myLOGD("onPause() : Closing app...");

        mCameraObject.stopCamera();

        finish(); // die..!

    }

    static {
        System.loadLibrary("KLTGPGPUNative");
    }

    public Camera.PreviewCallback newCamPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
//            AppHelperFuncs.myLOGD("camera callback received!");

            if(mGLView != null) {
                JNICaller.processFrameNative(data,mCameraObject.mPreviewWidth,mCameraObject.mPreviewHeight);


                //Native call to create edge map data
                mGLView.requestRender();
            }

            //Return buffer to camera
            camera.addCallbackBuffer(data);

        }
    };

    public static void setImmersiveSticky(Window appWindow) {

        View decorView = appWindow.getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                        //| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION //HA : Show the navigation bar
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        );//| View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
        // AM: commented this out to enable listview in landscape.  why?? tbd
    }

    public void loadResourcesDone(){
        AppHelperFuncs.myLOGD("loadResourcesDone()");
        mGLView = (MyGLSurfaceView)findViewById(R.id.glsurfaceview);
    }

    private class UIHandler extends Handler {
        MainActivity parent_activity;

        UIHandler(MainActivity a){
            parent_activity = a;
        }

        public void handleMessage(Message msg) {
            final int what = msg.what;
            AppHelperFuncs.UI_HANDLER_STATES[] uiStateNames = AppHelperFuncs.UI_HANDLER_STATES.values();
            AppHelperFuncs.myLOGD("UI thread : Message received : " +
                    uiStateNames[msg.what]);
            switch(uiStateNames[what]) {
                case LOAD_RESOURCES_DONE:
                    loadResourcesDone();
                    break;
            }
        }
    }

}
