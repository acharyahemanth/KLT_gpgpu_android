package hemanth.kltgpgpuandroid;

import android.content.res.AssetManager;
import android.graphics.Point;
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

import hemanth.kltgpgpuandroid.R;

public class StandaloneTestActivity extends AppCompatActivity {
    MyGLSurfaceView mGLView=null;
    UIHandler ui_handler=null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        AppHelperFuncs.is_standalone_test = true;

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
        AppHelperFuncs.setScreenDims(screenWidth, screenHeight);
        AppHelperFuncs.myLOGD("Screen size : w/h : " + screenWidth + " " + screenHeight);


    }

    @Override
    protected void onResume() {
        super.onResume();

        //Setup UI handler
        ui_handler = new UIHandler(this);
        AppHelperFuncs.setUIHandler(ui_handler);

        //Setup Layout
        setContentView(R.layout.activity_main);

        //Setup native shader reader object
        String pathToInternalDir = getApplicationContext().getFilesDir().getAbsolutePath();
        AssetManager am = getApplicationContext().getAssets();
        JNICaller.setupShaderReaderNative(pathToInternalDir, am);

    }

    @Override
    protected void onPause() {
        super.onPause();
        finish();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_standalone_test, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public static void setImmersiveSticky(Window appWindow) {

        View decorView = appWindow.getDecorView();
        decorView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN
                        //| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION //HA : Show the navigation bar
                        | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        );//| View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }

    static {
        System.loadLibrary("KLTGPGPUNative");
    }

    private class UIHandler extends Handler{
        StandaloneTestActivity parent_activity;

        UIHandler(StandaloneTestActivity a){
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

    public void loadResourcesDone(){
        AppHelperFuncs.myLOGD("loadResourcesDone()");
        mGLView = (MyGLSurfaceView)findViewById(R.id.glsurfaceview);
        mGLView.requestRender();//seems to be called already...
    }



}
