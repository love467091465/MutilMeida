package com.example.kass;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {


    private SurfaceHolder mSurfaceHoder;
//    static {
//        System.loadLibrary("native-lib");
//    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        requestPermission();
        // Example of a call to a native method
//        SurfaceView fv = findViewById(R.id.ffmpeg_view);
//        mSurfaceHoder = fv.getHolder();
//        mSurfaceHoder.addCallback(this);



    }


    private static final String[] PERMISSION_EXTERNAL_STORAGE = new String[]{
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};
    private static final int REQUEST_EXTERNAL_STORAGE = 100;

    /**
     * 检查权限方法
     */
    private void requestPermission() {
        //检查权限
        int permissionWrite = ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE);
        if (permissionWrite != PackageManager.PERMISSION_GRANTED) {
            //判断之前是否请求过，之前请求拒绝的话返回true.如果之前拒绝了并且选择了Don't ask again，那返回false.
            //如果设备规范禁止应用具有该权限，此方法也会返回 false。
            boolean b = ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE);
            if (b) {
                //再次申请权限
                ActivityCompat.requestPermissions(this, PERMISSION_EXTERNAL_STORAGE, REQUEST_EXTERNAL_STORAGE);
            } else {
                ActivityCompat.requestPermissions(this, PERMISSION_EXTERNAL_STORAGE, REQUEST_EXTERNAL_STORAGE);
            }
        } else {
            //执行权限允许后的操作！！
            Toast.makeText(this, "已经有权限啦", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {

        String sdDir = Environment.getExternalStorageDirectory().getAbsolutePath();
//        render( mSurfaceHoder.getSurface(),sdDir + "/test.mp4");
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }

    public native String render(Surface surface, String inputText);
}
