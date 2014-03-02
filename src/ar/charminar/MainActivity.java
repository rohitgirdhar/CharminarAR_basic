package ar.charminar;

import java.io.File;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;

public class MainActivity extends Activity {

    private Uri fileUri;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Intent camIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE, fileUri, this, null);
        File out = Environment.getExternalStorageDirectory();
        out = new File(out, "charminarAR/temp.jpg");
        out.delete();
        camIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(out)); // set the image file name
        startActivityForResult(camIntent, 100);
    }
    
    @Override
    protected void onActivityResult(int requestCode,
            int resultCode,
            Intent data) {
        Log.v("MainActivity", "Transformation starting");
        Context context = getApplicationContext();
        CharSequence text = "Transformation started";
        int duration = Toast.LENGTH_SHORT;
        Toast toast = Toast.makeText(context, text, duration);
        toast.show();
      
        transform();
        
        text = "Transformation done";
        toast = Toast.makeText(context, text, duration);
        toast.show();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    
    static {
        System.loadLibrary("vision");
    }
    
    public static native int transform();

}
