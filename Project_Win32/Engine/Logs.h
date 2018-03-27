#pragma once
//#include "stdafx.h"
#include <iostream>
#if defined(_WINDOWS)
#include <Windows.h>
#elif defined(ANDROID)
#include <android/log.h>
#endif

extern void PlatformLog(int type,const char* fmt, ...);

#if defined(_WINDOWS)
#define LOGI(...) PlatformLog(0, __VA_ARGS__)
#define LOGW(...) PlatformLog(1, __VA_ARGS__)
#define LOGE(...) PlatformLog(2, __VA_ARGS__)
#elif defined(ANDROID)
#define LOG_TAG "QHEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif