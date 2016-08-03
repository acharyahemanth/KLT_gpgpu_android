package hemanth.kltgpgpuandroid;

import android.hardware.Camera;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends AppCompatActivity {
    CameraClass mCameraObject;
    private native void helloWorldNative();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //Instantiate camera
        mCameraObject = new CameraClass(this, newCamPreviewCallback);

        setContentView(R.layout.activity_main);
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
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onResume() {
        super.onResume();
        helloWorldNative();

        //Start camera
        mCameraObject.initAndStartCamera();
    }

    @Override
    protected void onPause() {
        super.onPause();

        mCameraObject.stopCamera();

        // destroy native objects
//        deleteObjectNative();
        
        finish(); // die..!

    }

    static {
        System.loadLibrary("KLTGPGPUNative");
    }

    public Camera.PreviewCallback newCamPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            AppHelperFuncs.myLOGD("camera callback received!");


//            if(mGLView != null) {
//                //Native call to create edge map data
//                //if(mFramectr >= 30) {
//                //Start Canny after some 30 frames... camera seems to focus
//                sendCamImageToNative(data, mCameraObject.mPreviewHeight, mCameraObject.mPreviewWidth);
//                //}
//                mGLView.requestRender();
//            }

            //Return buffer to camera
            camera.addCallbackBuffer(data);

        }
    };
}
