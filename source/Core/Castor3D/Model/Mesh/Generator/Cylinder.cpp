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
	{
	}

	MeshGeneratorUPtr Cylinder::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Cylinder >();
	}

	void Cylinder::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;
		float height{};
		float radius{};
		uint32_t nbFaces{};

		if ( parameters.get( cuT( "faces" ), param ) )
		{
			nbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			radius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "height" ), param ) )
		{
			height = castor::string::toFloat( param );
		}

		if ( nbFaces >= 2 )
		{
			Submesh & submeshBase = *mesh.createDefaultSubmesh();
			auto indexMappingBase = submeshBase.createComponent< TriFaceMapping >();
			auto & indexMappingBaseData = indexMappingBase->getData();
			Submesh & submeshTop = *mesh.createDefaultSubmesh();
			auto indexMappingTop = submeshTop.createComponent< TriFaceMapping >();
			auto & indexMappingTopData = indexMappingTop->getData();
			Submesh & submeshSide = *mesh.createDefaultSubmesh();
			auto indexMappingSide = submeshSide.createComponent< TriFaceMapping >();
			auto & indexMappingSideData = indexMappingSide->getData();

			//CALCUL DE LA POSITION DES POINTS
			float angleRotation = castor::PiMult2< float > / float( nbFaces );
			auto rCosRot = float( cos( angleRotation ) );
			auto rSinRot = float( sin( angleRotation ) );
			float rCos = 1.0f;
			float rSin = 0.0f;
			float rSinT = 0.0f;
			InterleavedVertexArray topVertex;
			InterleavedVertexArray baseVertex;
			InterleavedVertexArray sideVertex;

			for ( uint32_t i = 0; i <= nbFaces; ++i )
			{
				if ( i < nbFaces )
				{
					baseVertex.emplace_back()
						.position( castor::Point3f{ radius * rCos, -height / 2, radius * rSin } )
						.normal( castor::Point3f{ 0.0, -1.0, 0.0 } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSin ) / 2 } );
					topVertex.emplace_back()
						.position( castor::Point3f{ radius * rCos, height / 2, radius * rSinT } )
						.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
						.texcoord( castor::Point2f{ ( 1 + rCos ) / 2, ( 1 + rSinT ) / 2 } );
				}

				sideVertex.emplace_back()
					.position( castor::Point3f{ radius * rCos, -height / 2, radius * rSin } )
					.normal( castor::Point3f{ -rCos, -rSin, 0.0 } )
					.texcoord( castor::Point2f{ float( 1.0 ) - float( i ) / float( nbFaces ), float( 0.0 ) } );
				sideVertex.emplace_back()
					.position( castor::Point3f{ radius * rCos, height / 2, radius * rSin } )
					.normal( castor::Point3f{ -rCos, -rSin, 0.0 } )
					.texcoord( castor::Point2f{ float( 1.0 ) - float( i ) / float( nbFaces ), float( 1.0 ) } );

				const float newCos = rCosRot * rCos - rSinRot * rSin;
				const float newSin = rSinRot * rCos + rCosRot * rSin;
				rCos = newCos;
				rSin = newSin;
				rSinT = -newSin;
			}

			auto topCenterIndex = uint32_t( topVertex.size() );
			auto bottomCenterIndex = uint32_t( baseVertex.size() );
			topVertex.emplace_back()
				.position( castor::Point3f{ 0.0, height / 2, 0.0 } )
				.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } );
			baseVertex.emplace_back()
				.position( castor::Point3f{ 0.0, -height / 2, 0.0 } )
				.normal( castor::Point3f{ 0.0, -1.0, 0.0 } )
				.texcoord( castor::Point2f{ 0.5, 0.5 } );

			submeshTop.addPoints( topVertex );
			submeshBase.addPoints( baseVertex );
			submeshSide.addPoints( sideVertex );

			//RECONSTITION DES FACES
			if ( height < 0 )
			{
				height = -height;
			}

			//Composition des extràmitàs
			for ( uint32_t i = 0; i < nbFaces - 1; i++ )
			{
				//Composition du bas
				indexMappingBaseData.addFace( i + 1, i, bottomCenterIndex );
				//Composition du dessus
				indexMappingTopData.addFace( i, topCenterIndex, i + 1 );
			}

			//Composition du bas
			indexMappingBaseData.addFace( 0, nbFaces - 1, bottomCenterIndex );
			//Composition du dessus
			indexMappingTopData.addFace( nbFaces - 1, topCenterIndex, 0 );

			//Composition des côtés
			for ( uint32_t i = 0; i < 2 * nbFaces; i += 2 )
			{
				indexMappingSideData.addFace( i + 1, i + 0, i + 2 );
				indexMappingSideData.addFace( i + 3, i + 1, i + 2 );
			}

			indexMappingBase->computeTangents();
			indexMappingTop->computeTangents();
			indexMappingSide->computeTangents();
		}
	}
}
