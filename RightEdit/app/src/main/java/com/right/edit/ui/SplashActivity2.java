package com.right.edit.ui;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.view.Window;
import android.view.WindowManager;

import com.right.edit.R;
import com.right.edit.utils.PermissionUtil;


public class SplashActivity2 extends Activity {

    // 延迟启动
    private static final long DELAY_MILLIS = 1000 * 3;

    private Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT); //竖屏
        setContentView(R.layout.splash);

        findView();
        initialize();
    }

    public void findView() {
    }

    public void initialize() {
        PermissionUtil.with(SplashActivity2.this)
                .listener((succ, requestCode) -> {
                    if (succ) {
                        mHandler = new Handler();
                        mHandler.postDelayed(runnable, DELAY_MILLIS);
                    }
                })
                .request(
                        Manifest.permission.CAMERA,
                        Manifest.permission.ACCESS_NETWORK_STATE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE,
                        Manifest.permission.READ_PHONE_STATE,
                        Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.RECORD_AUDIO);

    }

    private final Runnable runnable = () -> {
        startActivity(new Intent(SplashActivity2.this, MainActivity.class));
        finish();
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mHandler != null) {
            mHandler.removeCallbacks(runnable);
        }
    }
}
