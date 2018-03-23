#pragma once
#include "Timer.h"
class FrameRate
{
private:
	float currentFPS;
	int64_t SKIP_TICKS;
	static FrameRate* instance;
	FrameRate();
	void limitFPS();
public:
	float GetFPS() { return currentFPS; }
	void Counter();
	static FrameRate* getInstance();
	void setLimitFPS(unsigned int fps);
	~FrameRate();
};