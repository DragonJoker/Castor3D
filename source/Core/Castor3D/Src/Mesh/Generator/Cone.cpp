#include "Cone.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"

#include <Math/Angle.hpp>

using namespace Castor;

namespace Castor3D
{
	Cone::Cone()
		: MeshGenerator( eMESH_TYPE_CONE )
		, m_nbFaces( 0 )
		, m_height( 0 )
		, m_radius( 0 )
	{
	}

	Cone::~Cone()
	{
	}

	MeshGeneratorSPtr Cone::Create()
	{
		return std::make_shared< Cone >();
	}

	void Cone::DoGenerate( Mesh & p_mesh, UIntArray const & p_faces, RealArray const & p_dimensions )
	{
		m_nbFaces = p_faces[0];
		m_radius = std::abs( p_dimensions[0] );
		m_height = std::abs( p_dimensions[1] );

		if ( m_nbFaces >= 2 && m_height > std::numeric_limits< real >::epsilon() && m_radius > std::numeric_limits< real >::epsilon() )
		{
			Submesh & l_submeshBase	= *p_mesh.CreateSubmesh();
			Submesh & l_submeshSide	= *p_mesh.CreateSubmesh();
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

			BufferElementGroupSPtr l_ptBottomCenter	= l_submeshBase.AddPoint( 0.0, 0.0, 0.0 );
			Vertex( *l_ptBottomCenter ).SetTexCoord( 0.5, 0.5 );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extràmitàs
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				l_submeshBase.AddFace( l_submeshBase[i]->GetIndex(), l_submeshBase[i + 1]->GetIndex(), l_ptBottomCenter->GetIndex() );
			}

			//Composition du bas
			l_submeshBase.AddFace( l_submeshBase[m_nbFaces - 1]->GetIndex(), l_submeshBase[0]->GetIndex(), l_ptBottomCenter->GetIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				l_submeshSide.AddFace( l_submeshSide[i + 0]->GetIndex(), l_submeshSide[i + 1]->GetIndex(), l_submeshSide[i + 2]->GetIndex() );
			}

			ComputeNormals( p_mesh, true );
			Coords3r l_ptNormal0Top;
			Coords3r l_ptNormal0Base;
			Coords3r l_ptTangent0Top;
			Coords3r l_ptTangent0Base;
			Coords3r l_ptNormal1Top;
			Coords3r l_ptNormal1Base;
			Coords3r l_ptTangent1Top;
			Coords3r l_ptTangent1Base;
			Vertex::GetNormal( l_submeshSide[0], l_ptNormal0Top );
			Vertex::GetNormal( l_submeshSide[1], l_ptNormal0Base );
			Vertex::GetTangent( l_submeshSide[0], l_ptTangent0Top );
			Vertex::GetTangent( l_submeshSide[1], l_ptTangent0Base );
			l_ptNormal0Top += Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal1Top );
			l_ptNormal0Base += Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal1Base );
			l_ptTangent0Top += Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent1Top );
			l_ptTangent0Base += Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent1Base );
			point::normalise( l_ptNormal0Top );
			point::normalise( l_ptNormal0Base );
			point::normalise( l_ptTangent0Top );
			point::normalise( l_ptTangent0Base );
			Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal0Top );
			Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal0Base );
			Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent0Top );
			Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent0Base );
		}
	}
}
