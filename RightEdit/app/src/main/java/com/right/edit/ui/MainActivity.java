package com.right.edit.ui;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import androidx.annotation.Nullable;

import com.right.edit.R;
import com.right.edit.control.RHEditControl;
import com.right.edit.core.RHCoreEngine;

import java.io.File;

public class MainActivity extends Activity implements View.OnClickListener{

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        RHCoreEngine.createEngine(this).getVersion();

        findViewById(R.id.btn_thumbnail).setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btn_thumbnail:

                String thumbnailPath = getExternalFilesDir("video").getPath() + File.separator + "SampleVideo";
                RHEditControl.getInstance().loadThumbnail("/sdcard/SampleVideo_1280x720_20mb.mp4", thumbnailPath, 1000, 160, 90);

//                String thumbnailPath = getExternalFilesDir("video").getPath() + File.separator + "ddmp4";
//                RHEditControl.getInstance().loadThumbnail("/sdcard/dd.mp4", thumbnailPath, 1000, 160, 90);
                break;
        }
    }
}
