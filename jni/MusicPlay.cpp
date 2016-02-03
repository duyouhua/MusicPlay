/*
 * MusicPlay.cpp
 *
 *  Created on: 2016Äê1ÔÂ19ÈÕ
 *      Author: hail
 */

#include "MusicPlay.h"
#include "stdio.h"
#include "JNIUtil.h"

const SLEnvironmentalReverbSettings MusicPlay::reverbSettings =
SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

SLEngineItf MusicPlay::engineEngine = NULL;
SLObjectItf MusicPlay::engineObject = NULL;
SLObjectItf MusicPlay::outputMixObject = NULL;
SLEnvironmentalReverbItf MusicPlay::outputMixEnvironmentalReverb = NULL;
MusicPlay* MusicPlay::m_Player =NULL;
SLObjectItf MusicPlay::bqPlayerObject=NULL;
SLEffectSendItf MusicPlay::bqPlayerEffectSend=NULL;
SLMuteSoloItf MusicPlay::bqPlayerMuteSolo=NULL;
SLVolumeItf MusicPlay::bqPlayerVolume=NULL;
SLPlayItf MusicPlay::bqPlayerPlay=NULL;
SLAndroidSimpleBufferQueueItf MusicPlay::bqPlayerBufferQueue=NULL;
MusicPlay::MusicPlay():
m_ePlaystate(PlayState_Stop),
m_decoder(NULL),
m_bMute(false)
{

}
MusicPlay::~MusicPlay()
{

}
bool MusicPlay::Init()
{
	bool bRet = CreateEngine();
	if (!bRet)
	{
		LOGE("CreateEngine error!!!");
		return bRet;
	}
	bRet = CreateBufferQueueAudioPlayer(MusicPlayCallBack);
	if (!bRet)
	{
		LOGE("CreateBufferQueueAudioPlayer error!!!");
		return bRet;
	}
	MusicDecoder::Init();
	m_Player = new MusicPlay();
	LOGI("musicplay init success!!");
	return true;
}
void MusicPlay::Destory()
{
	delete m_Player;
	m_Player = NULL;
	MusicDecoder::Destroy();
	DestroyBufferQueueAudioPlayer();
	DestroyEngine();
	LOGI("MusicPlay::Destory()");

}
bool MusicPlay::CreateEngine()
{
	SLresult result;
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE,
			&engineEngine);
	if (SL_RESULT_SUCCESS != result)
		return false;
	const SLInterfaceID ids[1] =
	{ SL_IID_ENVIRONMENTALREVERB };
	const SLboolean req[1] =
	{ SL_BOOLEAN_FALSE };
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1,
			ids, req);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*outputMixObject)->GetInterface(outputMixObject,
			SL_IID_ENVIRONMENTALREVERB, &outputMixEnvironmentalReverb);
	if (SL_RESULT_SUCCESS == result)
	{
		result =
				(*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
						outputMixEnvironmentalReverb, &reverbSettings);
	}
	return true;
}
void MusicPlay::DestroyEngine()
{
	if (outputMixObject != NULL)
	{
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
		outputMixEnvironmentalReverb = NULL;
	}
	if (engineObject != NULL)
	{
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
}
bool MusicPlay::CreateBufferQueueAudioPlayer(PlayCallBack callback)
{
	SLresult result;
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq =
	{ SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm =
	{ SL_DATAFORMAT_PCM, 1, SL_SAMPLINGRATE_44_1,
	SL_PCMSAMPLEFORMAT_FIXED_16, SL_PCMSAMPLEFORMAT_FIXED_16,
	SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN };
	SLDataSource audioSrc =
	{ &loc_bufq, &format_pcm };

	SLDataLocator_OutputMix loc_outmix =
	{ SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
	SLDataSink audioSnk =
	{ &loc_outmix, NULL };

	const SLInterfaceID ids[3] =
	{ SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME };
	const SLboolean req[3] =
	{ SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject,
			&audioSrc, &audioSnk, 3, ids, req);
	if (SL_RESULT_SUCCESS != result)
		return false;

	result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result)
		return false;

	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY,
			&bqPlayerPlay);
	if (SL_RESULT_SUCCESS != result)
		return false;

	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
			&bqPlayerBufferQueue);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue,
			callback, NULL);
	if (SL_RESULT_SUCCESS != result)
		return false;

	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_EFFECTSEND,
			&bqPlayerEffectSend);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_VOLUME,
			&bqPlayerVolume);
	if (SL_RESULT_SUCCESS != result)
		return false;
	result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
	if (result != SL_RESULT_SUCCESS)
		return false;
	return true;
}
void MusicPlay::DestroyBufferQueueAudioPlayer()
{
	if (bqPlayerObject != NULL)
	{
		(*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
		(*bqPlayerObject)->Destroy(bqPlayerObject);
		bqPlayerObject = NULL;
		bqPlayerPlay = NULL;
		bqPlayerBufferQueue = NULL;
		bqPlayerEffectSend = NULL;
		bqPlayerMuteSolo = NULL;
		bqPlayerVolume = NULL;
	}
}
bool MusicPlay::Open(const char* path)
{
	LOGI("MusicPlay::Open  path = %s", path);
	if (m_decoder != NULL)
	{
		Stop();
	}
	m_decoder = new MusicDecoder();
	m_decoder->Open(path);
	return true;
}

void MusicPlay::MusicPlayCallBack(SLAndroidSimpleBufferQueueItf bq,
		void *context)
{
	static void* prevPlayBuff = NULL;
	if (GetObject()->m_ePlaystate == PlayState_Playing)
	{
		if (prevPlayBuff != NULL)
		{
			av_free(prevPlayBuff);
			prevPlayBuff = NULL;
		}
		Frame f = GetObject()->m_decoder->GetFrame();
		if (f.data != NULL && f.size != -1)
		{
			GetObject()->PlayBuff(f.data, f.size);
			prevPlayBuff = f.data;
		}
		else
		{
			LOGI("music play finish!");
			GetObject()->Stop();
		}
	}
	else
	{
		LOGI("music play stop!");
	}
}
void MusicPlay::Play()
{
	m_decoder->StartDecodeThread();
	Frame f = m_decoder->GetFrame();
	SetPlayState(PlayState_Playing);
	PlayBuff(f.data, f.size);
}
void MusicPlay::Pause()
{
	SetPlayState(PlayState_Pause);
}
void MusicPlay::SetMute(bool bMute)
{
	if (bMute == m_bMute)
		return;
	SLresult result = (*bqPlayerVolume)->SetMute(bqPlayerVolume, bMute);
	if (result == SL_RESULT_SUCCESS)
		m_bMute = bMute;
}
void MusicPlay::Stop()
{
	Close();
	SetPlayState(PlayState_Stop);
	(*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
}
PlayState MusicPlay::GetPlayState()
{
	PlayState playstate;
	SLuint32 pState = 0;
	SLresult result = (*bqPlayerPlay)->GetPlayState(bqPlayerPlay, &pState);
	if (result != SL_RESULT_SUCCESS)
		return PlayState_Stop;
	switch (pState)
	{
	case SL_PLAYSTATE_PLAYING:
		playstate = PlayState_Playing;
		break;
	case SL_PLAYSTATE_PAUSED:
		playstate = PlayState_Pause;
		break;
	case SL_PLAYSTATE_STOPPED:
		playstate = PlayState_Stop;
		break;
	}

	return playstate;
}
void MusicPlay::SetPlayState(PlayState playState)
{
	if (GetPlayState() == playState)
	{
		return;
	}
	SLuint32 value = SL_PLAYSTATE_STOPPED;
	switch (playState)
	{
	case PlayState_Playing:
		value = SL_PLAYSTATE_PLAYING;
		break;
	case PlayState_Pause:
		value = SL_PLAYSTATE_PAUSED;
		break;
	case PlayState_Stop:
		value = SL_PLAYSTATE_STOPPED;
		break;
	}
	SLresult result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, value);
	if (result == SL_RESULT_SUCCESS)
		m_ePlaystate = playState;
}
void MusicPlay::PlayBuff(void *pBuffer, int size)
{
	SLresult result = (*bqPlayerBufferQueue)->Enqueue(bqPlayerBufferQueue,
			pBuffer, size);
	if (result != SL_RESULT_SUCCESS)
		LOGE("Play buffer error!");
}
void MusicPlay::Close()
{
	m_decoder->Close();
	delete m_decoder;
	m_decoder = NULL;
	LOGI("MusicPlay::Close()");
}
