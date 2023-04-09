#include "Castor3D/Model/Mesh/Generator/Cylinder.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, Cylinder )

namespace castor3d
{
	Cylinder::Cylinder()
		: MeshGenerator( cuT( "cylinder" ) )
		, m_height( 0 )
		, m_radius( 0 )
		, m_nbFaces( 0 )
	{
	}

	MeshGeneratorUPtr Cylinder::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Cylinder >();
	}

	void Cylinder::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;

		if ( parameters.get( cuT( "faces" ), param ) )
		{
			m_nbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			m_radius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			m_height = castor::string::toFloat( param );
		}

		if ( m_nbFaces >= 2 )
		{
			Submesh & submeshBase = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			auto indexMappingBase = submeshBase.createComponent< TriFaceMapping >();
			Submesh & submeshTop = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			auto indexMappingTop = submeshTop.createComponent< TriFaceMapping >();
			Submesh & submeshSide = *mesh.createSubmesh( SubmeshFlag::ePosNmlTanTex );
			auto indexMappingSide = submeshSide.createComponent< TriFaceMapping >();

			//CALCUL DE LA POSITION DES POINTS
			float angleRotation = castor::PiMult2< float > / float( m_nbFaces );
			auto rCosRot = float( cos( angleRotation ) );
			auto rSinRot = float( sin( angleRotation ) );
			float rCos = 1.0f;
			float rSin = 0.0f;
			float rSinT = 0.0f;
			InterleavedVertexArray topVertex;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;

			for ( uint32_t i = 0; i <= m_nbFaces; ++i )
			{
				if ( i < m_nbFaces )
				{
					baseVertex.push_back( InterleavedVertex{}
						.position( castor::Point3f{ m_radius * rCos, -m_height / 2, m_radius * rSin } )
						.normal( castor::Point3f{ 0.0, -1.0, 0.0 } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } ) );
					topVertex.push_back( InterleavedVertex{}
						.position( castor::Point3f{ m_radius * rCos, m_height / 2, m_radius * rSinT } )
						.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSinT ) / 2 } ) );
				}

				sideVertex.push_back( InterleavedVertex{}
					.position( castor::Point3f{ m_radius * rCos, -m_height / 2, m_radius * rSin } )
					.normal( castor::Point3f{ -rCos, -rSin, 0.0 } )
					.texcoord( castor::Point2f{ float( 1.0 ) - float( i ) / float( m_nbFaces ), float( 0.0 ) } ) );
				sideVertex.push_back( InterleavedVertex{}
					.position( castor::Point3f{ m_radius * rCos, m_height / 2, m_radius * rSin } )
					.normal( castor::Point3f{ -rCos, -rSin, 0.0 } )
					.texcoord( castor::Point2f{ float( 1.0 ) - float( i ) / float( m_nbFaces ), float( 1.0 ) } ) );

				const float newCos = rCosRot * rCos - rSinRot * rSin;
				const float newSin = rSinRot * rCos + rCosRot * rSin;
				rCos = newCos;
				rSin = newSin;
				rSinT = -newSin;
			}

			auto topCenterIndex = uint32_t( topVertex.size() );
			auto bottomCenterIndex = uint32_t( baseVertex.size() );
			topVertex.push_back( InterleavedVertex{}
				.position( castor::Point3f{ 0.0, m_height / 2, 0.0 } )
				.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } ) );
			baseVertex.push_back( InterleavedVertex{}
				.position( castor::Point3f{ 0.0, -m_height / 2, 0.0 } )
				.normal( castor::Point3f{ 0.0, -1.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } ) );

			submeshTop.addPoints( topVertex );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			if ( m_height < 0 )
			{
				m_height = -m_height;
			}

			//Composition des extràmitàs
			for ( uint32_t i = 0; i < m_nbFaces - 1; i++ )
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
			for ( uint32_t i = 0; i < 2 * m_nbFaces; i += 2 )
			{
				indexMappingSide->addFace( i + 1, i + 0, i + 2 );
				indexMappingSide->addFace( i + 3, i + 1, i + 2 );
			}

			indexMappingBase->computeTangents();
			indexMappingTop->computeTangents();
			indexMappingSide->computeTangents();
		}
	}
}
