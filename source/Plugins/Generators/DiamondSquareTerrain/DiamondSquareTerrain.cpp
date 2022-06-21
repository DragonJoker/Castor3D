#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <random>

namespace diamond_square_terrain
{
	namespace
	{
		struct Matrix
		{
			inline explicit Matrix( uint32_t p_size )
				: m_size{ p_size }
			{
				m_map.resize( size_t( m_size ) * m_size );
			}

			inline uint32_t getIndex( uint32_t x, uint32_t y, uint32_t size )const
			{
				return x * size + y;
			}

			inline uint32_t getIndex( uint32_t x, uint32_t y )const
			{
				return getIndex( x, y,  m_size );
			}

			inline float & operator()( uint32_t x, uint32_t y )
			{
				auto index = getIndex( x, y );

				if ( index >= m_map.size() )
				{
					static float dummy = -1;
					return dummy;
				}

				return m_map[getIndex( x, y )];
			}

		private:
			std::vector< float > m_map;
			uint32_t m_size;
		};

		std::default_random_engine createRandomEngine( bool disableRandomSeed )
		{
			if ( disableRandomSeed )
			{
				return std::default_random_engine{};
			}

			std::random_device r;
			return std::default_random_engine{ r() };
		}
	}

	castor::String const Generator::Type = cuT( "diamond_square_terrain" );
	castor::String const Generator::Name = cuT( "Diamond Square Terrain Generator" );
	castor::String const Generator::ParamRoughness = cuT( "roughness" );
	castor::String const Generator::ParamRandomSeed = cuT( "disableRandomSeed" );
	castor::String const Generator::ParamScale = cuT( "scale" );
	castor::String const Generator::ParamDetail = cuT( "detail" );
	castor::String const Generator::ParamMinY = cuT( "minY" );

	Generator::Generator()
		: MeshGenerator{ cuT( "diamond_square_terrain" ) }
	{
	}

	castor3d::MeshGeneratorSPtr Generator::create()
	{
		return std::make_shared< Generator >();
	}

	void Generator::doGenerate( castor3d::Mesh & p_mesh
		, castor3d::Parameters const & p_parameters )
	{
		castor::String param;
		uint32_t size = 0u;
		float roughness = 0.0f;
		float scale = 1.0f;
		float minY = std::numeric_limits< float >::lowest();
		bool disableRandomSeed = false;

		if ( p_parameters.get( ParamRoughness, param ) )
		{
			roughness = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamRandomSeed, param ) )
		{
			disableRandomSeed = true;
		}

		if ( p_parameters.get( ParamScale, param ) )
		{
			scale = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamDetail, param ) )
		{
			size = uint32_t( pow( 2, castor::string::toUInt( param ) ) + 1u );
		}

		if ( p_parameters.get( ParamMinY, param ) )
		{
			minY = castor::string::toFloat( param );
		}

		if ( size )
		{
			Matrix map{ size };
			auto max = size - 1;
			auto engine = createRandomEngine( disableRandomSeed );

			auto average = []( std::vector< float > values )
			{
				auto total = std::accumulate( values.begin()
					, values.end()
					, 0.0f
					, []( float accum, float value )
					{
						if ( value != -1 )
						{
							accum += value;
						}

						return accum;
					} );
				return total / 4.0f;
			};

			auto diamond = [&average, &map]( uint32_t x
				, uint32_t y
				, uint32_t range
				, float offset )
			{
				auto ave = average( { {
						map( x, y - range ),      // top
						map( x + range, y ),      // right
						map( x, y + range ),      // bottom
						map( x - range, y )       // left
					} } );
				map( x, y ) = ave + offset;
			};

			auto square = [&average, &map]( uint32_t x
				, uint32_t y
				, uint32_t range
				, float offset )
			{
				auto ave = average( { {
						map( x - range, y - range ),   // upper left
						map( x + range, y - range ),   // upper right
						map( x + range, y + range ),   // lower right
						map( x - range, y + range )    // lower left
					} } );
				map( x, y ) = ave + offset;
			};

			std::function< void( uint32_t, float ) > divide = [&max
				, &square
				, &diamond
				, &engine
				, &divide]( uint32_t psize
				, float proughness )
			{
				uint32_t half = psize / 2;
				auto lscale = proughness * float( psize );
				std::uniform_real_distribution< float > distribution;

				if ( half < 1 ) return;

				for ( auto y = half; y < max; y += psize )
				{
					for ( auto x = half; x < max; x += psize )
					{
						square( x, y, half, distribution( engine ) * lscale * 2 - lscale );
					}
				}

				for ( auto y = 0u; y <= max; y += half )
				{
					for ( auto x = ( y + half ) % psize; x <= max; x += psize )
					{
						diamond( x, y, half, distribution( engine ) * lscale * 2 - lscale );
					}
				}

				divide( psize / 2, proughness );
			};

			divide( size, roughness );

			if ( minY != std::numeric_limits< float >::lowest() )
			{
				auto min = 0.0f;

				for ( auto z = 1u; z < max; z++ )
				{
					for ( auto x = 1u; x < max; x++ )
					{
						min = std::min( min, map( x, z ) );
					}
				}

				if ( min != minY )
				{
					auto offset = minY - min;

					for ( auto z = 1u; z < max; z++ )
					{
						for ( auto x = 1u; x < max; x++ )
						{
							map( x, z ) += offset;
						}
					}
				}
			}

			auto submesh = p_mesh.createSubmesh();
			auto positions = submesh->createComponent< castor3d::PositionsComponent >();
			auto normals = submesh->createComponent< castor3d::NormalsComponent >();
			auto tangents = submesh->createComponent< castor3d::TangentsComponent >();
			auto texcoords = submesh->createComponent< castor3d::Texcoords0Component >();
			auto mapping = submesh->createComponent< castor3d::TriFaceMapping >();
			auto transform = [&]( uint32_t v )
			{
				return scale * ( float( v ) - float( max ) / 2.0f );
			};

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					submesh->addPoint( castor3d::InterleavedVertex{}
						.position( castor::Point3f{ transform( x ), scale * map( x, z ), transform( z ) } )
						.texcoord( castor::Point2f{ float( x ) / scale, float( z ) / scale } ) );
				}
			}

			// Generate quads 
			for ( auto y = 0u; y < max - 2; y++ )
			{
				for ( auto x = 0u; x < max - 2; x++ )
				{
					mapping->addFace( map.getIndex( x, y, size - 2 )
						, map.getIndex( x + 1, y, size - 2 )
						, map.getIndex( x, y + 1, size - 2 ) );
					mapping->addFace( map.getIndex( x + 1, y, size - 2 )
						, map.getIndex( x + 1, y + 1, size - 2 )
						, map.getIndex( x, y + 1, size - 2 ) );
				}
			}

			mapping->computeNormals( true );
		}
	}
}
