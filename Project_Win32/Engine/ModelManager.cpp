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
		mListModel[i]->SyncPhysics();
		mListModel[i]->UpdateAnimation();
	}
}

void ModelManager::Render(RenderMode mode)
{
	for (int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Render(mode);
	}
}

void ModelManager::Init()
{
}

void ModelManager::Loading()
{
	for (int i = 0; i < mListModel.size(); i++)
	{
		mListModel[i]->Loading();
	}
}

ModelManager::ModelManager()
{
	mCurrentId = 0;
}


ModelManager::~ModelManager()
{
}
