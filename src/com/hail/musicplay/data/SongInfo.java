package com.hail.musicplay.data;

import android.graphics.Bitmap;

public class SongInfo {
	public String path;
	public String name;
	public String singer;
	public String album;
	public int duration;
	public long size;
	public Bitmap mMusicBmp;
	
	public SongInfo(String path,String name,String singer,String album,int duration, long size)
	{
		this.name = name;
		this.singer = singer;
		this.album = album;
		this.duration = duration;
		this.size = size;
		this.path = path;
	}
}
