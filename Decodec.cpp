#include "stdafx.h"
#include "Decodec.h"

FfmpegDecodec* FfmpegDecodec::instance;//初始化
FfmpegDecodec::FfmpegDecodec()
{
}
FfmpegDecodec::~FfmpegDecodec()
{
	//不要在析构函数中析构单例。单例应该由类的使用者释放而不是创造者
}
FfmpegDecodec* FfmpegDecodec::getInstance()
{
	if (nullptr == instance)
		instance = new(std::nothrow)FfmpegDecodec();
	return instance;
};
int FfmpegDecodec::format2stream(AVFormatContext *pFormatCtx, char filePath[],int &videoIndex)
{
	av_log(NULL,AV_LOG_INFO,"format2stream,begin...");
	if (avformat_open_input(&pFormatCtx, filePath, NULL, NULL) != 0) {
		av_log(NULL,AV_LOG_ERROR,"Couldn't open input stream.");
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}
	//videoindex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoIndex = i;
			break;
		}
	if (videoIndex == -1) {
		printf("Didn't find a video stream.\n");
		return -1;
	}
	return 0;
}
int FfmpegDecodec::stream2frame(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx)
{
	AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return -1;
	}
	return 0;
}
int FfmpegDecodec::packet2H264(AVFormatContext *pFormatCtx, FILE *file,int videoIndex)
{
	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoIndex) {
			/*
			* 在此处添加输出H264码流的代码
			* 取自于packet，使用fwrite()
			*/
			fwrite(packet->data, 1, packet->size, file);
		}
		av_free_packet(packet);
	}
	return 0;
}
int FfmpegDecodec::frame2YUV(AVFormatContext *pFormatCtx, AVCodecContext *pCodecCtx
	,FILE *file, int videoIndex)
{
	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	AVFrame *pFrame = av_frame_alloc();
	AVFrame *pFrameYUV = av_frame_alloc();
	uint8_t *out_buffer = (uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	
	struct SwsContext* img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	int frame_cnt = 0, got_picture = -1;
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoIndex) {

			int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				//去除右侧的黑色的图像，和硬件相关
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);
				printf("Decoded frame index: %d\n", frame_cnt);

				/*
				* 在此处添加输出YUV的代码
				* 取自于pFrameYUV，使用fwrite()
				*/
				//pFrameYUV的data由Y\U\V 三个分量组成的数组
				fwrite(pFrameYUV->data[0],1,pCodecCtx->width*pCodecCtx->height,file);//Y数据
				fwrite(pFrameYUV->data[1], 1, pCodecCtx->width*pCodecCtx->height/4, file);//Y数据
				fwrite(pFrameYUV->data[2], 1, pCodecCtx->width*pCodecCtx->height/4, file);//Y数据
				frame_cnt++;
			}
		}
	}
	av_free_packet(packet);
	sws_freeContext(img_convert_ctx);
	av_frame_free(&pFrameYUV);
	return 0;
}
