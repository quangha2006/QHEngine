#pragma once
#include <iostream>

namespace Timer
{
	int64_t getMillisecond();
	std::string getCalendar(const char* format = "%d-%d-%d %d:%d:%d");
	void sleep(uint64_t sleep_time);
	int64_t GetTicksPerSecond();
}