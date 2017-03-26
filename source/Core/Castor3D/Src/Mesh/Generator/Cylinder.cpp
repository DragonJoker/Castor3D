#include "Cylinder.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor;

namespace Castor3D
{
	Cylinder::Cylinder()
		: MeshGenerator( MeshType::eCylinder )
		, m_height( 0 )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	Cylinder::~Cylinder()
	{
	}

	MeshGeneratorSPtr Cylinder::Create()
	{
		return std::make_shared< Cylinder >();
	}

	void Cylinder::DoGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String l_param;

		if ( p_parameters.Get( cuT( "faces" ), l_param ) )
		{
			m_nbFaces = string::to_uint( l_param );
		}

		if ( p_parameters.Get( cuT( "radius" ), l_param ) )
		{
			m_radius = string::to_float( l_param );
		}

		if ( p_parameters.Get( cuT( "height" ), l_param ) )
		{
			m_height = string::to_float( l_param );
		}

		if ( m_nbFaces >= 2 )
		{
			Submesh & l_submeshBase	= *p_mesh.CreateSubmesh();
			Submesh & l_submeshTop	= *p_mesh.CreateSubmesh();
			Submesh & l_submeshSide	= *p_mesh.CreateSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real l_rCosRot = cos( angleRotation );
			real l_rSinRot = sin( angleRotation );
			BufferElementGroupSPtr l_vertex;
			real l_rCos = real( 1 );
			real l_rSin = real( 0 );
			real l_rSinT = real( 0 );

			for ( uint32_t i = 0; i <= m_nbFaces; ++i )
			{
				if ( i < m_nbFaces )
				{
					l_vertex = l_submeshBase.AddPoint(	m_radius * l_rCos,	-m_height / 2,	m_radius * l_rSin );
					Vertex::SetNormal( l_vertex, 0.0, -1.0, 0.0 );
					Vertex::SetTexCoord( l_vertex,	( 1 + l_rCos ) / 2,	( 1 + l_rSin ) / 2 );
					l_vertex = l_submeshTop.AddPoint(	m_radius * l_rCos,	 m_height / 2,	m_radius * l_rSinT );
					Vertex::SetNormal( l_vertex, 0.0, 1.0, 0.0 );
					Vertex::SetTexCoord( l_vertex,	( 1 + l_rCos ) / 2,	( 1 + l_rSinT ) / 2 );
				}

				l_vertex = l_submeshSide.AddPoint(	m_radius * l_rCos,	-m_height / 2,	m_radius * l_rSin );
				Vertex::SetTexCoord( l_vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 0.0 ) );
				Vertex::SetNormal( l_vertex, l_rCos, l_rSin, 0.0 );
				l_vertex = l_submeshSide.AddPoint(	m_radius * l_rCos,	 m_height / 2,	m_radius * l_rSin );
				Vertex::SetNormal( l_vertex, l_rCos, l_rSin, 0.0 );
				Vertex::SetTexCoord( l_vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 1.0 ) );

				const real l_newCos = l_rCosRot * l_rCos - l_rSinRot * l_rSin;
				const real l_newSin = l_rSinRot * l_rCos + l_rCosRot * l_rSin;
				l_rCos = l_newCos;
				l_rSin = l_newSin;
				l_rSinT = -l_newSin;
			}

			FaceSPtr l_pFace;
			BufferElementGroupSPtr l_ptTopCenter = l_submeshTop.AddPoint( 0.0, m_height / 2, 0.0 );
			BufferElementGroupSPtr l_ptBottomCenter = l_submeshBase.AddPoint( 0.0, -m_height / 2, 0.0 );
			Vertex::SetTexCoord( l_ptTopCenter, 0.5, 0.5 );
			Vertex::SetTexCoord( l_ptBottomCenter, 0.5, 0.5 );

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
				//Composition du dessus
				l_submeshTop.AddFace( l_ptTopCenter->GetIndex(), l_submeshTop[i]->GetIndex(), l_submeshTop[i + 1]->GetIndex() );
			}

			//Composition du bas
			l_submeshBase.AddFace( l_submeshBase[m_nbFaces - 1]->GetIndex(), l_submeshBase[0]->GetIndex(), l_ptBottomCenter->GetIndex() );
			//Composition du dessus
			l_submeshTop.AddFace( l_ptTopCenter->GetIndex(), l_submeshTop[m_nbFaces - 1]->GetIndex(), l_submeshTop[0]->GetIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				l_submeshSide.AddFace( l_submeshSide[i + 0]->GetIndex(), l_submeshSide[i + 1]->GetIndex(), l_submeshSide[i + 2]->GetIndex() );
				l_submeshSide.AddFace( l_submeshSide[i + 1]->GetIndex(), l_submeshSide[i + 3]->GetIndex(), l_submeshSide[i + 2]->GetIndex() );
			}

			l_submeshBase.ComputeTangentsFromNormals();
			l_submeshTop.ComputeTangentsFromNormals();
			l_submeshSide.ComputeTangentsFromNormals();
			//ComputeNormals( p_mesh );
			//Coords3r l_ptNormal0Top;
			//Coords3r l_ptNormal0Base;
			//Coords3r l_ptTangent0Top;
			//Coords3r l_ptTangent0Base;
			//Coords3r l_ptNormal1Top;
			//Coords3r l_ptNormal1Base;
			//Coords3r l_ptTangent1Top;
			//Coords3r l_ptTangent1Base;
			//Vertex::GetNormal( l_submeshSide[0], l_ptNormal0Top );
			//Vertex::GetNormal( l_submeshSide[1], l_ptNormal0Base );
			//Vertex::GetTangent( l_submeshSide[0], l_ptTangent0Top );
			//Vertex::GetTangent( l_submeshSide[1], l_ptTangent0Base );
			//l_ptNormal0Top += Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal1Top );
			//l_ptNormal0Base += Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal1Base );
			//l_ptTangent0Top += Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent1Top );
			//l_ptTangent0Base += Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent1Base );
			//point::normalise( l_ptNormal0Top );
			//point::normalise( l_ptNormal0Base );
			//point::normalise( l_ptTangent0Top );
			//point::normalise( l_ptTangent0Base );
			//Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptNormal0Top );
			//Vertex::GetNormal( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptNormal0Base );
			//Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 2], l_ptTangent0Top );
			//Vertex::GetTangent( l_submeshSide[l_submeshSide.GetPointsCount() - 1], l_ptTangent0Base );
		}

		p_mesh.ComputeContainers();
	}
}
