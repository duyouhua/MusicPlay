#ifndef AUDIO_PLAY_H
#define AUDIO_PLAY_H
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>
#define LOG_TAG "AudioPlay"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define MAX_AUDIO_FRAME_SIZE 192000
typedef void PlayCallBack(SLAndroidSimpleBufferQueueItf bq, void *context);


const SLEnvironmentalReverbSettings reverbSettings =
		SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

enum PlayState
{
	PlayState_Playing,
	PlayState_Pause,
	PlayState_Stop
};
class AudioPlay
{
private:
	AudioPlay();
	AudioPlay(const AudioPlay& play);
	AudioPlay & operator=(const AudioPlay &);
public:
	static inline AudioPlay& GetObject()
	{
		static AudioPlay Audio;
		return Audio;
	}
	~AudioPlay();
	void init();
	void createBufferQueueAudioPlayer(int rate, int channel, int bitsPerSample,
			PlayCallBack callback);
	SLresult PlayBuff(void* buff, int len);
	void Pause();
	void Stop();
	PlayState GetPlayState();
	void SetMute(bool mute);
	bool GetMute();
	void destroy();

private:
	void createEngine();
private:
	// engine interfaces
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
	// output mix interfaces
	SLObjectItf outputMixObject;
	SLEnvironmentalReverbItf outputMixEnvironmentalReverb;
	// buffer queue player interfaces
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
	SLEffectSendItf bqPlayerEffectSend;
	SLMuteSoloItf bqPlayerMuteSolo;
	SLVolumeItf bqPlayerVolume;
	SLDeviceVolumeItf  bqeviceVolume;
	SLint32 maxVolume;
	SLint32 minVolume;
	SLboolean IsMillibelScale;
	// aux effect on the output mix, used by the buffer queue player
};
#endif
