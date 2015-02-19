package com.example.imagematcher;

//import org.opencv.android.OpenCVLoader;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;

import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.os.Environment;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.os.Build;
import android.widget.*;
import android.provider.MediaStore;
import android.util.Log;
import android.content.Intent;
import android.net.Uri;


public class MainActivity extends ActionBarActivity {

	//private native int getMatch(String imgPath, String descDir, String outPath);
	public static String _imgpath;		// Save the name of the picture taken
	private static String _descDir;		// Save the Descriptor directory path
	private static String _outPath;		// Save the output folder path
	private static boolean _matchModeFlag = true;

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
				

        
       // _path = Environment.getExternalStorageDirectory() + "/Pictures/query.jpg";

		if (savedInstanceState == null) {
			getSupportFragmentManager().beginTransaction()
					.add(R.id.container, new PlaceholderFragment()).commit();
		}
	}
	
	/** Called when the user clicks the button Take_Picture */
	public void takePicture(View view) {
	    // Do something in response to button
		
		startCameraActivity();
	}
	
	
	protected void startCameraActivity()
    {
    	//Log.i("MakeMachine", "startCameraActivity()" );
		String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());

		_imgpath = Environment.getExternalStorageDirectory() + "/Pictures/query_"+timeStamp + ".jpg";
    	File file = new File( _imgpath );
    	Uri outputFileUri = Uri.fromFile( file );
    	
    	//Intent intent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE );
    	//intent.putExtra( MediaStore.EXTRA_OUTPUT, outputFileUri );
    	
    	//startActivityForResult( intent, 1 );
		
		Intent takePictureIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
		takePictureIntent.putExtra( MediaStore.EXTRA_OUTPUT, outputFileUri );
	    if (takePictureIntent.resolveActivity(getPackageManager()) != null) {
	        startActivityForResult(takePictureIntent, 1);
	    }

    }
	
	
	public void onRadioButtonClicked(View view) {
	    // Is the button now checked?
	    boolean checked = ((RadioButton) view).isChecked();
	    
	    // Check which radio button was clicked
	    if (view.getId() == R.id.radio0){
	    	if (checked)
                _matchModeFlag = true; //get similar match
	    }
	    else if(view.getId() == R.id.radio1){
	    	if (checked)
            	_matchModeFlag = false; //get exact match
	    }
	    	
	}
	
	/** Called when the user clicks the button Find_Matches */
	public void getMatches(View view){
		//System.out.println("start matching");
		imgRetrieval.match();
		//System.out.println("finished matching");
		displayMatches();
		
    }
	
	// Display the results
	protected void displayMatches()
	{
		// Location of the result file
		String _resFile = Environment.getExternalStorageDirectory() + "/Results/AndroidAccuracy.txt";
		//System.out.println("display");
		try {
			 //Read the result file and its contents
			 Scanner fileIn = new Scanner(new File(_resFile));
			 String match1 = fileIn.next();
			 String match2 = fileIn.next();
			 String match3 = fileIn.next();
			 String match4 = fileIn.next();
			 
			 String result_String = new String();
			 
			 // Display number of matches according to the selected mode
			 if (_matchModeFlag == false){
				 result_String = match1;
			 }
			 else if(_matchModeFlag == true){
				 result_String = match1 + "\n" + match2 + "\n" + match3 + "\n" + match4;
			 }
			 
			 // Write the result
			 ((TextView)findViewById (R.id.editText1)).setText (result_String);
			 
			//System.out.print("output");
			 //System.out.println(match1);
			 //System.out.println(match2);
			 //System.out.println(match3);
			 //System.out.println(match4);
		}
		catch(IOException ex){
			
		}
		
		
		
		//getMatch(_imgpath, descDir, outPath);
		
	}
	

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
	
	

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			return rootView;
		}
		
	}
	

	

}
