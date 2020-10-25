package com.example.kass.view;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.viewpager.widget.ViewPager;

import com.example.kass.R;
import com.example.kass.adapter.ViewPagerFragmentApt;
import com.example.kass.utils.MessageEvent;
import com.example.kass.view.fragment.AudioFragment;
import com.example.kass.view.fragment.BaseFragment;
import com.example.kass.view.fragment.IndexFragment;
import com.example.kass.view.fragment.LiveFragment;
import com.example.kass.view.fragment.VideoFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {


    private SurfaceHolder mSurfaceHoder;
    private ArrayList<BaseFragment> mFragmentList;

    //    static {
//        System.loadLibrary("native-lib");
//    }
    @BindView(R.id.viewPager)
    ViewPager mViewPager;
    @BindView(R.id.nav_img_index)
    ImageView mIndexImg;
    @BindView(R.id.nav_img_audio)
    ImageView mAudioImg;
    @BindView(R.id.nav_img_video)
    ImageView mVideoImg;
    @BindView(R.id.nav_img_live)
    ImageView mLiveImg;
    @BindView(R.id.nav_tv_index)
    TextView mIndexTv;
    @BindView(R.id.nav_tv_audio)
    TextView mAudioTv;
    @BindView(R.id.nav_tv_video)
    TextView mVideoTv;
    @BindView(R.id.nav_tv_live)
    TextView mLiveTv;
    @BindView(R.id.nav_rl_index)
    RelativeLayout mIndexRl;
    @BindView(R.id.nav_rl_audio)
    RelativeLayout mAudioRl;
    @BindView(R.id.nav_rl_video)
    RelativeLayout mVideoRl;
    @BindView(R.id.nav_rl_live)
    RelativeLayout mLiveRl;
    private ViewPagerFragmentApt mVideoPagerFragmentApt;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        if (getSupportActionBar() != null) {
            getSupportActionBar().hide();
        }
        setContentView(R.layout.activity_main);

        ButterKnife.bind(this);

        requestPermission();

        initFragementList();


        EventBus.getDefault().register(this);
        // Example of a call to a native method
//        SurfaceView fv = findViewById(R.id.ffmpeg_view);
//        mSurfaceHoder = fv.getHolder();
//        mSurfaceHoder.addCallback(this);

        MessageEvent messageEvent = new MessageEvent("my name is cyl");
        EventBus.getDefault().post(messageEvent);


    }

    @OnClick({R.id.nav_rl_index, R.id.nav_rl_audio, R.id.nav_rl_video, R.id.nav_rl_live})
    public void OnClick(View view) {
        switch (view.getId()) {
            case R.id.nav_rl_index:
                break;
            case R.id.nav_rl_audio:
                break;
            case R.id.nav_rl_video:
                break;
            case R.id.nav_rl_live:
                break;

        }
    }

    private void initFragementList() {
        mFragmentList = new ArrayList<BaseFragment>();
        mFragmentList.add(new AudioFragment());
        mFragmentList.add(new VideoFragment());
        mFragmentList.add(new IndexFragment());
        mFragmentList.add(new LiveFragment());
        mVideoPagerFragmentApt= new ViewPagerFragmentApt(getSupportFragmentManager(),mFragmentList);
        mViewPager.setAdapter(mVideoPagerFragmentApt);
        mViewPager.setCurrentItem(0);

    }

    /**
     * 设置一个ViewPager的侦听事件，当左右滑动ViewPager时菜单栏被选中状态跟着改变
     *
     */
    public class MyPagerChangeListener implements ViewPager.OnPageChangeListener {

        @Override
        public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

        }

        @Override
        public void onPageSelected(int position) {

        }

        @Override
        public void onPageScrollStateChanged(int state) {

        }
    }

    //订阅方法，当接收到事件的时候，会调用该方法
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onEvent(MessageEvent messageEvent) {
        Log.d("cylog", "receive it" + messageEvent.getMessage());
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


    @Override
    protected void onDestroy() {
        super.onDestroy();
        //解除注册
        EventBus.getDefault().unregister(this);
    }

//    public native String render(Surface surface, String inputText);
}
