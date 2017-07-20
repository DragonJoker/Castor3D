#include "DiamondSquareTerrain.hpp"

#include <Mesh/Mesh.hpp>
#include <Mesh/Submesh.hpp>
#include <Miscellaneous/Parameter.hpp>

#include <random>

using namespace Castor;
using namespace Castor3D;

namespace diamond_square_terrain
{
	namespace
	{
		struct Matrix
		{
			inline Matrix( uint32_t p_size )
				: m_size{ p_size }
				, m_max{ m_size - 1 }
			{
				m_map.resize( m_size * m_size );
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
				static float dummy = -1;
				auto index = getIndex( x, y );

				if ( index >= m_map.size() )
				{
					return dummy;
				}

				return m_map[getIndex( x, y )];
			}

			inline float const & operator()( uint32_t x, uint32_t y )const
			{
				static float dummy = -1;
				auto index = getIndex( x, y );

				if ( index >= m_map.size() )
				{
					return dummy;
				}

				return m_map[getIndex( x, y )];
			}


		private:
			std::vector< float > m_map;
			uint32_t m_size;
			uint32_t m_max;
		};
	}

	String const Generator::Type = cuT( "diamond_square_terrain" );
	String const Generator::Name = cuT( "Diamond Square Terrain Generator" );

	Generator::Generator()
		: MeshGenerator{ cuT( "diamond_square_terrain" ) }
	{
	}

	Generator::~Generator()
	{
	}

	MeshGeneratorSPtr Generator::Create()
	{
		return std::make_shared< Generator >();
	}

	void Generator::DoGenerate( Mesh & p_mesh
		, Parameters const & p_parameters )
	{
		String param;
		uint32_t size = 0u;
		float roughness = 0.0f;

		if ( p_parameters.Get( cuT( "roughness" ), param ) )
		{
			roughness = string::to_float( param );
		}

		if ( p_parameters.Get( cuT( "detail" ), param ) )
		{
			size = uint32_t( pow( 2, string::to_uint( param ) ) + 1u );
		}

		if ( size )
		{
			Matrix map{ size };
			auto max = size - 1;
			std::random_device device;

			auto average = []( std::vector< float > p_values )
			{
				auto total = std::accumulate( p_values.begin()
					, p_values.end()
					, 0.0f
					, []( float p_accum, float p_value )
				{
					if ( p_value != -1 )
					{
						p_accum += p_value;
					}

					return p_accum;
				} );

				return total / 4;
			};

			auto square = [&average, &map]( uint32_t p_x
				, uint32_t p_y
				, uint32_t p_size
				, float p_offset )
			{
				auto ave = average( { {
						map( p_x - p_size, p_y - p_size ),   // upper left
						map( p_x + p_size, p_y - p_size ),   // upper right
						map( p_x + p_size, p_y + p_size ),   // lower right
						map( p_x - p_size, p_y + p_size )    // lower left
					} } );
				map( p_x, p_y ) = ave + p_offset;
			};

			auto diamond = [&average, &map]( uint32_t p_x
				, uint32_t p_y
				, uint32_t p_size
				, float p_offset )
			{
				auto ave = average( { {
						map( p_x, p_y - p_size ),      // top
						map( p_x + p_size, p_y ),      // right
						map( p_x, p_y + p_size ),      // bottom
						map( p_x - p_size, p_y )       // left
					} } );
				map( p_x, p_y ) = ave + p_offset;
			};

			std::function< void( uint32_t, float ) > divide = [&max
				, &square
				, &diamond
				, &device
				, &divide]( uint32_t p_size
				, float p_roughness )
			{
				uint32_t half = p_size / 2;
				auto scale = p_roughness * p_size;
				std::uniform_real_distribution< float > distribution;

				if ( half < 1 ) return;

				for ( auto y = half; y < max; y += p_size )
				{
					for ( auto x = half; x < max; x += p_size )
					{
						square( x, y, half, distribution( device ) * scale * 2 - scale );
					}
				}

				for ( auto y = 0u; y <= max; y += half )
				{
					for ( auto x = ( y + half ) % p_size; x <= max; x += p_size )
					{
						diamond( x, y, half, distribution( device ) * scale * 2 - scale );
					}
				}

				divide( p_size / 2, p_roughness );
			};

			divide( size, roughness );

			auto submesh = p_mesh.CreateSubmesh();

			for ( auto y = 1u; y < max; y++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					submesh->AddPoint( real( x ), map( x, y ), real( y ) );
				}
			}

			// Generate quads 
			for ( auto y = 0u; y < max - 2; y++ )
			{
				for ( auto x = 0u; x < max - 2; x++ )
				{
					submesh->AddFace( map.getIndex( x, y, size - 2 )
						, map.getIndex( x + 1, y, size - 2 )
						, map.getIndex( x, y + 1, size - 2 ) );
					submesh->AddFace( map.getIndex( x + 1, y, size - 2 )
						, map.getIndex( x + 1, y + 1, size - 2 )
						, map.getIndex( x, y + 1, size - 2 ) );
				}
			}

			submesh->ComputeNormals( true );
			p_mesh.ComputeContainers();
		}
	}
}
