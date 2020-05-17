package com.aispeech.opensl_es_test;

import android.os.Bundle;
import android.util.Log;

import com.hjq.permissions.OnPermission;
import com.hjq.permissions.XXPermissions;

import java.util.List;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

//        <uses-permission android:name="android.permission.RECORD_AUDIO" /> <!-- MODIFY_AUDIO_SETTINGS is needed to use audio effects such as environmental reverb -->
//    <uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" /> <!-- INTERNET is needed to use a java.net.URI-based audio player, depending on the URI -->
//    <uses-permission android:name="android.permission.INTERNET" />
//    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
//    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

    public static final String[] mpermissions = new String[]{
            "android.permission.READ_EXTERNAL_STORAGE",
            "android.permission.WRITE_EXTERNAL_STORAGE",
            "android.permission.RECORD_AUDIO",
            "android.permission.MODIFY_AUDIO_SETTINGS",
            "android.permission.INTERNET",
    };

    private static final String TAG = MainActivity.class.getSimpleName();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        hasPermissions();
    }

    private void hasPermissions() {
        Log.d(TAG,"hasPermissions()");
        if (XXPermissions.isHasPermission(MainActivity.this,mpermissions)) {
            OpenSLActivity.jumpOpenSLActivity(this);
            finish();
        }else {
            goPermissions();
        }
    }

    private void goPermissions() {
        Log.d(TAG,"goPermissions()");
        XXPermissions.with(this)
                .constantRequest()
                .permission(mpermissions)
                .request(new OnPermission() {
                    @Override
                    public void hasPermission(List<String> granted, boolean isAll) {
                        Log.d(TAG,"hasPermissions() isAll:"+isAll);
                        if (isAll){
                            OpenSLActivity.jumpOpenSLActivity(MainActivity.this);
                            finish();
                        }
                    }

                    @Override
                    public void noPermission(List<String> denied, boolean quick) {
                        Log.d(TAG,"noPermission() quick:"+quick);
                        if (quick)
                            XXPermissions.gotoPermissionSettings(MainActivity.this);
                        else
                            finish();
                    }
                });
    }
}
