#include "FrameRate.h"
#include "Logs.h"
FrameRate * FrameRate::instance = NULL;

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

	if (total_deltatime + 2 * deltatime >= 1000)
	{
		float abc = 1000 - (total_deltatime + deltatime);
		if (deltatime != 0 && abc > 0.01f )
			currentFPS = framesPerSecond + abc / (float)deltatime;
		else
			currentFPS = framesPerSecond;
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
	//TextRendering::getInstance()->Draw("SleepTime: ", sleep_time, 400.0f, 5.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
	if (sleep_time > 0 && sleep_time < SKIP_TICKS) {
		Timer::sleep(sleep_time);
	}
	lasttimeframe = Timer::getMillisecond();
}

FrameRate::FrameRate()
{
	currentFPS = 60;
	SKIP_TICKS = 0;
}

FrameRate::~FrameRate()
{
}
