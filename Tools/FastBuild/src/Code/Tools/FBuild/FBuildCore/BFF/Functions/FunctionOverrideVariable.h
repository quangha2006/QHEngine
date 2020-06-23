#pragma once

#include "Function.h"


class FunctionOverrideVariable : public Function
{
public:
	explicit        FunctionOverrideVariable();
	inline virtual ~FunctionOverrideVariable() = default;

protected:
	virtual bool AcceptsHeader() const override;
	virtual bool NeedsHeader() const override;
	virtual bool NeedsBody() const override;

	virtual bool ParseFunction(NodeGraph & nodeGraph,
		const BFFIterator & functionNameStart,
		const BFFIterator * functionBodyStartToken,
		const BFFIterator * functionBodyStopToken,
		const BFFIterator * functionHeaderStartToken,
		const BFFIterator * functionHeaderStopToken) const override;
};