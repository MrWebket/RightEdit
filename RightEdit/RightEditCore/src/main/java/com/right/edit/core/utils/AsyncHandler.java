package com.right.edit.core.utils;

import android.os.Handler;
import android.os.HandlerThread;

public class AsyncHandler {

    private HandlerThread mHandlerThread;
    private Handler mHandler;

    public AsyncHandler(String name) {
        mHandlerThread = new HandlerThread(name);
        mHandlerThread.start();
        mHandler = new Handler(mHandlerThread.getLooper());
    }

    public AsyncHandler(String name, int priority) {
        mHandlerThread = new HandlerThread(name, priority);
        mHandlerThread.start();

        mHandler = new Handler(mHandlerThread.getLooper());
    }

    public synchronized void post(Runnable runnable) {
        if (mHandler != null) {
            mHandler.post(runnable);
        }
    }

    public synchronized void release() {
        if (mHandler != null) {
            mHandler.removeCallbacksAndMessages(null);
            mHandler = null;
        }
    }

    @Override
    protected void finalize() {
        if (mHandlerThread != null) {
            mHandlerThread.quit();
            mHandlerThread = null;
        }
    }
}
