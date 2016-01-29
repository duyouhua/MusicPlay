/*
 * MusicDecoder.cpp
 *
 *  Created on: 2016年1月20日
 *      Author: hail
 */

#include "MusicDecoder.h"
#include "JNIUtil.h"

AudioEncode MusicDecoder::m_androidAudioEncode;

MusicDecoder::MusicDecoder() :
		m_pFormatCtx(NULL), m_pAudioCodecCtx(NULL), m_pAudioCodec(NULL), m_nAudioIndex(
				-1), m_strPath(""), m_bDecoding(false), m_tidDecode(-1)
{

}
MusicDecoder::~MusicDecoder()
{
}

bool MusicDecoder::Init()
{
	av_register_all();
	m_androidAudioEncode.sample_fmt = AV_SAMPLE_FMT_S16;
	m_androidAudioEncode.sample_rate = 44100;
	m_androidAudioEncode.channel = av_get_channel_layout_nb_channels(
	AV_CH_FRONT_CENTER);
	LOGI("MusicDecoder::Init()");
	return true;
}
void MusicDecoder::Destroy()
{
	LOGI("MusicDecoder::Destroy()");
}
bool MusicDecoder::ReadFileHeadInfo(const char* path)
{
	m_pFormatCtx = avformat_alloc_context();
	if (0 != avformat_open_input(&m_pFormatCtx, path, NULL, NULL))
	{
		LOGE("open %s fail!", m_strPath.c_str());
		return false;
	}
	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		LOGE("Couldn't find stream information.\n");
		return false;
	}
	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
		if (m_pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_nAudioIndex = i;
			break;
		}
	if (m_nAudioIndex == -1)
	{
		LOGE("Couldn't find a video stream.\n");
		return false;
	}
	m_pAudioCodecCtx = m_pFormatCtx->streams[m_nAudioIndex]->codec;
	m_pAudioCodec = avcodec_find_decoder(m_pAudioCodecCtx->codec_id); //寻找解码器
	if (m_pAudioCodec < 0)
	{
		LOGE("Couldn't find Codec.\n");
		return false;
	}
	if (avcodec_open2(m_pAudioCodecCtx, m_pAudioCodec, NULL) < 0)
	{
		LOGE("Couldn't open codec.\n");
		return false;
	}

	LOGI("bit_rate = %d ", m_pAudioCodecCtx->bit_rate);
	LOGI("sample_rate = %d ", m_pAudioCodecCtx->sample_rate);
	LOGI("channels = %d ", m_pAudioCodecCtx->channels);
	LOGI("code_name = %s ", m_pAudioCodecCtx->codec->name);
	LOGI("frame_size = %d", m_pAudioCodecCtx->frame_size);
	LOGI("time_base.den=%d，time_base.num=%d", m_pAudioCodecCtx->time_base.den,
			m_pAudioCodecCtx->time_base.num);

	return true;
}
bool MusicDecoder::Open(const char* path)
{
	m_strPath = path;
	ReadFileHeadInfo(path);
	sem_init(&semVideoEmpty, 0, MAX_BUFF_SIZE);
	sem_init(&semVideoFull, 0, 0);
	pthread_mutex_init(&mutexVideo, NULL);
}
void* MusicDecoder::DecodeThread(void* data)
{
	MusicDecoder* decoder = (MusicDecoder*) data;
	decoder->Decode();
	pthread_exit(0);
}
void MusicDecoder::Decode()
{
	m_bDecoding = true;
	//Swr
	int64_t in_channel_layout = av_get_default_channel_layout(
			m_pAudioCodecCtx->channels);
	struct SwrContext* au_convert_ctx = swr_alloc();
	au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
			m_androidAudioEncode.channel, m_androidAudioEncode.sample_fmt,
			m_androidAudioEncode.sample_rate, in_channel_layout,
			m_pAudioCodecCtx->sample_fmt, m_pAudioCodecCtx->sample_rate, 0,
			NULL);
	swr_init(au_convert_ctx);
	AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
	AVFrame *pAudioFrame = av_frame_alloc();
	uint8_t* Audiobuffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
	bool bFinish = true;
	while (av_read_frame(m_pFormatCtx, packet) >= 0)
	{
		if (packet->stream_index == m_nAudioIndex)
		{
			if (!m_bDecoding)
			{
				bFinish = false;
				break;
			}
			int nAudioFinished = 0;
			int nRet = avcodec_decode_audio4(m_pAudioCodecCtx, pAudioFrame,
					&nAudioFinished, packet);
			if (nRet > 0 && nAudioFinished != 0)
			{
				LOGI("duration=%d,pts=%lld", packet->duration, packet->pts);
				int out_samples = swr_convert(au_convert_ctx, &Audiobuffer,
				MAX_AUDIO_FRAME_SIZE, (const uint8_t **) pAudioFrame->data,
						pAudioFrame->nb_samples);
				sem_wait(&semVideoEmpty);
				pthread_mutex_lock(&mutexVideo);
				int Audiobuffer_size = av_samples_get_buffer_size(NULL,
						m_androidAudioEncode.channel, out_samples,
						m_androidAudioEncode.sample_fmt, 1);
				uint8_t *tmp = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE * 2);
				memcpy(tmp, Audiobuffer, Audiobuffer_size);
				Frame frame(tmp, Audiobuffer_size);
				m_queueData.push(frame);
				tmp = NULL;
				pthread_mutex_unlock(&mutexVideo);
				sem_post(&semVideoFull);
			}
		}
	}
	av_free(Audiobuffer);
	av_frame_free(&pAudioFrame);
	av_free(packet);
	if (bFinish)
	{
		sem_wait(&semVideoEmpty);
		pthread_mutex_lock(&mutexVideo);
		Frame frame(NULL, -1);
		m_queueData.push(frame);
		pthread_mutex_unlock(&mutexVideo);
		sem_post(&semVideoFull);
		m_bDecoding = false;
		LOGI("decode finish!");
	}
	else
	{
		LOGI("decode stop!!");
	}

}
void MusicDecoder::StartDecodeThread()
{
	pthread_create(&m_tidDecode, NULL, DecodeThread, this);
}
Frame MusicDecoder::GetFrame()
{
	Frame f(NULL, -1);
	sem_wait(&semVideoFull);
	pthread_mutex_lock(&mutexVideo);
	f = m_queueData.front();
	m_queueData.pop();
	pthread_mutex_unlock(&mutexVideo);
	sem_post(&semVideoEmpty);
	return f;
}
void MusicDecoder::Close()
{
	m_bDecoding = false;
	sem_post(&semVideoFull);
	sem_post(&semVideoEmpty);
	void* ret = 0;
	if (m_tidDecode != -1)
		pthread_join(m_tidDecode, &ret);
	sem_destroy(&semVideoEmpty);
	sem_destroy(&semVideoFull);
	pthread_mutex_destroy(&mutexVideo);
	if (m_pFormatCtx != NULL)
	{
		avformat_close_input(&m_pFormatCtx);
		avformat_free_context(m_pFormatCtx);
		m_pFormatCtx = NULL;

		m_pAudioCodec = NULL;
		m_nAudioIndex = -1;
		m_strPath = "";
	}
	if (m_queueData.size() > 0)
	{
		LOGI("m_queueData.size()=%d", m_queueData.size());
		for (int i = 0; i < m_queueData.size(); i++)
		{
			Frame f = m_queueData.front();
			av_free(f.data);
			m_queueData.pop();
		}
	}
	LOGI("MusicDecoder::Close()");
}
