package com.hail.musicplay;

public class MusicPlay {
	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("MusicPlay");
	}
	private native boolean init();
	private native boolean openFile(String path);
	private native void play();
	private native void pause();
	private native void stop();
	private native void setMute(boolean isMute);
	private native void destroy();
}
