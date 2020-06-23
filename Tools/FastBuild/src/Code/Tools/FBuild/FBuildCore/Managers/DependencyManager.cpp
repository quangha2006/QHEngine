#include "DependencyManager.h"

#include "Tools/FBuild/FBuildCore/BFF/BFFParser.h"
#include "Tools/FBuild/FBuildCore/Graph/NodeGraph.h"
#include "Tools/FBuild/FBuildCore/BFF/BFFStackFrame.h"
#include "Tools/FBuild/FBuildCore/Graph/ObjectNode.h"
#include "Tools/FBuild/FBuildCore/FLog.h"

#include "Core/Math/xxHash.h"
#include "Core/FileIO/PathUtils.h"

#if defined(__OSX__)
#include "Core/FileIO/FileStream.h"
#include "Core/FileIO/FileIO.h"
//system
#include "string.h" // added to use strlen for build on MAC OS
#endif

DependencyManager::~DependencyManager()
{

	Array<NodeGraph*>::Iter beg = m_ToBuildDependencies.Begin();
	Array<NodeGraph*>::Iter end = m_ToBuildDependencies.End();
	for (; beg < end; ++beg)
	{
		//remode 
		(*beg)->m_AllNodes.Clear();
		FDELETE(*beg);
	}

	if (m_SolutionNodeGraph)
	{
		FDELETE(m_SolutionNodeGraph);
		m_SolutionNodeGraph = nullptr;
	}

}

void DependencyManager::RegisterCommonNode(Node* node)
{
	bool duplicate = false;
	Array<CommonNode>::Iter beg = m_CommonNodes.Begin();
	Array<CommonNode>::Iter end = m_CommonNodes.End();
	{
		for (; beg < end; ++beg)
		{
			const CommonNode common = *beg;
			if (!common.m_Name.Compare(node->GetName()))
			{
				duplicate = true;
				return;
			}
		}
	}

	//this doesnt have ownership
	CommonNode cm;

	cm.m_Node = node;
	cm.m_Name = node->GetName();

	m_CommonNodes.Append(cm);

	Dependencies deps = node->GetStaticDependencies();
	for (Dependency dep : deps)
	{
		RegisterCommonNode(dep.GetNode());
	}
}


ResultedDependency DependencyManager::GetNodeFromBuildDep(const char* nodeName)
{
	Array<NodeGraph*>::Iter beg = m_ToBuildDependencies.Begin();
	Array<NodeGraph*>::Iter end = m_ToBuildDependencies.End();
	for (; beg < end; ++beg)
	{
		NodeGraph* ng = *beg;
		Node* node = nullptr;
		node = ng->FindNode(AString(nodeName));
		if (node)
		{
			ResultedDependency resd;
			resd.m_Node = node;
			resd.m_NodeGraph = ng;
			return resd;
		}
	}

	return ResultedDependency();
}

MissingDependcyStatus DependencyManager::InitializeMissingDepencies(const char* bff)
{
	if (!m_ToBuildDependencies.IsEmpty() && !m_SolutionNodeGraph)
	{
		for (NodeGraph* ng : m_ToBuildDependencies)
		{
			FDELETE(ng);
		}

		m_ToBuildDependencies.Clear();
	}

	if (!m_SolutionNodeGraph)
	{
		m_SolutionNodeGraph = FNEW(NodeGraph);
		if (!m_SolutionNodeGraph->ParseFromRoot(bff))
		{
			return MISSING_DEPENDENCY_ERROR;
		}

		return MISSING_DEPENDENCY_REPARSED;
	}
	return MISSING_DEPENDENCY_UP_TO_DATE;
}

void DependencyManager::SaveToBuildDependencies(NodeGraph* fdbFile)
{
	m_ToBuildDependencies.Append(fdbFile);
}


DependencySaveStatus DependencyManager::SaveToBuildDependencies(const char* bffFile)
{
	const AString FDBFile(bffFile);

	const char* prjFile = FDBFile.FindLast(NATIVE_SLASH) + 1;

	AString prj;
	prj.Append(prjFile, strlen(prjFile) - 4);

	AString replaceFDBPath(bffFile);
	AString replaceFDBFileName(prjFile);
	replaceFDBFileName.Replace(".bff", ".fdb");

	replaceFDBPath.Replace(prjFile, replaceFDBFileName.Get());

	NodeGraph * oldNG = FNEW(NodeGraph);
	NodeGraph::LoadResult res = oldNG->Load(replaceFDBPath.Get());
	oldNG->SetFDBPathFile(replaceFDBPath.Get());
	
	//FLOG_WARN("Loading Dependency %s with status %i", bffFile,res);

	FileStream bffStream;
	if (bffStream.Open(bffFile) == false)
	{
		// missing bff is a fatal problem
		FLOG_ERROR("Failed to open BFF '%s'", bffFile);
		return DEPENDENCY_FAILED_TO_OPEN;
	}
	const uint64_t BFFTimeStamp = FileIO::GetFileLastWriteTime(AStackString<>(bffFile));

	// read entire config into memory
	uint32_t size = (uint32_t)bffStream.GetFileSize();
	AutoPtr< char > data((char *)ALLOC(size + 1)); // extra byte for null character sentinel
	if (bffStream.Read(data.Get(), size) != size)
	{
		FLOG_ERROR("Error reading BFF '%s'", bffFile);
		return DEPENDENCY_FAILED_TO_READ;
	}

	const uint64_t BFFDataHash = xxHash::Calc64(data.Get(), size);

	RegisterDependencyFileInfo(bffFile, (size_t)BFFTimeStamp, (size_t)BFFDataHash);

	switch (res)
	{
		case NodeGraph::LoadResult::LOAD_ERROR:
		{
			return DEPENDENCY_FAILED_TO_SAVE;
		}	
		break;
		case NodeGraph::LoadResult::OK:
		{
			SaveToBuildDependencies(oldNG);
			return DEPENDENCY_UPTODATE;
		}
		break;
		case NodeGraph::LoadResult::MISSING_OR_INCOMPATIBLE:
		case NodeGraph::LoadResult::OK_BFF_NEEDS_REPARSING:
		{
			NodeGraph* newNG = FNEW(NodeGraph);

			//[TODO]We need a hard copy of these nodes
			for (auto n : m_CommonNodes)
			{
				newNG->AddNode(n.m_Node);
			}

			BFFParser bffParser(*newNG);
			data.Get()[size] = '\0'; // data passed to parser must be NULL terminated
			if (!bffParser.Parse(data.Get(), size, bffFile, BFFTimeStamp, BFFDataHash))
			{
				FDELETE(newNG);
				FDELETE(oldNG);
				return DEPENDENCY_FAILED_TO_SAVE;
			}

			// TODO: Migrate old DB info to new DB
			FDELETE(oldNG);

			newNG->SetFDBPathFile(replaceFDBPath.Get());
			m_ToBuildDependencies.Append(newNG);

#if defined( __WINDOWS__ )
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

			AStackString<> output;
			output += "Loading Dependency: ";
			output += bffFile;
			output += '\n';
			FLOG_BUILD_DIRECT(output.Get());
			SetConsoleTextAttribute(hConsole, 15);
#endif
			return DEPENDENCY_SAVED;
		}
		break;
		default:
			break;
	}

	return DEPENDENCY_UNDEFINED;
}

void DependencyManager::RegisterDependencyFileInfo(const char* path, const size_t timestamp, const size_t datahash)
{
	DependencyBFFInformation bffInfo(path, timestamp, datahash);
	m_DependeciesInfo.Append(bffInfo);
}