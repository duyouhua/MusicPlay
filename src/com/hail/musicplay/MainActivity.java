package com.hail.musicplay;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;

public class MainActivity extends Activity implements OnClickListener{
	private String TAG = "MusicPlay";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
	
	}
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId())
		{
		case R.id.audio_btn_play:
			Log.i(TAG,"Play onclick");
			break;
		case R.id.audio_btn_stop:
			Log.i(TAG,"Play stop");
			break;
		case R.id.audio_btn_pause:
			Log.i(TAG,"Play pause");
			break;
		case R.id.audio_btn_volume_mute:
			Log.i(TAG,"volumeMute");
			break;
		}
	}
}
