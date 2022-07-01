package com.right.edit.core.impl;

import android.content.Context;
import android.util.Log;

import com.right.edit.core.RHCoreEngine;
import com.right.edit.core.controller.RHEnginelController;
import com.right.edit.core.jni.NativeApi;
import com.right.edit.core.utils.AsyncHandler;

import java.io.File;

public class RHCoreEngineImpl extends RHCoreEngine {

    private AsyncHandler mApiAsyns = new AsyncHandler("ApiAsyns");

    public RHCoreEngineImpl(Context context) {
        super();
    }

    @Override
    public String getVersion() {
        return NativeApi.getVersion();
    }

    @Override
    public void loadThumbnail(String videoPath, String outDir, int interval, int cropWidth, int cropHeight) {
        mApiAsyns.post(new Runnable() {
            @Override
            public void run() {
                File file = new File(outDir);
                if(!file.exists()) {
                    file.mkdirs();
                }
                RHEnginelController.getInstance().loadThumbnail(videoPath, outDir, interval, cropWidth, cropHeight);
            }
        });
    }

}
