#include "LoopEdge.hpp"
#include "LoopVertex.hpp"
#include "LoopDivider.hpp"

using namespace Loop;
using namespace Castor;

//*********************************************************************************************

namespace Loop
{
	Edge::Edge( VertexSPtr p_v1, VertexSPtr p_v2, Castor3D::FaceSPtr p_f1, bool p_toDivide )
		: m_firstVertex( p_v1 )
		, m_secondVertex( p_v2 )
		, m_firstFace( p_f1 )
		, m_divided( false )
		, m_toDivide( p_toDivide )
		, m_toDelete( false )
	{
	}

	Edge::~Edge()
	{
	}

	void Edge::AddFace( Castor3D::FaceSPtr p_face )
	{
		if ( m_firstFace.expired() )
		{
			m_firstFace = p_face;
		}
		else if ( m_secondFace.expired() )
		{
			m_secondFace = p_face;
		}
	}

	VertexSPtr Edge::Divide( Subdivider * p_pDivider, real p_value )
	{
		VertexSPtr l_return;

		if ( m_toDivide )
		{
			if ( !m_divided )
			{
				// The edge is to divide and not divided yet, we create the new point
				Point3r l_ptPoint1;
				Point3r l_ptPoint2;
				Castor3D::Vertex::GetPosition( GetVertex1()->GetPoint(), l_ptPoint1 );
				l_ptPoint1 += Castor3D::Vertex::GetPosition( *GetVertex2()->GetPoint(), l_ptPoint2 );
				l_ptPoint1 *= p_value;
				m_createdVertex = p_pDivider->AddPoint( l_ptPoint1 );
				m_divided = true;
			}

			l_return = m_createdVertex;
		}

		return l_return;
	}
}

//*********************************************************************************************
