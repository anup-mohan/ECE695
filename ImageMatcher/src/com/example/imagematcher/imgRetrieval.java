package com.example.imagematcher;


import android.os.Environment;

public class imgRetrieval{
	
	public native void getMatch(String imgPath, String imgDir, String outPath);
	
	static {
		
		System.loadLibrary("nonfree");
		System.loadLibrary("imgRetrieval");
		System.loadLibrary("opencv_java");
		
	}
	
	
	public static void match(){
	String _descDir = Environment.getExternalStorageDirectory() + "/Descriptors_320x240";
	String _outPath = Environment.getExternalStorageDirectory() + "/Results";
	String _queryImg = MainActivity._imgpath;
	
	
	
	new imgRetrieval().getMatch(_queryImg,_descDir,_outPath);}
	

}
