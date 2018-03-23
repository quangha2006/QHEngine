#include "AppContext.h"
#include "Logs.h"
AppContext * AppContext::instance = NULL;

void AppContext::SetWindowSize(int32_t width, int32_t height)
{
	this->width = width;
	this->height = height;
}

AppContext * AppContext::getInstance()
{
	if (instance == NULL)
		//instance = new AppContext();
		LOGE("instance is not init");
	return instance;
}

AppContext::AppContext()
{
}


AppContext::~AppContext()
{
}
