#pragma once

#include "Core/Containers/Array.h"

#if defined(__OSX__)
	#include "Core/Strings/AString.h"
#endif

class NodeGraph;
class Node;

struct ResultedDependency
{
	Node* m_Node = nullptr;
	NodeGraph* m_NodeGraph = nullptr;


	bool IsValid()
	{
		return m_Node != nullptr && m_NodeGraph != nullptr;
	}
};

struct DependencyBFFInformation
{
	AString m_BFFFilePath;
	size_t m_TimeStamp;
	size_t m_DataHash;

	DependencyBFFInformation(const char* path, size_t timestamp, size_t datahash)
	{
		m_BFFFilePath = path;
		m_TimeStamp = timestamp;
		m_DataHash = datahash;
	}
};

enum DependencySaveStatus
{
	DEPENDENCY_UNDEFINED = -999,
	DEPENDENCY_FAILED_TO_READ = -3,
	DEPENDENCY_FAILED_TO_OPEN = -2,
	DEPENDENCY_FAILED_TO_SAVE = -1,
	DEPENDENCY_SAVED = 0,
	DEPENDENCY_UPTODATE = 1,
};

enum MissingDependcyStatus
{
	MISSING_DEPENDENCY_UNDEFINED = -999,
	MISSING_DEPENDENCY_ERROR = -1,
	MISSING_DEPENDENCY_REPARSED = 0,
	MISSING_DEPENDENCY_UP_TO_DATE = 1,
};

class DependencyManager
{
public:
	DependencyManager() = default;
	virtual ~DependencyManager();
	
	void RegisterCommonNode(Node* node);
	void BuildDependency(Node* node);

	ResultedDependency GetNodeFromBuildDep(const char* nodeName);
	void SaveToBuildDependencies(NodeGraph* db);
	DependencySaveStatus SaveToBuildDependencies(const char* bffFile);
	MissingDependcyStatus InitializeMissingDepencies(const char* bffFile);

	const Array<DependencyBFFInformation>& GetDependiciesFileInfo() { return m_DependeciesInfo; }
private:
	class CommonNode
	{
	public:
		Node* m_Node;
		AString m_Name;
	};

	NodeGraph*							m_SolutionNodeGraph = nullptr;
	Array<CommonNode>					m_CommonNodes;
	Array<NodeGraph*>					m_ToBuildDependencies;
	Array<DependencyBFFInformation>		m_DependeciesInfo;

	void RegisterDependencyFileInfo(const char* path, const size_t timestamp, const size_t datahash);

};