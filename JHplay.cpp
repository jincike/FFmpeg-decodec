// JHplay.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "iostream"
#define __STDC_CONSTANT_MACROS  //c++要求的
extern "C"
{
#include "libavcodec/avcodec.h"
}

int main()
{
	std::cout << avcodec_configuration();
    return 0;
}

