package com.right.edit;

import android.app.Application;
import android.text.TextUtils;

import java.io.File;

public class App extends Application {

    private static App mApp;


    @Override
    public void onCreate() {
        super.onCreate();
        mApp = this;
    }

    public static App getInstance() {
        return mApp;
    }



}
