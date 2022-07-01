package com.right.edit.control;

import com.right.edit.App;
import com.right.edit.core.RHCoreEngine;

public class RHEditControl {

    private RHCoreEngine mRHCoreEngine;

    private static volatile RHEditControl mControl;

    private RHEditControl() {
        mRHCoreEngine = RHCoreEngine.createEngine(App.getInstance());
    }


    public static RHEditControl getInstance() {
        if(mControl == null) {
            synchronized (RHEditControl.class) {
                mControl = new RHEditControl();
            }
        }
        return mControl;
    }

    public void loadThumbnail(String videoPath, String outDir, int interval, int cropWidth, int cropHeight) {
        mRHCoreEngine.loadThumbnail(videoPath, outDir, interval, cropWidth, cropHeight);
    }
}
