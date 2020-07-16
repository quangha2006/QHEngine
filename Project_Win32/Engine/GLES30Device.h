#pragma once
#include "IDevice.h"
namespace gpu
{
class GLES30Device : public IDevice
{
public:
	GLES30Device();
	~GLES30Device();
};
}
