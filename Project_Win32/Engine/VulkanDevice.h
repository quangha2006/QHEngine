#pragma once
#include "IDevice.h"
namespace gpu
{
class VulkanDevice : public IDevice
{
public:
	VulkanDevice();
	~VulkanDevice();
};

}
