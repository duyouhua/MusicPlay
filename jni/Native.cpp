/*
 * Native.cpp
 *
 *  Created on: 2016Äê1ÔÂ19ÈÕ
 *      Author: hail
 */

#include "JNIUtil.h"
#include <android/bitmap.h>
#include "MusicPlay.h"
extern "C"
{
bool Java_com_hail_musicplay_jni_MusicPlay_init(JNIEnv* env, jobject obj);
bool Java_com_hail_musicplay_jni_MusicPlay_openFile(JNIEnv* env, jobject obj,
		jstring path);
void Java_com_hail_musicplay_jni_MusicPlay_play(JNIEnv* env, jobject obj);
void Java_com_hail_musicplay_jni_MusicPlay_pause(JNIEnv* env, jobject obj);
void Java_com_hail_musicplay_jni_MusicPlay_stop(JNIEnv* env, jobject obj);
void Java_com_hail_musicplay_jni_MusicPlay_setMute(JNIEnv* env, jobject obj,
		jboolean isMute);
void Java_com_hail_musicplay_jni_MusicPlay_destroy(JNIEnv* env, jobject obj);

static MusicPlay* g_player = NULL;
bool Java_com_hail_musicplay_jni_MusicPlay_init(JNIEnv* env, jobject obj)
{
	JavaVM* vm;
	env->GetJavaVM(&vm);
	JNIUtil::SetJavaVm(vm);
	MusicPlay::Init();
	g_player = MusicPlay::GetObject();
	return true;
}
bool Java_com_hail_musicplay_jni_MusicPlay_openFile(JNIEnv* env, jobject obj,
		jstring path)
{
	JNIUtil util;
	char* chPath = util.Jstring2String(path);
	if (g_player != NULL)
		g_player->Open(chPath);
	return true;
}
void Java_com_hail_musicplay_jni_MusicPlay_play(JNIEnv* env, jobject obj)
{
	if (g_player != NULL)
		g_player->Play();
}
void Java_com_hail_musicplay_jni_MusicPlay_pause(JNIEnv* env, jobject obj)
{
	if (g_player != NULL)
		g_player->Pause();
}
void Java_com_hail_musicplay_jni_MusicPlay_stop(JNIEnv* env, jobject obj)
{
	if (g_player != NULL)
		g_player->Stop();
}
void Java_com_hail_musicplay_jni_MusicPlay_setMute(JNIEnv* env, jobject obj,
		jboolean isMute)
{
	if (g_player != NULL)
		g_player->SetMute(isMute);
}
void Java_com_hail_musicplay_jni_MusicPlay_destroy(JNIEnv* env, jobject obj)
{
	MusicPlay::Destory();
}

}
