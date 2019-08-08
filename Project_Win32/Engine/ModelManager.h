#ifndef __MODELMANAGER_H__
#define __MODELMANAGER_H__

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
	void Render(RenderTargetType RT_Type);
	void Init();
	void Loading();
	ModelManager();
	~ModelManager();
};
#endif // !__MODELMANAGER_H__