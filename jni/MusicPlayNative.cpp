/*
 * AudioNative.cpp
 *
 *  Created on: 2015年10月16日
 *      Author: hail
 */

#include "JniUtil.h"
#include "MusicPlay.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"

bool Java_com_hail_musicplay_MainActivity_init(JNIEnv*env, jobject obj);
int Java_com_hail_musicplay_MainActivity_openAudioFile(JNIEnv*env,
		jobject obj, jstring filePath);
int Java_com_hail_musicplay_MainActivity_play(JNIEnv*env, jobject obj);
void Java_com_hail_musicplay_MainActivity_pause(JNIEnv*env, jobject obj);
void Java_com_hail_musicplay_MainActivity_stop(JNIEnv*env, jobject obj);
void Java_com_hail_musicplay_MainActivity_volumeMute(JNIEnv*env, jobject obj,
		jboolean enable);
void Java_com_hail_musicplay_MainActivity_closeAudioFile(JNIEnv*env,
		jobject obj);
void Java_com_hail_musicplay_MainActivity_destory(JNIEnv*env, jobject obj);
}
#define MAX_AUDIO_FRAME_SIZE 192000
AudioPlay &play = AudioPlay::GetObject();
AVFormatContext* formatCtx = NULL;
int audioIndex = -1;
AVCodecContext *audioCodecCtx = NULL;
AVCodec *audioCodec = NULL;
struct SwrContext *au_convert_ctx;

int android_sample_rate = 0;
int android_channels = 0;
uint64_t android_channel_layout = AV_CH_FRONT_CENTER;
AVSampleFormat android_sample_fmt = AV_SAMPLE_FMT_S16;
uint8_t* audioBuffer = NULL;
int audioBuffer_size = 0;

uint8_t* ReadAudioFrame()
{
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	AVFrame *AudioFrame = av_frame_alloc();
	uint64_t android_out_channel = AV_CH_FRONT_CENTER;
	uint8_t* outbuffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	while (av_read_frame(formatCtx, packet) >= 0)
	{
		if (packet->stream_index == audioIndex)
		{
			int AudioFinished = 0;
			int ret = avcodec_decode_audio4(audioCodecCtx, AudioFrame,
					&AudioFinished, packet);
			if (ret > 0 && AudioFinished)
			{
				int android_nb_samples = swr_convert(au_convert_ctx, &outbuffer,
						MAX_AUDIO_FRAME_SIZE,
						(const uint8_t **) AudioFrame->data,
						AudioFrame->nb_samples);
				audioBuffer_size = av_samples_get_buffer_size(NULL,
						android_channels, android_nb_samples,
						android_sample_fmt, 1);
				av_frame_free(&AudioFrame);
				av_free(packet);
				return outbuffer;
			}
		}
	}
	av_frame_free(&AudioFrame);
	av_free(packet);
	return NULL;
}
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	if (audioBuffer != NULL)
	{
		av_free(audioBuffer);
		audioBuffer = NULL;
	}
	audioBuffer = ReadAudioFrame();
	if (audioBuffer != NULL)
	{
		play.PlayBuff(audioBuffer, audioBuffer_size);
	}
	else
	{
		LOGI("play finish!");
	}
}
bool Java_com_hail_musicplay_MainActivity_init(JNIEnv*env, jobject obj)
{
	play.init();
	av_register_all();
	return true;
}
int Java_com_hail_musicplay_MainActivity_openAudioFile(JNIEnv*env,
		jobject obj, jstring filePath)
{
	LOGI("native play!");
	char* path = jstring2string(env, filePath);
	formatCtx = avformat_alloc_context();
	if (0 != avformat_open_input(&formatCtx, path, NULL, NULL))
	{
		LOGE("open %s fail!", path);
		return -1;
	}
	if (avformat_find_stream_info(formatCtx, NULL) < 0)
	{
		LOGE("Couldn't find stream information.\n");
		return -1;
	}
	for (int i = 0; i < formatCtx->nb_streams; i++)
		if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioIndex = i;
			break;
		}
	if (audioIndex == -1)
	{
		LOGE("Couldn't find a video stream.\n");
		return -1;
	}
	audioCodecCtx = formatCtx->streams[audioIndex]->codec;
	audioCodec = avcodec_find_decoder(audioCodecCtx->codec_id); //寻找解码器
	if (audioCodec < 0)
	{
		LOGE("Couldn't find Codec.\n");
		return -1;
	}
	if (avcodec_open2(audioCodecCtx, audioCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return -1;
	}
	LOGI("bit_rate = %d ", audioCodecCtx->bit_rate);
	LOGI("sample_rate = %d ", audioCodecCtx->sample_rate);
	LOGI("channels = %d ", audioCodecCtx->channels);
	LOGI("code_name = %s ", audioCodecCtx->codec->name);
	LOGI("block_align = %d", audioCodecCtx->block_align);
	LOGI("frame_size = %d", audioCodecCtx->frame_size);
	//初始化转码器
	android_channel_layout = AV_CH_FRONT_CENTER;
	android_sample_fmt = AV_SAMPLE_FMT_S16;
	android_sample_rate = audioCodecCtx->sample_rate;
	android_channels = av_get_channel_layout_nb_channels(
			android_channel_layout);
	int64_t in_channel_layout = av_get_default_channel_layout(
			audioCodecCtx->channels);
	//Swr
	au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx, android_channels,
			android_sample_fmt, android_sample_rate, in_channel_layout,
			audioCodecCtx->sample_fmt, audioCodecCtx->sample_rate, 0,
			NULL);
	swr_init(au_convert_ctx);
	LOGI("audioBuffer_size=%d", audioBuffer_size);
	play.createBufferQueueAudioPlayer(android_sample_rate, android_channels,
	SL_PCMSAMPLEFORMAT_FIXED_16, bqPlayerCallback);
}
int Java_com_hail_musicplay_MainActivity_play(JNIEnv*env, jobject obj)
{
	audioBuffer = ReadAudioFrame();
	if (audioBuffer != NULL)
	{
		play.PlayBuff(audioBuffer, audioBuffer_size);

	}
	return 0;
}
void Java_com_hail_musicplay_MainActivity_pause(JNIEnv*env, jobject obj)
{
	play.Pause();
}
void Java_com_hail_musicplay_MainActivity_stop(JNIEnv*env, jobject obj)
{
	play.Stop();
}

void Java_com_hail_musicplay_MainActivity_volumeMute(JNIEnv*env, jobject obj,
		jboolean enable)
{
	static bool state = enable;
	play.SetMute(state);
	state = !state;
}
void Java_com_hail_musicplay_MainActivity_closeAudioFile(JNIEnv*env,
		jobject obj)
{
}
void Java_com_hail_musicplay_MainActivity_destory(JNIEnv*env, jobject obj)
{

	play.destroy();
	avformat_free_context(formatCtx);
}
