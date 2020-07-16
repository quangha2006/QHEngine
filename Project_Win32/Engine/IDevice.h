#pragma once
#include <stdlib.h>
#include <stdint.h>

namespace gpu
{
class IDevice
{
public:
	virtual void InitWindows(int32_t width, int32_t height) {};
	virtual void SwapInterval(int interval) {};
	virtual void SwapBuffers() {}
	IDevice();
	~IDevice();
};
}