/*
 * MusicDecoder.h
 *
 *  Created on: 2016年1月20日
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
	//视频处理消费者生产者
	sem_t semVideoEmpty; // 同步信号量， 当满了时阻止生产者放产品
	sem_t semVideoFull;   // 同步信号量， 当没产品时阻止消费者消费
	pthread_mutex_t mutexVideo;   // 互斥信号量， 一次只有一个线程访问缓冲
};

#endif /* MUSICDECODER_H_ */
