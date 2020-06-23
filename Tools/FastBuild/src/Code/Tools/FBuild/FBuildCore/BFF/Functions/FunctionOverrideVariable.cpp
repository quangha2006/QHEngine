// FunctionUsing
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "FunctionOverrideVariable.h"
#include "Tools/FBuild/FBuildCore/FLog.h"
#include "Tools/FBuild/FBuildCore/FBuild.h"
#include "Tools/FBuild/FBuildCore/BFF/BFFIterator.h"
#include "Tools/FBuild/FBuildCore/BFF/BFFParser.h"
#include "Tools/FBuild/FBuildCore/BFF/BFFStackFrame.h"

#include "Core/FileIO/PathUtils.h"
#include "Core/Mem/Mem.h"

//#ifdef _WINDOWS_
//#include "Shlwapi.h"
//#endif

// CONSTRUCTOR
//------------------------------------------------------------------------------
FunctionOverrideVariable::FunctionOverrideVariable()
	: Function("OverrideVariable")
{
}

// AcceptsHeader
//------------------------------------------------------------------------------
/*virtual*/ bool FunctionOverrideVariable::AcceptsHeader() const
{
	return true;
}

// NeedsHeader
//------------------------------------------------------------------------------
/*virtual*/ bool FunctionOverrideVariable::NeedsHeader() const
{
	return true;
}

// NeedsBody
//------------------------------------------------------------------------------
/*virtual*/ bool FunctionOverrideVariable::NeedsBody() const
{
	return false;
}

//------------------------------------------------------------------------------
/*virtual*/ bool FunctionOverrideVariable::ParseFunction(NodeGraph & /*nodeGraph*/,
	const BFFIterator & functionNameStart,
	const BFFIterator * functionBodyStartToken,
	const BFFIterator * functionBodyStopToken,
	const BFFIterator * functionHeaderStartToken,
	const BFFIterator * functionHeaderStopToken) const
{
	if (!BFFStackFrame::GetAcceptOverride())
	{
		return true;
	}

	(void)functionNameStart;
	(void)functionBodyStartToken;
	(void)functionBodyStopToken;

	if (functionHeaderStartToken && functionHeaderStopToken &&
		(functionHeaderStartToken->GetDistTo(*functionHeaderStopToken) >= 1))
	{
		BFFIterator start(*functionHeaderStartToken);
		ASSERT(*start == BFFParser::BFF_FUNCTION_ARGS_OPEN);
		start++;
		start.SkipWhiteSpace();

		// a variable name?
		const char c = *start;
		if (c != BFFParser::BFF_DECLARE_VAR_INTERNAL &&
			c != BFFParser::BFF_DECLARE_VAR_PARENT)
		{
			Error::Error_1007_ExpectedVariable(start, this);
			return false;
		}

		// we want 1 frame above this function
		BFFStackFrame * frame = BFFStackFrame::GetCurrent()->GetParent();
		ASSERT(frame);

		// find variable name
		BFFIterator stop(start);
		AStackString< BFFParser::MAX_VARIABLE_NAME_LENGTH > varName;
		bool parentScope = false;
		if (BFFParser::ParseVariableName(stop, varName, parentScope) == false)
		{
			return false;
		}

		ASSERT(stop.GetCurrent() <= functionHeaderStopToken->GetCurrent()); // should not be in this function if strings are not validly terminated

		// find variable
		const BFFVariable * v = nullptr;
		BFFStackFrame * const varFrame = (parentScope)
			? BFFStackFrame::GetParentDeclaration(varName, frame, v)
			: nullptr;

		if (false == parentScope)
		{
			v = BFFStackFrame::GetVar(varName, nullptr);
		}

		if ((parentScope && (nullptr == varFrame)) || (nullptr == v))
		{
			Error::Error_1009_UnknownVariable(start, this, varName);
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
