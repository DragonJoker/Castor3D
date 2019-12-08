#include "LoopDivider/LoopEdge.hpp"
#include "LoopDivider/LoopVertex.hpp"
#include "LoopDivider/LoopDivider.hpp"

using namespace Loop;
using namespace castor;

//*********************************************************************************************

namespace Loop
{
	Edge::Edge( VertexSPtr v1
		, VertexSPtr v2
		, castor3d::Face const & f1
		, bool toDivide )
		: m_firstVertex( v1 )
		, m_secondVertex( v2 )
		, m_firstFace( &f1 )
		, m_divided( false )
		, m_toDivide( toDivide )
		, m_toDelete( false )
		, m_secondFace( nullptr )
	{
	}

	void Edge::addFace( castor3d::Face const & face )
	{
		if ( !m_firstFace )
		{
			m_firstFace = &face;
		}
		else if ( !m_secondFace )
		{
			m_secondFace = &face;
		}
	}

	VertexSPtr Edge::divide( Subdivider * divider
		, float value )
	{
		VertexSPtr result;

		if ( m_toDivide )
		{
			if ( !m_divided )
			{
				// The edge is to divide and not divided yet, we create the new point
				castor::Point3f point = getVertex1()->getPoint().pos;
				point += getVertex2()->getPoint().pos;
				point *= value;
				m_createdVertex = divider->addPoint( point );
				m_divided = true;
			}

			result = m_createdVertex;
		}

		return result;
	}
}

//*********************************************************************************************
