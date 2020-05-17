package com.aispeech.opensl_es_test;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

public class OpenSLActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_s_l);
        playpcm("/sdcard/mydream.pcm");
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void playpcm(String url);

    public static void jumpOpenSLActivity(Context context){
        context.startActivity(new Intent(context,OpenSLActivity.class));
    }
}
