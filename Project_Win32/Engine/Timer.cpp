#include "stdafx.h"
#include "Timer.h"
#include "Utils.h"
#include <cstdarg>
#include <cassert>
#include <math.h>
#ifdef ANDROID
#include <time.h>
#include <unistd.h>
#endif
#ifdef _WINDOWS
#include <Windows.h>
#include <mmsystem.h>
#endif
//template< typename... Args >
//std::string string_sprintf(const char* format, Args... args) {
//	int length = std::snprintf(nullptr, 0, format, args...);
//	assert(length >= 0);
//
//	char* buf = new char[length + 1];
//	std::snprintf(buf, length + 1, format, args...);
//
//	std::string str(buf);
//	delete[] buf;
//	return std::move(str);
//}
namespace Timer
{
	int64_t getMillisecond()
	{
#if defined(ANDROID)
		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		uint64_t nowNs = now.tv_sec*(int)1000 + round(now.tv_nsec / 1.0e6);
		return nowNs;
#else
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		return ((ticks.QuadPart/ double(GetTicksPerSecond())) * 1000);
#endif
	}
	void sleep(uint64_t sleep_time)
	{
#if defined(ANDROID)
		usleep(sleep_time * 1000);
#else
		timeBeginPeriod(1); // Need add winmm.lib
		Sleep(sleep_time);
		timeEndPeriod(1);
#endif
	}
	std::string getCalendar(const char* format)
	{
		std::string currentTime_str;
#ifdef _WINDOWS
		SYSTEMTIME time;
		GetLocalTime(&time);
		currentTime_str = Utils::toString(format, time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);//, time.wMilliseconds);

#elif ANDROID
		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);
		struct tm now;
		localtime_r(&(time.tv_sec), &now);
		currentTime_str = Utils::toString(format, now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec);
#endif
		return currentTime_str;
	}
	int64_t GetTicksPerSecond()
	{ //maybe cache this value !
#ifdef ANDROID
		return 1000000;
#else
		LARGE_INTEGER tickPerSeconds;
		QueryPerformanceFrequency(&tickPerSeconds);

		return tickPerSeconds.QuadPart;
#endif
	}
	int64_t GetTicks()
	{
#ifdef OS_ANDROID
		struct timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts))
		{
			__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time initTSFAIL===================");
		}

		// Static So It'll Assign For The First Time.
		static struct timespec initTS = ts;
		int64_t temp = (double)((ts.tv_sec - initTS.tv_sec) * 1000000.0f) + (double)((ts.tv_nsec - initTS.tv_nsec) / 1000.0f);

		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time 0 initTS: %lld", initTS.tv_sec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time 0 ts: %lld", ts.tv_sec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time 1 initTS: %lld", initTS.tv_nsec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time 1 ts: %lld", ts.tv_nsec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time ts.tv_sec - initTS.tv_sec: %lld", ts.tv_sec - initTS.tv_sec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time ts.tv_nsec - initTS.tv_nsec: %lld", ts.tv_nsec - initTS.tv_nsec);
		__android_log_print(ANDROID_LOG_INFO, "HDPhuoc", "Time: %lld", temp);

		// Convert Second And Nanosecond To Microseconds.
		return static_cast<int64_t>((double)(ts.tv_sec - initTS.tv_sec) * 1000000.0f + (double)(ts.tv_nsec - initTS.tv_nsec) / 1000.0f);
#elif _WIN32
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		return ticks.QuadPart;
#endif
	}
}