/*
 * MusicPlay.h
 *
 *  Created on: 2016Äê1ÔÂ19ÈÕ
 *      Author: hail
 */

#ifndef MUSICPLAY_H_
#define MUSICPLAY_H_

#include "MusicDecoder.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
enum PlayState
{
	PlayState_Pause, PlayState_Stop, PlayState_Playing
};

class MusicPlay
{
public:
	~MusicPlay();
	static bool Init();
	static void Destory();
	static inline MusicPlay* GetObject()
	{
		return m_Player;
	}
private:
	MusicPlay();
	MusicPlay(const MusicPlay& play);
	MusicPlay & operator =(const MusicPlay &);
	static MusicPlay* m_Player;

	typedef void PlayCallBack(SLAndroidSimpleBufferQueueItf bq, void *context);
	const static SLEnvironmentalReverbSettings reverbSettings;
	// engine interfaces
	static SLObjectItf engineObject;
	static SLEngineItf engineEngine;
	// output mix interfaces
	static SLObjectItf outputMixObject;
	static SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
	static SLObjectItf bqPlayerObject;
	static SLEffectSendItf bqPlayerEffectSend;
	static SLMuteSoloItf bqPlayerMuteSolo;
	static SLVolumeItf bqPlayerVolume;
	static SLPlayItf bqPlayerPlay;
	static SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
	static bool CreateBufferQueueAudioPlayer(PlayCallBack callback);
	static void DestroyBufferQueueAudioPlayer();
//	// buffer queue player interfaces
//	static SLDeviceVolumeItf bqeviceVolume;
//	static SLint32 maxVolume;
//	static SLint32 minVolume;
//	static SLboolean IsMillibelScale;
//	// aux effect on the output mix, used by the buffer queue player
	static bool CreateEngine();
	static void DestroyEngine();
	static void MusicPlayCallBack(SLAndroidSimpleBufferQueueItf bq,
			void *context);

public:
	bool Open(const char* path);
	void Play();
	void Pause();
	void SetMute(bool bMute);
	void Stop();
	PlayState GetPlayState();
	void SetPlayState(PlayState playState);
private:
	void Close();
	void PlayBuff(void *pBuffer, int size);
private:
	MusicDecoder* m_decoder;
	bool m_bMute;
	PlayState m_ePlaystate;
};

#endif /* MUSICPLAY_H_ */
