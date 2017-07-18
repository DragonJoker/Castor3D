#include "Cylinder.hpp"

#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Parameter.hpp"

using namespace Castor;

namespace Castor3D
{
	Cylinder::Cylinder()
		: MeshGenerator( cuT( "cylinder" ) )
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

		if ( m_nbFaces >= 2 )
		{
			Submesh & submeshBase	= *p_mesh.CreateSubmesh();
			Submesh & submeshTop	= *p_mesh.CreateSubmesh();
			Submesh & submeshSide	= *p_mesh.CreateSubmesh();
			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = real( Angle::PiMult2 / m_nbFaces );
			uint32_t i = 0;
			real rCosRot = cos( angleRotation );
			real rSinRot = sin( angleRotation );
			BufferElementGroupSPtr vertex;
			real rCos = real( 1 );
			real rSin = real( 0 );
			real rSinT = real( 0 );

			for ( uint32_t i = 0; i <= m_nbFaces; ++i )
			{
				if ( i < m_nbFaces )
				{
					vertex = submeshBase.AddPoint(	m_radius * rCos,	-m_height / 2,	m_radius * rSin );
					Vertex::SetNormal( vertex, 0.0, -1.0, 0.0 );
					Vertex::SetTexCoord( vertex,	( 1 + rCos ) / 2,	( 1 + rSin ) / 2 );
					vertex = submeshTop.AddPoint(	m_radius * rCos,	 m_height / 2,	m_radius * rSinT );
					Vertex::SetNormal( vertex, 0.0, 1.0, 0.0 );
					Vertex::SetTexCoord( vertex,	( 1 + rCos ) / 2,	( 1 + rSinT ) / 2 );
				}

				vertex = submeshSide.AddPoint(	m_radius * rCos,	-m_height / 2,	m_radius * rSin );
				Vertex::SetTexCoord( vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 0.0 ) );
				Vertex::SetNormal( vertex, rCos, rSin, 0.0 );
				vertex = submeshSide.AddPoint(	m_radius * rCos,	 m_height / 2,	m_radius * rSin );
				Vertex::SetNormal( vertex, rCos, rSin, 0.0 );
				Vertex::SetTexCoord( vertex,	real( 1.0 ) - real( i ) / m_nbFaces,	real( 1.0 ) );

				const real newCos = rCosRot * rCos - rSinRot * rSin;
				const real newSin = rSinRot * rCos + rCosRot * rSin;
				rCos = newCos;
				rSin = newSin;
				rSinT = -newSin;
			}

			FaceSPtr pFace;
			BufferElementGroupSPtr ptTopCenter = submeshTop.AddPoint( 0.0, m_height / 2, 0.0 );
			BufferElementGroupSPtr ptBottomCenter = submeshBase.AddPoint( 0.0, -m_height / 2, 0.0 );
			Vertex::SetTexCoord( ptTopCenter, 0.5, 0.5 );
			Vertex::SetTexCoord( ptBottomCenter, 0.5, 0.5 );

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
				//Composition du dessus
				submeshTop.AddFace( ptTopCenter->GetIndex(), submeshTop[i]->GetIndex(), submeshTop[i + 1]->GetIndex() );
			}

			//Composition du bas
			submeshBase.AddFace( submeshBase[m_nbFaces - 1]->GetIndex(), submeshBase[0]->GetIndex(), ptBottomCenter->GetIndex() );
			//Composition du dessus
			submeshTop.AddFace( ptTopCenter->GetIndex(), submeshTop[m_nbFaces - 1]->GetIndex(), submeshTop[0]->GetIndex() );

			//Composition des càtàs
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				submeshSide.AddFace( submeshSide[i + 0]->GetIndex(), submeshSide[i + 1]->GetIndex(), submeshSide[i + 2]->GetIndex() );
				submeshSide.AddFace( submeshSide[i + 1]->GetIndex(), submeshSide[i + 3]->GetIndex(), submeshSide[i + 2]->GetIndex() );
			}

			submeshBase.ComputeTangentsFromNormals();
			submeshTop.ComputeTangentsFromNormals();
			submeshSide.ComputeTangentsFromNormals();
			//ComputeNormals( p_mesh );
			//Coords3r ptNormal0Top;
			//Coords3r ptNormal0Base;
			//Coords3r ptTangent0Top;
			//Coords3r ptTangent0Base;
			//Coords3r ptNormal1Top;
			//Coords3r ptNormal1Base;
			//Coords3r ptTangent1Top;
			//Coords3r ptTangent1Base;
			//Vertex::GetNormal( submeshSide[0], ptNormal0Top );
			//Vertex::GetNormal( submeshSide[1], ptNormal0Base );
			//Vertex::GetTangent( submeshSide[0], ptTangent0Top );
			//Vertex::GetTangent( submeshSide[1], ptTangent0Base );
			//l_ptNormal0Top += Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 2], ptNormal1Top );
			//l_ptNormal0Base += Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 1], ptNormal1Base );
			//l_ptTangent0Top += Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 2], ptTangent1Top );
			//l_ptTangent0Base += Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 1], ptTangent1Base );
			//point::normalise( ptNormal0Top );
			//point::normalise( ptNormal0Base );
			//point::normalise( ptTangent0Top );
			//point::normalise( ptTangent0Base );
			//Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 2], ptNormal0Top );
			//Vertex::GetNormal( submeshSide[submeshSide.GetPointsCount() - 1], ptNormal0Base );
			//Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 2], ptTangent0Top );
			//Vertex::GetTangent( submeshSide[submeshSide.GetPointsCount() - 1], ptTangent0Base );
		}

		p_mesh.ComputeContainers();
	}
}
