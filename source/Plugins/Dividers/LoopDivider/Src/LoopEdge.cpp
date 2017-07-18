#include "LoopEdge.hpp"
#include "LoopVertex.hpp"
#include "LoopDivider.hpp"

using namespace Loop;
using namespace Castor;

//*********************************************************************************************

namespace Loop
{
	Edge::Edge( VertexSPtr p_v1, VertexSPtr p_v2, Castor3D::Face const & p_f1, bool p_toDivide )
		: m_firstVertex( p_v1 )
		, m_secondVertex( p_v2 )
		, m_firstFace( &p_f1 )
		, m_divided( false )
		, m_toDivide( p_toDivide )
		, m_toDelete( false )
		, m_secondFace( nullptr )
	{
	}

	Edge::~Edge()
	{
	}

	void Edge::AddFace( Castor3D::Face const & p_face )
	{
		if ( !m_firstFace )
		{
			m_firstFace = &p_face;
		}
		else if ( !m_secondFace )
		{
			m_secondFace = &p_face;
		}
	}

	VertexSPtr Edge::Divide( Subdivider * p_pDivider, real p_value )
	{
		VertexSPtr result;

		if ( m_toDivide )
		{
			if ( !m_divided )
			{
				// The edge is to divide and not divided yet, we create the new point
				Point3r ptPoint1;
				Point3r ptPoint2;
				Castor3D::Vertex::GetPosition( GetVertex1()->GetPoint(), ptPoint1 );
				ptPoint1 += Castor3D::Vertex::GetPosition( *GetVertex2()->GetPoint(), ptPoint2 );
				ptPoint1 *= p_value;
				m_createdVertex = p_pDivider->AddPoint( ptPoint1 );
				m_divided = true;
			}

			result = m_createdVertex;
		}

		return result;
	}
}

//*********************************************************************************************
