package com.right.edit.core.controller;

public class RHThumbnailController {

    private volatile static RHThumbnailController mController;

    private RHThumbnailController() {

    }

    public static RHThumbnailController getInstance() {
        if(mController == null) {
            synchronized (RHThumbnailController.class) {
                mController = new RHThumbnailController();
            }
        }
        return mController;
    }
}
