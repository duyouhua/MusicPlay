package com.hail.musicplay.activity;

import java.util.ArrayList;

import android.app.Activity;
import android.content.ContentResolver;
import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;

import com.hail.musicplay.R;
import com.hail.musicplay.adapter.SongAdapter;
import com.hail.musicplay.data.SongInfo;
import com.hail.musicplay.jni.MusicPlay;

public class MainActivity extends Activity implements OnClickListener {
	private String TAG = "MusicPlay";
	private ListView mSongListView;
	private SongAdapter mAdapter;
	private ArrayList<SongInfo> mSongInfos = new ArrayList<SongInfo>();
	private MusicPlay mPlay = new MusicPlay();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_layout);
		mPlay.init();
		initData();
		initView();
		
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		mPlay.destroy();
	}

	private void initView() {
		Button prevBtn = (Button) findViewById(R.id.audio_btn_prev);
		Button playBtn = (Button) findViewById(R.id.audio_btn_play);
		Button nextBtn = (Button) findViewById(R.id.audio_btn_next);
		prevBtn.setOnClickListener(this);
		playBtn.setOnClickListener(this);
		playBtn.setTag("play");
		nextBtn.setOnClickListener(this);
		mSongListView = (ListView) findViewById(R.id.main_list);
		mSongListView.setOnItemClickListener(new OnItemClickListener() {

			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				// TODO Auto-generated method stub
				Log.i(TAG, mSongInfos.get(position).path);
				mPlay.openFile(mSongInfos.get(position).path);
			}
		});
		mSongListView.setAdapter(mAdapter);
	}

	private void initData() {
		ContentResolver cr = getApplication().getContentResolver();
		// ��ȡ���и���
		Cursor cursor = cr.query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI,
				null, null, null, MediaStore.Audio.Media.DEFAULT_SORT_ORDER);
		Log.i(TAG, "song nub ="+cursor.getCount());
		if (cursor.moveToFirst()) {
			do {
				String path = cursor.getString(cursor
						.getColumnIndex(MediaStore.Audio.Media.DATA));
				String name = cursor.getString(cursor
						.getColumnIndex(MediaStore.Audio.Media.TITLE));
				String singer = cursor.getString(cursor
						.getColumnIndex(MediaStore.Audio.Media.ARTIST));
				long size = cursor.getLong(cursor
						.getColumnIndex(MediaStore.Audio.Media.SIZE));
				int duration = cursor.getInt(cursor
						.getColumnIndex(MediaStore.Audio.Media.DURATION));
				String album = cursor.getString(cursor
						.getColumnIndex(MediaStore.Audio.Media.ALBUM));
				mSongInfos.add(new SongInfo(path, name, singer, album,
						duration, size));
				Log.i(TAG, "name:"+name);
			} while (cursor.moveToNext());
		}
		mAdapter = new SongAdapter(getApplicationContext(), mSongInfos);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.audio_btn_prev:
			Log.i(TAG, "prev");
			break;
		case R.id.audio_btn_play:
			
			Button play = (Button) v;
			Log.i(TAG, (String) play.getText());
			if(play.getText().equals("����"))
			{
				mPlay.play();
				play.setText("ֹͣ");
			}
			else
			{
				mPlay.stop();
				play.setText("����");
			}
			break;
		case R.id.audio_btn_next:
			Log.i(TAG, "next");
			break;
		}
	}
}
