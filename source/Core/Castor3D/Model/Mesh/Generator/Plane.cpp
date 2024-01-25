#include "Castor3D/Model/Mesh/Generator/Plane.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

CU_ImplementSmartPtr( castor3d, Plane )

namespace castor3d
{
	Plane::Plane()
		: MeshGenerator{ cuT( "plane" ) }
	{
	}

	MeshGeneratorUPtr Plane::create()
	{
		return castor::makeUniqueDerived< MeshGenerator, Plane >();
	}

	void Plane::doGenerate( Mesh & mesh, Parameters const & parameters )
	{
		bool sortAroundCenter{};
		parameters.get( cuT( "sort_around_center" ), sortAroundCenter );
		bool tileUV{};
		parameters.get( cuT( "tile_uv" ), tileUV );
		bool flipYZ{};
		parameters.get( cuT( "flipYZ" ), flipYZ );

		castor::String param;
		uint32_t subDivisionsW{};
		if ( parameters.get( cuT( "width_subdiv" ), param ) )
		{
			subDivisionsW = castor::string::toUInt( param );
		}

		uint32_t subDivisionsD{};
		if ( parameters.get( cuT( "depth_subdiv" ), param ) )
		{
			subDivisionsD = castor::string::toUInt( param );
		}

		float width{};
		if ( parameters.get( cuT( "width" ), param ) )
		{
			width = castor::string::toFloat( param );
		}

		float depth{};
		if ( parameters.get( cuT( "depth" ), param ) )
		{
			depth = castor::string::toFloat( param );
		}

		++subDivisionsW;
		++subDivisionsD;
		uint32_t nbVertexW = subDivisionsW + 1;
		uint32_t nbVertexH = subDivisionsD + 1;
		float offsetW = ( sortAroundCenter 
			? 0.0f
			: -width / 2 );
		float offsetH = ( sortAroundCenter 
			? 0.0f
			: -depth / 2 );
		float gapW = width / float( subDivisionsW );
		float gapH = depth / float( subDivisionsD );
		auto submesh = mesh.createDefaultSubmesh();
		InterleavedVertexArray points;
		points.reserve( size_t( nbVertexW ) * nbVertexH );

		if ( flipYZ )
		{
			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					points.emplace_back()
						.position( castor::Point3f{ offsetW + ( float( i ) * gapW ), 0.0, offsetH + ( float( j ) * gapH ) } )
						.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
						.texcoord( castor::Point3f{ float( i ) * gapW / width, float( j ) * gapH / depth, 0.0f } );
				}
			}

			if ( sortAroundCenter )
			{
				std::sort( std::begin( points )
					, std::end( points )
					, [&]( InterleavedVertex a, InterleavedVertex b )
					{
							a.pos -= castor::Point3f{ float( nbVertexW ) / 2.0f, 0.0f, float( nbVertexH ) / 2.0f };
							b.pos -= castor::Point3f{ float( nbVertexW ) / 2.0f, 0.0f, float( nbVertexH ) / 2.0f };
							return castor::point::dot( a.pos, a.pos ) < castor::point::dot( b.pos, b.pos );
					} );
			}
		}
		else
		{
			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					points.emplace_back()
						.position( castor::Point3f{ offsetW + ( float( i ) * gapW ), offsetH + ( float( j ) * gapH ), 0.0 } )
						.normal( castor::Point3f{ 0.0, 0.0, 1.0 } )
						.texcoord( castor::Point3f{ float( i ) * gapW / width, float( j ) * gapH / depth, 0.0f } );
				}
			}

			if ( sortAroundCenter )
			{
				std::sort( std::begin( points )
					, std::end( points )
					, [&]( InterleavedVertex a, InterleavedVertex b )
					{
							a.pos -= castor::Point3f{ float( nbVertexW ) / 2.0f, float( nbVertexH ) / 2.0f, 0.0f };
							b.pos -= castor::Point3f{ float( nbVertexW ) / 2.0f, float( nbVertexH ) / 2.0f, 0.0f };
							return castor::point::dot( a.pos, a.pos ) < castor::point::dot( b.pos, b.pos );
					} );
			}
		}

		submesh->addPoints( points );

		if ( !sortAroundCenter )
		{
			auto indexMapping = submesh->createComponent< TriFaceMapping >();
			auto & indexMappingData = indexMapping->getData();
			castor::Vector< FaceIndices > faces;
			faces.reserve( size_t( subDivisionsW ) * subDivisionsD * 2u );

			for ( uint32_t i = 0; i < subDivisionsW; i++ )
			{
				for ( uint32_t j = i * subDivisionsD; j < ( i + 1 ) * subDivisionsD; j++ )
				{
					faces.push_back( FaceIndices{ j + subDivisionsW + 1 + i, j + i, j + subDivisionsW + 2 + i } );
					faces.push_back( FaceIndices{ j + i + 1, j + subDivisionsW + 2 + i, j + i } );
				}
			}

			indexMappingData.addFaceGroup( faces );
			indexMapping->computeTangents();
		}

		if ( tileUV )
		{
			auto secondaryUV = submesh->createComponent< Texcoords1Component >();
			auto & secondaryUVData = secondaryUV->getData();
			auto & tiledUV = secondaryUVData.getData();
			tiledUV.reserve( size_t( nbVertexW ) * nbVertexH );

			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					tiledUV.push_back( castor::Point3f{ float( ( subDivisionsW + 1u )* i ) * gapW / width
						, float( ( subDivisionsD + 1u ) * j ) * gapH / depth
						, 0.0f } );
				}
			}
		}
	}
}
