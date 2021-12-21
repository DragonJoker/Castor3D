#include "Castor3D/Model/Mesh/Generator/Plane.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SecondaryUVComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"

namespace castor3d
{
	Plane::Plane()
		: MeshGenerator{ cuT( "plane" ) }
	{
	}

	MeshGeneratorSPtr Plane::create()
	{
		return std::make_shared< Plane >();
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
		castor::Point3f ptCurrentUV;
		castor::Point3f ptPreviousUV;
		castor::Point3f ptNormal( 0.0, 1.0, 0.0 );
		castor::Point3f ptTangent;
		castor::Point2f ptUv;
		SubmeshSPtr submesh = mesh.createSubmesh();
		InterleavedVertexArray points;

		if ( flipYZ )
		{
			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					points.push_back( InterleavedVertex{}
						.position( castor::Point3f{ offsetW + ( float( i ) * gapW ), 0.0, offsetH + ( float( j ) * gapH ) } )
						.normal( castor::Point3f{ 0.0, 1.0, 0.0 } )
						.texcoord( castor::Point2f{ float( i ) * gapW / width, float( j ) * gapH / depth } ) );
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
					points.push_back( InterleavedVertex{}
						.position( castor::Point3f{ offsetW + ( float( i ) * gapW ), offsetH + ( float( j ) * gapH ), 0.0 } )
						.normal( castor::Point3f{ 0.0, 0.0, 1.0 } )
						.texcoord( castor::Point2f{ float( i ) * gapW / width, float( j ) * gapH / depth } ) );
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
			auto indexMapping = std::make_shared< TriFaceMapping >( *submesh );

			for ( uint32_t i = 0; i < subDivisionsW; i++ )
			{
				for ( uint32_t j = i * subDivisionsD; j < ( i + 1 ) * subDivisionsD; j++ )
				{
					indexMapping->addFace( j + subDivisionsW + 1 + i, j + i, j + subDivisionsW + 2 + i );
					indexMapping->addFace( j + i + 1, j + subDivisionsW + 2 + i, j + i );
				}
			}

			indexMapping->computeTangentsFromNormals();
			submesh->setIndexMapping( indexMapping );
		}

		if ( tileUV )
		{
			std::vector< castor::Point3f > tiledUV;

			for ( uint32_t i = 0; i < nbVertexW; i++ )
			{
				for ( uint32_t j = 0; j < nbVertexH; j++ )
				{
					tiledUV.push_back( castor::Point3f{ float( ( subDivisionsW + 1u )* i ) * gapW / width
						, float( ( subDivisionsD + 1u ) * j ) * gapH / depth
						, 0.0f } );
				}
			}

			auto secondaryUV = std::make_shared< SecondaryUVComponent >( *submesh );
			secondaryUV->addTexcoords( tiledUV );
			submesh->addComponent( secondaryUV );
		}

		mesh.computeContainers();
	}
}
