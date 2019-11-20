#include "ModelManager.h"
#include "Timer.h"
#include "RenderTarget.h"

ModelManager * ModelManager::instance = NULL;

ModelManager * ModelManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new ModelManager();
	}
	return instance;
}

int ModelManager::AddModel(Model * model)
{
	if (model)
	{
		mListModel.push_back(model);
		return mCurrentId++;
	}
	return -1;
}

void ModelManager::RemoveModel(int id)
{
	for (unsigned int i = 0; i < mListModel.size(); i++)
	{
		if (mListModel[i]->GetId() == id)
		{
			mListModel.erase(mListModel.begin() + i);
			return;
		}
	}
}

void ModelManager::Update()
{
	for (unsigned int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->UpdateAnimation();
		mListModel[i]->SyncPhysics();
	}
}

void ModelManager::Render(RenderTargetType RT_Type)
{
	for (unsigned int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Render(RT_Type);
	}
}

void ModelManager::Init()
{
}

void ModelManager::Loading()
{
	for (unsigned int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Loading();
	}
}

ModelManager::ModelManager()
	: mCurrentId(0)
{
}


ModelManager::~ModelManager()
{
}
