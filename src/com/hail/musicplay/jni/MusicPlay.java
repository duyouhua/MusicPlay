package com.hail.musicplay.jni;

public class MusicPlay {
	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("MusicPlay");
	}

	public native boolean init();

	public native boolean openFile(String path);

	public native void play();

	public native void pause();

	public native void stop();

	public native void setMute(boolean isMute);
	public native void destroy();
}
