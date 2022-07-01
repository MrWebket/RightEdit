package com.right.edit.core.jni;

public class NativeApi {

    static {
        System.loadLibrary("RightEdit");
    }

    public static native String getVersion();

    public native long createEngine();

    public native void loadThumbnail(long addr, String videoPath, String outDir, int interval, int cropWidth, int cropHeight);
}
