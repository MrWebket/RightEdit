package com.right.edit.core.listener;

import android.graphics.Bitmap;

public interface RHThumbnailListener {

    void onPrepare(long videoDuration, int totalThumbnail,int interval);

    void onThumbnaiFrameAvailable(long pts, int index, Bitmap bitmap);

    void onSaveFrameAvailable(long pst, int index, String path);

    void onComplete(String localPath);
}
