#pragma once
#include "Model.h"
#include <vector>

class ModelManager
{
private:
	static ModelManager* instance;
	std::vector<Model*> mListModel;
	int mCurrentId;
public:
	static ModelManager* getInstance();
	void AddModel(Model* model);
	void RemoveModel(int id);
	void Update();
	void Render();
	void Init();
	ModelManager();
	~ModelManager();
};