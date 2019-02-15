#include "Castor3D/Mesh/Generator/Cylinder.hpp"

#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Mesh/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

using namespace castor;

namespace castor3d
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

	MeshGeneratorSPtr Cylinder::create()
	{
		return std::make_shared< Cylinder >();
	}

	void Cylinder::doGenerate( Mesh & p_mesh, Parameters const & p_parameters )
	{
		String param;

		if ( p_parameters.get( cuT( "faces" ), param ) )
		{
			m_nbFaces = string::toUInt( param );
		}

		if ( p_parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = string::toFloat( param );
		}

		if ( p_parameters.get( cuT( "height" ), param ) )
		{
			m_height = string::toFloat( param );
		}

		if ( m_nbFaces >= 2 )
		{
			Submesh & submeshBase = *p_mesh.createSubmesh();
			Submesh & submeshTop = *p_mesh.createSubmesh();
			Submesh & submeshSide = *p_mesh.createSubmesh();
			auto indexMappingBase = std::make_shared< TriFaceMapping >( submeshBase );
			auto indexMappingTop = std::make_shared< TriFaceMapping >( submeshTop );
			auto indexMappingSide = std::make_shared< TriFaceMapping >( submeshSide );

			//CALCUL DE LA POSITION DES POINTS
			real angleRotation = PiMult2< real > / m_nbFaces;
			uint32_t i = 0;
			real rCosRot = cos( angleRotation );
			real rSinRot = sin( angleRotation );
			real rCos = 1.0_r;
			real rSin = 0.0_r;
			real rSinT = 0.0_r;
			InterleavedVertexArray topVertex;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;

			for ( uint32_t i = 0; i <= m_nbFaces; ++i )
			{
				if ( i < m_nbFaces )
				{
					baseVertex.push_back( InterleavedVertex::createPNT( Point3f{ m_radius * rCos, -m_height / 2, m_radius * rSin }
						, Point3f{ 0.0, -1.0, 0.0 }
						, Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } ) );
					topVertex.push_back( InterleavedVertex::createPNT( Point3f{ m_radius * rCos, m_height / 2, m_radius * rSinT }
						, Point3f{ 0.0, 1.0, 0.0 }
						, Point2f{ ( 1 + rCos ) / 2, ( 1 + rSinT ) / 2 } ) );
				}

				sideVertex.push_back( InterleavedVertex::createPNT( Point3f{ m_radius * rCos, -m_height / 2, m_radius * rSin }
					, Point3f{ -rCos, -rSin, 0.0 }
					, Point2f{ real( 1.0 ) - real( i ) / m_nbFaces, real( 0.0 ) } ) );
				sideVertex.push_back( InterleavedVertex::createPNT( Point3f{ m_radius * rCos, m_height / 2, m_radius * rSin }
					, Point3f{ -rCos, -rSin, 0.0 }
					, Point2f{ real( 1.0 ) - real( i ) / m_nbFaces, real( 1.0 ) } ) );

				const real newCos = rCosRot * rCos - rSinRot * rSin;
				const real newSin = rSinRot * rCos + rCosRot * rSin;
				rCos = newCos;
				rSin = newSin;
				rSinT = -newSin;
			}

			auto topCenterIndex = uint32_t( topVertex.size() );
			auto bottomCenterIndex = uint32_t( baseVertex.size() );
			FaceSPtr pFace;
			topVertex.push_back( InterleavedVertex::createPNT( Point3f{ 0.0, m_height / 2, 0.0 }
				, Point3f{ 0.0, 1.0, 0.0 }
				, Point2f{ 0.5, 0.5 } ) );
			baseVertex.push_back( InterleavedVertex::createPNT( Point3f{ 0.0, -m_height / 2, 0.0 }
				, Point3f{ 0.0, -1.0, 0.0 }
				, Point2f{ 0.5, 0.5 } ) );

			submeshTop.addPoints( topVertex );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extràmitàs
			for ( i = 0; i < m_nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBase->addFace( i + 1, i, bottomCenterIndex );
				//Composition du dessus
				indexMappingTop->addFace( i, topCenterIndex, i + 1 );
			}

			//Composition du bas
			indexMappingBase->addFace( 0, m_nbFaces - 1, bottomCenterIndex );
			//Composition du dessus
			indexMappingTop->addFace( m_nbFaces - 1, topCenterIndex, 0 );

			//Composition des côtés
			for ( i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				indexMappingSide->addFace( i + 1, i + 0, i + 2 );
				indexMappingSide->addFace( i + 3, i + 1, i + 2 );
			}

			indexMappingBase->computeTangentsFromNormals();
			indexMappingTop->computeTangentsFromNormals();
			indexMappingSide->computeTangentsFromNormals();

			submeshBase.setIndexMapping( indexMappingBase );
			submeshTop.setIndexMapping( indexMappingTop );
			submeshSide.setIndexMapping( indexMappingSide );
		}

		p_mesh.computeContainers();
	}
}
