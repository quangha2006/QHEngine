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

namespace Timer
{
#if defined(_WINDOWS)
	int64_t getMillisecond()
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		return ((ticks.QuadPart / double(GetTicksPerSecond())) * 1000);
	}

	void sleep(uint64_t sleep_time)
	{
		timeBeginPeriod(1); // Need add winmm.lib
		Sleep(sleep_time);
		timeEndPeriod(1);
	}

	std::string getCalendar(const char* format)
	{
		std::string currentTime_str;
		SYSTEMTIME time;
		GetLocalTime(&time);
		currentTime_str = Utils::toString(format, time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);//, time.wMilliseconds);
		
		return currentTime_str;
	}

	int64_t GetTicksPerSecond()
	{
		LARGE_INTEGER tickPerSeconds;
		QueryPerformanceFrequency(&tickPerSeconds);

		return tickPerSeconds.QuadPart;
	}

	int64_t GetTicks()
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		return ticks.QuadPart;
	}
#else //defined(_WINDOWS)
	int64_t getMillisecond()
	{
		timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		uint64_t nowNs = now.tv_sec*(int)1000 + round(now.tv_nsec / 1.0e6);
		return nowNs;
	}
	void sleep(uint64_t sleep_time)
	{
		usleep(sleep_time * 1000);
	}
	std::string getCalendar(const char* format)
	{
		std::string currentTime_str;

		timespec time;
		clock_gettime(CLOCK_REALTIME, &time);
		struct tm now;
		localtime_r(&(time.tv_sec), &now);
		currentTime_str = Utils::toString(format, now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec);

		return currentTime_str;
	}
	int64_t GetTicksPerSecond()
	{
		return 1000000;
	}
	int64_t GetTicks()
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);

		// Static So It'll Assign For The First Time.
		static struct timespec initTS = ts;

		// Convert Second And Nanosecond To Microseconds.
		return static_cast<int64_t>((double)(ts.tv_sec - initTS.tv_sec) * 1000000.0f + (double)(ts.tv_nsec - initTS.tv_nsec) / 1000.0f);
	}
#endif

}