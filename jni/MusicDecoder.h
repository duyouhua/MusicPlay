/*
 * MusicDecoder.h
 *
 *  Created on: 2016��1��20��
 *      Author: hail
 */

#ifndef MUSICDECODER_H_
#define MUSICDECODER_H_

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/log.h"
#include "libavutil/frame.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#include <string>
#include <queue>
#include <semaphore.h>
#include <pthread.h>
using namespace std;
struct AudioEncode
{
	AVSampleFormat sample_fmt;
	uint64_t sample_rate;
	int channel;
};
class Frame
{
public:
	Frame( void* pData,int nSize) :
		 data(pData),size(nSize)
	{

	}
	~Frame()
	{
	}
	int size;
	void* data;
};
#define MAX_AUDIO_FRAME_SIZE 192000
class MusicDecoder
{
public:
	MusicDecoder();
	~MusicDecoder();

	static bool Init();
	static void Destroy();
	static void* DecodeThread(void* data);
	bool Open(const char* path);
	void StartDecodeThread();
	Frame GetFrame();
	void Close();
private:
	bool ReadFileHeadInfo(const char* path);
	void Decode();
private:
	static AudioEncode m_androidAudioEncode;
	AVFormatContext* m_pFormatCtx;
	AVCodecContext* m_pAudioCodecCtx;
	AVCodec* m_pAudioCodec;
	int m_nAudioIndex;
	string m_strPath;
	queue<Frame> m_queueData;
	bool m_bDecoding;
	static const int MAX_BUFF_SIZE = 128;
	pthread_t m_tidDecode;
	//��Ƶ����������������
	sem_t semVideoEmpty; // ͬ���ź����� ������ʱ��ֹ�����߷Ų�Ʒ
	sem_t semVideoFull;   // ͬ���ź����� ��û��Ʒʱ��ֹ����������
	pthread_mutex_t mutexVideo;   // �����ź����� һ��ֻ��һ���̷߳��ʻ���
};

#endif /* MUSICDECODER_H_ */
