// Meta_Range
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "Core/PrecompiledHeader.h"
#include "Meta_Range.h"

// Reflection
//------------------------------------------------------------------------------
REFLECT_BEGIN( Meta_Range, IMetaData, MetaNone() )
REFLECT_END( Meta_Range )

// CONSTRUCTOR
//------------------------------------------------------------------------------
Meta_Range::Meta_Range()
: m_Min( 0 )
, m_Max( 1 )
{
}

// CONSTRUCTOR
//------------------------------------------------------------------------------
Meta_Range::Meta_Range( int32_t minValue, int32_t maxValue )
: m_Min( minValue )
, m_Max( maxValue )
{
}

// DESTRUCTOR
//------------------------------------------------------------------------------
Meta_Range::~Meta_Range()
{
}

//------------------------------------------------------------------------------
