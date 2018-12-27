#pragma once
#include "Timer.h"
class FrameRate
{
private:
	float currentFPS;
	int prevFrametime;
	int64_t m_prevCputime;
	int64_t SKIP_TICKS;
	int64_t m_begincputime;
	int64_t m_endcputime;
	static FrameRate* instance;
	FrameRate();
	void limitFPS();
public:
	float GetFPS() { return currentFPS; }
	int GetPrevFrameTime();
	void BeginCpuTime();
	void EndCpuTime();
	int GetPrevCpuTime();
	void Counter();
	static FrameRate* getInstance();
	void setLimitFPS(unsigned int fps);
	~FrameRate();
};