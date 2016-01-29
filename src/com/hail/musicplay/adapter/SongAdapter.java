package com.hail.musicplay.adapter;

import java.util.ArrayList;

import com.hail.musicplay.R;
import com.hail.musicplay.data.SongInfo;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

public class SongAdapter extends BaseAdapter {
	private ArrayList<SongInfo> mSongInfos;
	private Context mCtx;
	private LayoutInflater mInflater;

	public SongAdapter(Context ctx, ArrayList<SongInfo> songInfos) {
		mSongInfos = songInfos;
		mCtx = ctx;
		mInflater = LayoutInflater.from(ctx);
	}

	@Override
	public int getCount() {
		// TODO Auto-generated method stub
		return mSongInfos.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return mSongInfos.get(position);
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return position;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// TODO Auto-generated method stub
		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.main_list_item, null);
		}
		((TextView) convertView.findViewById(R.id.song_singer))
				.setText(mSongInfos.get(position).singer);
		String time = "" + (mSongInfos.get(position).duration / 1000) / 60
				+ "·Ö" + (mSongInfos.get(position).duration / 1000) % 60 + "Ãë";
		((TextView) convertView.findViewById(R.id.song_duration)).setText(time);
		((TextView) convertView.findViewById(R.id.song_name))
				.setText(mSongInfos.get(position).name);
		return convertView;
	}

}
