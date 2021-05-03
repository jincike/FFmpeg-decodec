#pragma once
//#include
/**
* 最简单的基于FFmpeg的解码器
* Simplest FFmpeg Decoder
* 本程序实现了视频文件的解码(支持HEVC，H.264，MPEG2等)。
* 是最简单的FFmpeg视频解码方面的教程。
* 通过学习本例子可以了解FFmpeg的解码流程。
* This software is a simplest video decoder based on FFmpeg.
* Suitable for beginner of FFmpeg.
*
*/
#include <stdio.h>
#include "iostream"

#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

class FfmpegDecodec
{
public:
	FfmpegDecodec();
	~FfmpegDecodec();
	static FfmpegDecodec* getInstance();
	
	//解封装
	int format2stream(AVFormatContext *pFormatCtx, char filepath[],int &videoIndex);
	//解码上下文
	int stream2frame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx);
    //写264文件
	int packet2H264(AVFormatContext *pFormatCtx, FILE *file, int vedioIndex);
	//解码packet写到YUV文件
	int frame2YUV(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx
		, FILE *file, int videoIndex);
	
protected:
private:
	static FfmpegDecodec* instance;
	//内类来释放单例，防止内存泄漏
	class delInstance {
	public:
		~delInstance() {
			if (nullptr != instance)
			{
				delete instance;
				instance = nullptr;
			}
		}
	};
	static delInstance gc;
};