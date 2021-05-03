// JHplay.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "iostream"
#include "Decodec.h"

#define __STDC_CONSTANT_MACROS
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};
#define RETSUCESS 0
class FfmpegDecodec;
int main()
{
	//初始化
	AVFormatContext	*pFormatCtx;
	int				videoIndex;//视频流的indexcode
	AVCodecContext	*pCodecCtx;
	
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	//获取解码器单例
	FfmpegDecodec* instanceDecodec = FfmpegDecodec::getInstance();
	char filepath[] = "Titanic.ts";//输入文件路径

	//1获取封装上下文
	instanceDecodec->format2stream(pFormatCtx, filepath, videoIndex);

	//2解封装到H264
	FILE* fp_H264 = nullptr;
	int ret = fopen_s(&fp_H264,"test1.h264", "wb+");
	if (RETSUCESS == ret)
	{
		//AVFormatContext	*pFormatCtxTmp = new AVFormatContext;
		//memcpy(pFormatCtxTmp, pFormatCtx, sizeof(AVFormatContext));
		//pFormatCtx只能av_read_frame一次？这里放开frame2YUV就不能解析同一个pFormatCtx
		instanceDecodec->packet2H264(pFormatCtx, fp_H264, videoIndex);
		//delete pFormatCtxTmp;
	}
	fclose(fp_H264);

	//3获取解码上下文
	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
	instanceDecodec->stream2frame(pFormatCtx, pCodecCtx);

	//4解码到yuv文件
	FILE* fp_yuv = nullptr;
	int retYuv = fopen_s(&fp_yuv,"test2.yuv", "wb+");
	if (RETSUCESS == retYuv)
	{
		instanceDecodec->frame2YUV(pFormatCtx, pCodecCtx, fp_yuv, videoIndex);
	}
	fclose(fp_yuv);

	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);//获取地址，不是引用
    return 0;
}

