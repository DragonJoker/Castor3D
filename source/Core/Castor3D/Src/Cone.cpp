#include "Cone.hpp"
#include "Submesh.hpp"
#include "Vertex.hpp"

#include <Angle.hpp>

using namespace Castor;

namespace Castor3D
{
	Cone::Cone()
		:	MeshCategory( eMESH_TYPE_CONE )
		,	m_nbFaces( 0 )
		,	m_height( 0 )
		,	m_radius( 0 )
	{
	}

	Cone::~Cone()
	{
	}

	MeshCategorySPtr Cone::Create()
	{
		return std::make_shared< Cone >();
	}

	void Cone::Generate()
	{
		m_height = abs( m_height );
		m_radius = abs( m_radius );

		if ( m_nbFaces >= 2 && m_height > std::numeric_limits< real >::epsilon() && m_radius > std::numeric_limits< real >::epsilon() )
		{
			Submesh & l_submeshBase	= *GetMesh()->CreateSubmesh();
			Submesh & l_submeshSide	= *GetMesh()->CreateSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real l_rCos, l_rSin;

			for ( real dAlphaI = 0; i <= m_nbFaces; dAlphaI += angleRotation )
			{
				l_rCos = cos( dAlphaI );
				l_rSin = sin( dAlphaI );

				if ( i < m_nbFaces )
				{
					Vertex::SetTexCoord( l_submeshBase.AddPoint( m_radius * l_rCos, 0.0, m_radius * l_rSin ), ( 1 + l_rCos ) / 2, ( 1 + l_rSin ) / 2 );
				}

				Vertex::SetTexCoord( l_submeshSide.AddPoint( m_radius * l_rCos, 0.0, m_radius * l_rSin ), real( i ) / m_nbFaces, real( 1.0 ) );
				Vertex::SetTexCoord( l_submeshSide.AddPoint( real( 0 ), m_height, real( 0 ) ), real( i ) / m_nbFaces, real( 0.0 ) );
				i++;
			}

			FaceSPtr l_pFace;
			BufferElementGroupSPtr l_ptBottomCenter	= l_submeshBase.AddPoint( 0.0, 0.0, 0.0 );
			Vertex( *l_ptBottomCenter ).SetTexCoord( 0.5, 0.5 );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extr�mit�s
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				l_pFace = l_submeshBase.AddFace(	l_submeshBase[i]->GetIndex(), l_submeshBase[i + 1]->GetIndex(), l_ptBottomCenter->GetIndex() );
			}

			//Composition du bas
			l_pFace = l_submeshBase.AddFace(	l_submeshBase[m_nbFaces - 1]->GetIndex(), l_submeshBase[0]->GetIndex(), l_ptBottomCenter->GetIndex() );

			//Composition des c�t�s
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				l_pFace = l_submeshSide.AddFace(	l_submeshSide[i + 0]->GetIndex(), l_submeshSide[i + 1]->GetIndex(), l_submeshSide[i + 2]->GetIndex() );
			}

			ComputeNormals( true );
			Coords3r l_ptNormal0Top;
			Coords3r l_ptNormal0Base;
			Coords3r l_ptTangent0Top;
			Coords3r l_ptTangent0Base;
			Coords3r l_ptNormal1Top;
			Coords3r l_ptNormal1Base;
			Coords3r l_ptTangent1Top;
			Coords3r l_ptTangent1Base;
			Vertex::GetNormal(	l_submeshSide[0], l_ptNormal0Top );
			Vertex::GetNormal(	l_submeshSide[1], l_ptNormal0Base );
			Vertex::GetTangent(	l_submeshSide[0], l_ptTangent0Top );
			Vertex::GetTangent(	l_submeshSide[1], l_ptTangent0Base );
			l_ptNormal0Top		+= Vertex::GetNormal(	l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal1Top );
			l_ptNormal0Base		+= Vertex::GetNormal(	l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal1Base );
			l_ptTangent0Top		+= Vertex::GetTangent(	l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent1Top );
			l_ptTangent0Base	+= Vertex::GetTangent(	l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent1Base );
			point::normalise( l_ptNormal0Top );
			point::normalise( l_ptNormal0Base );
			point::normalise( l_ptTangent0Top );
			point::normalise( l_ptTangent0Base );
			Vertex::GetNormal(	l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal0Top );
			Vertex::GetNormal(	l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal0Base );
			Vertex::GetTangent(	l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent0Top );
			Vertex::GetTangent(	l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent0Base );
		}
	}

	void Cone::Initialise( UIntArray const & p_arrayFaces, RealArray const & p_arrayDimensions )
	{
		m_nbFaces = p_arrayFaces[0];
		m_radius = p_arrayDimensions[0];
		m_height = p_arrayDimensions[1];
		GetMesh()->Cleanup();
		Generate();
	}
}
