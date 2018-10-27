package com.create.whc.wxpicturecompress;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        findViewById(R.id.btn).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                File input=new File(Environment.getExternalStorageDirectory(),"main.jpg");
                Bitmap inputBitmap= BitmapFactory.decodeFile(input.getAbsolutePath());
                File output=new File(Environment.getExternalStorageDirectory(),"压缩图片.jpg");
                nativeCompress(inputBitmap,output.getAbsolutePath());
            }
        });

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void nativeCompress(Bitmap bitmap, String absolutePath);

}
