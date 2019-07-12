#include "FrameRate.h"
#include "Logs.h"

FrameRate * FrameRate::instance = NULL;

int FrameRate::GetPrevFrameTime()
{
	return prevFrametime;
}

void FrameRate::BeginCpuTime()
{
	m_begincputime = Timer::getMillisecond();
}

void FrameRate::EndCpuTime()
{
	m_endcputime = Timer::getMillisecond();
	m_prevCputime = m_endcputime - m_begincputime;
}

int FrameRate::GetPrevCpuTime()
{
	return (int)m_prevCputime;
}

void FrameRate::Counter()
{
	static unsigned int framesPerSecond = 0;       // This will store our fps
	static uint64_t lastTime = Timer::getMillisecond();       // This will hold the time from the last frame
	static int total_deltatime = 0;
	if (SKIP_TICKS > 0)
	{
		limitFPS();
	}
	uint64_t currentTime = Timer::getMillisecond();

	int deltatime = (int)(currentTime - lastTime);

	prevFrametime = deltatime;

	if (total_deltatime + 2 * deltatime >= 1000)
	{
		int abc = 1000 - (total_deltatime + deltatime);
		if (deltatime != 0 && abc > 0 )
			currentFPS = framesPerSecond + (float)abc / (float)deltatime;
		else
			currentFPS = (float)framesPerSecond;
		total_deltatime = 0;
		framesPerSecond = 1;
	}
	else
	{
		total_deltatime += deltatime;
		framesPerSecond++;
	}
	lastTime = currentTime;
}

FrameRate * FrameRate::getInstance()
{
	if (instance == NULL)
	{
		instance = new FrameRate();
	}
	return instance;
}

void FrameRate::setLimitFPS(unsigned int fps)
{
	SKIP_TICKS = 1000 / fps;
}

void FrameRate::limitFPS()
{
	static int64_t lasttimeframe = Timer::getMillisecond();
	int64_t frame_time = Timer::getMillisecond() - lasttimeframe; // milisecond

	int64_t sleep_time = SKIP_TICKS - frame_time;

	if (sleep_time > 0 && sleep_time < SKIP_TICKS) {
		Timer::sleep(sleep_time);
	}
	lasttimeframe = Timer::getMillisecond();
}

FrameRate::FrameRate()
	: currentFPS(0.0f)
	, SKIP_TICKS(0)
	, prevFrametime(0)
	, m_begincputime(0)
	, m_endcputime(0)
	, m_prevCputime(0)
{
}

FrameRate::~FrameRate()
{
}
