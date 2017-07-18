#include "Cone.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor;

namespace Castor3D
{
	Cone::Cone()
		: MeshGenerator( cuT( "cone" ) )
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

	void Cone::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String param;

		if ( p_parameters.Get( cuT( "faces" ), param ) )
		{
			m_nbFaces = string::to_uint( param );
		}

		if ( p_parameters.Get( cuT( "radius" ), param ) )
		{
			m_radius = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "height" ), param ) )
		{
			m_height = string::to_float( param );
		}

		if ( m_nbFaces >= 2 && m_height > std::numeric_limits< real >::epsilon() && m_radius > std::numeric_limits< real >::epsilon() )
		{
			Submesh & submeshBase	= *p_mesh.CreateSubmesh();
			Submesh & submeshSide	= *p_mesh.CreateSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real rCos, rSin;

			for ( real dAlphaI = 0; i <= m_nbFaces; dAlphaI += angleRotation )
			{
				rCos = cos( dAlphaI );
				rSin = sin( dAlphaI );

				if ( i < m_nbFaces )
				{
					Vertex::SetTexCoord( submeshBase.AddPoint( m_radius * rCos, 0.0, m_radius * rSin ), ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 );
				}

				Vertex::SetTexCoord( submeshSide.AddPoint( m_radius * rCos, 0.0, m_radius * rSin ), real( i ) / m_nbFaces, real( 1.0 ) );
				Vertex::SetTexCoord( submeshSide.AddPoint( real( 0 ), m_height, real( 0 ) ), real( i ) / m_nbFaces, real( 0.0 ) );
				i++;
			}

			BufferElementGroupSPtr ptBottomCenter	= submeshBase.AddPoint( 0.0, 0.0, 0.0 );
			Vertex( *ptBottomCenter ).SetTexCoord( 0.5, 0.5 );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extràmitàs
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				submeshBase.AddFace( submeshBase[i]->GetIndex(), submeshBase[i + 1]->GetIndex(), ptBottomCenter->GetIndex() );
			}

			//Composition du bas
			submeshBase.AddFace( submeshBase[m_nbFaces - 1]->GetIndex(), submeshBase[0]->GetIndex(), ptBottomCenter->GetIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				submeshSide.AddFace( submeshSide[i + 0]->GetIndex(), submeshSide[i + 1]->GetIndex(), submeshSide[i + 2]->GetIndex() );
			}

			ComputeNormals( p_mesh, true );
			Coords3r ptNormal0Top;
			Coords3r ptNormal0Base;
			Coords3r ptTangent0Top;
			Coords3r ptTangent0Base;
			Coords3r ptNormal1Top;
			Coords3r ptNormal1Base;
			Coords3r ptTangent1Top;
			Coords3r ptTangent1Base;
			Vertex::GetNormal( submeshSide[0], ptNormal0Top );
			Vertex::GetNormal( submeshSide[1], ptNormal0Base );
			Vertex::GetTangent( submeshSide[0], ptTangent0Top );
			Vertex::GetTangent( submeshSide[1], ptTangent0Base );
			ptNormal0Top += Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 2], ptNormal1Top );
			ptNormal0Base += Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 1], ptNormal1Base );
			ptTangent0Top += Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 2], ptTangent1Top );
			ptTangent0Base += Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 1], ptTangent1Base );
			point::normalise( ptNormal0Top );
			point::normalise( ptNormal0Base );
			point::normalise( ptTangent0Top );
			point::normalise( ptTangent0Base );
			Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 2], ptNormal0Top );
			Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 1], ptNormal0Base );
			Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 2], ptTangent0Top );
			Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 1], ptTangent0Base );
		}
	}
}
