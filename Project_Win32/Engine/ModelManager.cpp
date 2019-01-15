#include "ModelManager.h"


ModelManager * ModelManager::instance = NULL;

ModelManager * ModelManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new ModelManager();
	}
	return instance;
}

void ModelManager::AddModel(Model * model)
{
	if (model)
	{
		mListModel.push_back(model);
		model->SetId(mCurrentId++);
	}
}

void ModelManager::RemoveModel(int id)
{
	for (int i = 0; i < mListModel.size(); i++)
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
	for (int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Update();
	}
}

void ModelManager::Render()
{
	for (int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Render();
	}
}

void ModelManager::Init()
{
}

ModelManager::ModelManager()
{
	mCurrentId = 0;
}


ModelManager::~ModelManager()
{
}
