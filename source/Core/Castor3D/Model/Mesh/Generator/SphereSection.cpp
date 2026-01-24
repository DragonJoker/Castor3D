#include "Castor3D/Model/Mesh/Generator/SphereSection.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

namespace c3d
{
	SphereSection::SphereSection()
		: MeshGenerator( cuT( "sphere_section" ) )
	{
	}

	MeshGeneratorUPtr SphereSection::create()
	{
		return makeUniqueDerived< MeshGenerator, SphereSection >();
	}

	void SphereSection::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		String param;
		float radius{};
		Angle angle{};
		uint32_t nbFaces{};

		if ( parameters.get( cuT( "subdiv" ), param ) )
		{
			nbFaces = string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			radius = string::toFloat( param );
		}

		if ( parameters.get( cuT( "angle" ), param ) )
		{
			angle = Angle::fromDegrees( string::toFloat( param ) );
		}

		if ( nbFaces >= 3
			&& angle.degrees() > 0.0f )
		{
			auto arcAngle = angle / ( 2.0f * float( nbFaces ) );
			Vector< Point2f > arc( nbFaces + 1u );
			Angle arcAlpha = 0.0_degrees;
			uint32_t iCur = 0;
			uint32_t iPrv = 0;
			float rAlphaI = 0;
			auto rAngle = PiMult2< float > / float( nbFaces );

			for ( uint32_t i = 0; i <= nbFaces; i++ )
			{
				arc[i]->x = float( radius * arcAlpha.sin() );
				arc[i]->y = float( radius * arcAlpha.cos() );
				arcAlpha += arcAngle;
			}

			// Constitution de la base sphérique
			Submesh & baseSubmesh = *mesh.createDefaultSubmesh();
			auto baseIndexMapping = baseSubmesh.createComponent< TriFaceMapping >();
			auto & baseIndexMappingData = baseIndexMapping->getData();

			for ( uint32_t k = 0; k < nbFaces; k++ )
			{
				Point2f ptT = arc[k + 0];
				Point2f ptB = arc[k + 1];

				if ( k == 0 )
				{
					// Calcul de la position des points du haut
					for ( uint32_t i = 0; i <= nbFaces; rAlphaI += rAngle, i++ )
					{
						auto rCos = float( cos( rAlphaI ) );
						auto rSin = float( sin( rAlphaI ) );
						auto pos = Point3f{ ptT->x * rCos, ptT->x * rSin, ptT->y };
						baseSubmesh.addPoint( InterleavedVertex{}
							.position( pos )
							.normal( point::getNormalised( pos ) )
							.texcoord( Point2f{ 0.5f, 0.5f } ) );
						iCur++;
					}
				}

				// Calcul de la position des points
				rAlphaI = 0;

				for ( uint32_t i = 0; i <= nbFaces; rAlphaI += rAngle, i++ )
				{
					auto rCos = float( cos( rAlphaI ) );
					auto rSin = float( sin( rAlphaI ) );
					auto pos = Point3f{ ptB->x * rCos, ptB->x * rSin, ptB->y };
					InterleavedVertex vertex{};
					vertex.position( pos )
						.normal( point::getNormalised( pos ) )
						.texcoord( Point2f{ 0.5 + ( float( k ) / float( nbFaces ) ) * rCos / 2.0f
							, 0.5 + ( float( k ) / float( nbFaces ) ) * rSin / 2.0f } );
					baseSubmesh.addPoint( vertex );
				}

				// Reconstition des faces de la base sphérique
				for ( uint32_t i = 0; i < nbFaces; i++ )
				{
					baseIndexMappingData.addFace( iCur + 0, iPrv + 0, iPrv + 1 );
					baseIndexMappingData.addFace( iCur + 1, iCur + 0, iPrv + 1 );
					iPrv++;
					iCur++;
				}

				iPrv++;
				iCur++;
			}

			baseIndexMapping->computeTangents();

			// Constitution des côtés
			Submesh & sideSubmesh = *mesh.createDefaultSubmesh();
			Point2f ptA = arc[nbFaces];
			rAlphaI = 0;

			// Calcul de la position des points des côtés
			for ( uint32_t i = 0; i <= nbFaces; rAlphaI += rAngle, i++ )
			{
				auto rCos = float( cos( rAlphaI ) );
				auto rSin = float( sin( rAlphaI ) );
				sideSubmesh.addPoint( InterleavedVertex{}
					.position( { ptA->x * rCos, ptA->x * rSin, ptA->y } )
					.texcoord( Point2f{ float( i ) / float( nbFaces ), float( 0.0 ) } ) );
				sideSubmesh.addPoint( InterleavedVertex{}
					.position( { 0.0f, 0.0f, 0.0f } )
					.texcoord( Point2f{ float( i ) / float( nbFaces ), float( 1.0 ) } ) );
				iCur++;
			}

			// Reconstition des faces des côtés
			auto sideIndexMapping = sideSubmesh.createComponent< TriFaceMapping >();
			auto & sideIndexMappingData = sideIndexMapping->getData();

			for ( uint32_t i = 0; i < 2 * nbFaces; i += 2 )
			{
				sideIndexMappingData.addFace( i + 1, i + 0, i + 2 );
			}

			sideIndexMapping->computeNormals();
			sideIndexMapping->computeTangents();
		}
	}
}
