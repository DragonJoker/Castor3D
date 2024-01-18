#include "Castor3D/Model/Mesh/Generator/Torus.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, Torus )

namespace castor3d
{
	Torus::Torus()
		: MeshGenerator( cuT( "torus" ) )
	{
	}

	MeshGeneratorUPtr Torus::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Torus >();
	}

	void Torus::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		castor::String param;
		float internalRadius{};
		float externalRadius{};
		uint32_t internalNbFaces{};
		uint32_t externalNbFaces{};

		if ( parameters.get( cuT( "inner_size" ), param ) )
		{
			internalRadius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "outer_size" ), param ) )
		{
			externalRadius = castor::string::toFloat( param );
		}

		if ( parameters.get( cuT( "inner_count" ), param ) )
		{
			internalNbFaces = castor::string::toUInt( param );
		}

		if ( parameters.get( cuT( "outer_count" ), param ) )
		{
			externalNbFaces = castor::string::toUInt( param );
		}

		mesh.cleanup();

		if ( internalNbFaces >= 3 && externalNbFaces >= 3 )
		{
			Submesh & submesh = *( mesh.createDefaultSubmesh() );
			uint32_t uiCur = 0;
			uint32_t uiPrv = 0;
			uint32_t uiPCr = 0;
			uint32_t uiPPr = 0;
			float rAngleIn = 0.0;
			float rAngleEx = 0.0;
			uint32_t uiExtMax = externalNbFaces;
			uint32_t uiIntMax = internalNbFaces;

			// Build the internal circle that will be rotated to build the torus
			float step = castor::PiMult2< float > / float( internalNbFaces );

			for ( uint32_t j = 0; j <= uiIntMax; j++ )
			{
				submesh.addPoint( InterleavedVertex{}
					.position( castor::Point3f{ internalRadius * cos( rAngleIn ) + externalRadius, internalRadius * sin( rAngleIn ), 0.0 } )
					.normal( castor::point::getNormalised( castor::Point3f{ float( cos( rAngleIn ) ), float( sin( rAngleIn ) ), 0.0f } ) )
					.texcoord( castor::Point2f{ 0.0f, float( j ) / float( internalNbFaces ) } ) );
				uiCur++;
				rAngleIn += step;
			}

			// Build the torus
			step = castor::PiMult2< float > / float( externalNbFaces );
			auto indexMapping = submesh.createComponent< TriFaceMapping >();
			auto & indexMappingData = indexMapping->getData();

			for ( uint32_t i = 1; i <= uiExtMax; i++ )
			{
				uiPCr = uiCur;
				uiPPr = uiPrv;
				rAngleEx += step;

				for ( uint32_t j = 0; j <= uiIntMax; j++ )
				{
					auto vertex = submesh.getInterleavedPoint( j );
					vertex.pos = castor::Point3f{ vertex.pos[0] * cos( rAngleEx ), vertex.pos[1], vertex.pos[0] * sin( rAngleEx ) };
					vertex.tex = castor::Point3f{ float( i ) / float( externalNbFaces ), float( j ) / float( internalNbFaces ) };
					vertex.nml = castor::point::getNormalised( castor::Point3f( float( vertex.nml[0] * cos( rAngleEx ) ), vertex.nml[1], float( vertex.nml[0] * sin( rAngleEx ) ) ) );
					submesh.addPoint( vertex );
				}

				for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
				{
					indexMappingData.addFace( uiPrv + 0, uiCur + 1, uiPrv + 1 );
					indexMappingData.addFace( uiPrv + 0, uiCur + 0, uiCur + 1 );
					uiPrv++;
					uiCur++;
				}

				indexMappingData.addFace( uiPrv + 0, uiPCr + 0, uiPPr + 0 );
				indexMappingData.addFace( uiPrv + 0, uiCur + 0, uiPCr + 0 );
				uiPrv++;
				uiCur++;
			}

			uiCur = 0;
			uiPCr = uiCur;
			uiPPr = uiPrv;

			for ( uint32_t j = 0; j <= uiIntMax - 1; j++ )
			{
				indexMappingData.addFace( uiPrv + 0, uiCur + 1, uiPrv + 1 );
				indexMappingData.addFace( uiPrv + 0, uiCur + 0, uiCur + 1 );
				uiPrv++;
				uiCur++;
			}

			indexMappingData.addFace( uiPrv + 0, uiPCr + 0, uiPPr + 0 );
			indexMappingData.addFace( uiPrv + 0, uiCur + 0, uiPCr + 0 );

			indexMapping->computeTangents();
		}
	}
}
