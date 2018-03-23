#pragma once
#include <iostream>
#if defined(_WINDOWS)
#include <Windows.h>
#elif defined(ANDROID)
#include <android/log.h>
#endif

extern void PlatformLog(const char* fmt, ...);

#if defined(_WINDOWS)
#define LOGI(...) PlatformLog(__VA_ARGS__)
#define LOGW(...) PlatformLog(__VA_ARGS__)
#define LOGE(...) PlatformLog(__VA_ARGS__)
#elif defined(ANDROID)
#define LOG_TAG "QHEngine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif