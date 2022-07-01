package com.right.edit.core;

import android.content.Context;

import com.right.edit.core.impl.RHCoreEngineImpl;

public abstract class RHCoreEngine {

    protected volatile static RHCoreEngine mInstance;

    public static RHCoreEngine createEngine(Context context) {
        if(mInstance == null) {
            synchronized (RHCoreEngine.class) {
                mInstance = new RHCoreEngineImpl(context);
            }
        }
        return mInstance;
    }

    public abstract String getVersion();

    public abstract void loadThumbnail(String videoPath, String outDir, int interval,int cropWidth, int cropHeight);
}
