package com.right.edit.core.controller;

import com.right.edit.core.jni.NativeApi;

public class RHEnginelController {

    private volatile static RHEnginelController mController;

    private final NativeApi mNativeApi = new NativeApi();

    private long mNativeAddr;

    private RHEnginelController() {
        createEngine();
    }

    public static RHEnginelController getInstance() {
        if(mController == null) {
            synchronized (RHEnginelController.class) {
                mController = new RHEnginelController();
            }
        }
        return mController;
    }

    public void createEngine() {
        mNativeAddr = mNativeApi.createEngine();
    }

    public void loadThumbnail(String videoPath, String outDir, int interval, int cropWidth, int cropHeight) {
        mNativeApi.loadThumbnail(mNativeAddr, videoPath, outDir, interval, cropWidth, cropHeight);
    }

    public void releaseEngine() {
        mNativeAddr = 0;
    }
}
