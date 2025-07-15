#include "Castor3D/Model/Mesh/Generator/Sphere.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"

#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

namespace c3d
{
	Sphere::Sphere()
		: MeshGenerator( cuT( "sphere" ) )
	{
	}

	MeshGeneratorUPtr Sphere::create()
	{
		return makeUniqueDerived< MeshGenerator, Sphere >();
	}

	void Sphere::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		String param;
		float radius{};
		uint32_t nbFaces{};

		if ( parameters.get( cuT( "subdiv" ), param ) )
		{
			nbFaces = string::toUInt( param );
		}

		if ( parameters.get( cuT( "radius" ), param ) )
		{
			radius = string::toFloat( param );
		}

		if ( nbFaces >= 3 )
		{
			Submesh & submesh = *mesh.createDefaultSubmesh();
			float rAngle = PiMult2< float > / float( nbFaces );
			Vector< Point2f > arc( nbFaces + 1u );
			float rAlpha = 0;
			uint32_t iCur = 0;
			uint32_t iPrv = 0;
			float rAlphaI = 0;

			for ( uint32_t i = 0; i <= nbFaces; i++ )
			{
				auto x = float( radius * sin( rAlpha ) );
				auto y = float( -radius * cos( rAlpha ) );
				arc[i][0] = x;
				arc[i][1] = y;
				rAlpha += rAngle / 2;
			}

			auto indexMapping = submesh.createComponent< TriFaceMapping >();
			auto & indexMappingData = indexMapping->getData();

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
						auto pos = Point3f{ ptT[0] * rCos, ptT[1], ptT[0] * rSin };
						submesh.addPoint( InterleavedVertex{}
							.position( pos )
							.normal( point::getNormalised( pos ) )
							.texcoord( Point2f{ float( i ) / float( nbFaces ), float( 1.0 + ptT[1] / radius ) / 2 } ) );
						iCur++;
					}
				}

				// Calcul de la position des points
				rAlphaI = 0;

				for ( uint32_t i = 0; i <= nbFaces; rAlphaI += rAngle, i++ )
				{
					auto rCos = float( cos( rAlphaI ) );
					auto rSin = float( sin( rAlphaI ) );
					auto pos = Point3f{ ptB[0] * rCos, ptB[1], ptB[0] * rSin };
					submesh.addPoint( InterleavedVertex{}
						.position( pos )
						.normal( point::getNormalised( pos ) )
						.texcoord( Point2f{ float( i ) / float( nbFaces ), float( 1.0 + ptB[1] / radius ) / 2 } ) );
				}

				// Reconstition des faces
				for ( uint32_t i = 0; i < nbFaces; i++ )
				{
					indexMappingData.addFace( iCur + 0, iPrv + 0, iPrv + 1 );
					indexMappingData.addFace( iCur + 1, iCur + 0, iPrv + 1 );
					iPrv++;
					iCur++;
				}

				iPrv++;
				iCur++;
			}

			indexMapping->computeTangents();
		}
	}
}
