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

			inline uint32_t index( uint32_t x, uint32_t y, uint32_t l_size )const
			{
				return x * l_size + y;
			}

			inline uint32_t index( uint32_t x, uint32_t y )const
			{
				return index( x, y,  m_size );
			}

			inline float & operator()( uint32_t x, uint32_t y )
			{
				static float l_dummy = -1;
				auto l_index = index( x, y );

				if ( l_index >= m_map.size() )
				{
					return l_dummy;
				}

				return m_map[index( x, y )];
			}

			inline float const & operator()( uint32_t x, uint32_t y )const
			{
				static float l_dummy = -1;
				auto l_index = index( x, y );

				if ( l_index >= m_map.size() )
				{
					return l_dummy;
				}

				return m_map[index( x, y )];
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
		String l_param;
		uint32_t l_size = 0u;
		float l_roughness = 0.0f;

		if ( p_parameters.Get( cuT( "roughness" ), l_param ) )
		{
			l_roughness = string::to_float( l_param );
		}

		if ( p_parameters.Get( cuT( "detail" ), l_param ) )
		{
			l_size = uint32_t( pow( 2, string::to_uint( l_param ) ) + 1u );
		}

		if ( l_size )
		{
			Matrix l_map{ l_size };
			auto l_max = l_size - 1;
			std::random_device l_device;

			auto l_average = []( std::vector< float > p_values )
			{
				auto l_total = std::accumulate( p_values.begin()
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

				return l_total / 4;
			};

			auto l_square = [&l_average, &l_map]( uint32_t p_x
				, uint32_t p_y
				, uint32_t p_size
				, float p_offset )
			{
				auto l_ave = l_average( { {
						l_map( p_x - p_size, p_y - p_size ),   // upper left
						l_map( p_x + p_size, p_y - p_size ),   // upper right
						l_map( p_x + p_size, p_y + p_size ),   // lower right
						l_map( p_x - p_size, p_y + p_size )    // lower left
					} } );
				l_map( p_x, p_y ) = l_ave + p_offset;
			};

			auto l_diamond = [&l_average, &l_map]( uint32_t p_x
				, uint32_t p_y
				, uint32_t p_size
				, float p_offset )
			{
				auto l_ave = l_average( { {
						l_map( p_x, p_y - p_size ),      // top
						l_map( p_x + p_size, p_y ),      // right
						l_map( p_x, p_y + p_size ),      // bottom
						l_map( p_x - p_size, p_y )       // left
					} } );
				l_map( p_x, p_y ) = l_ave + p_offset;
			};

			std::function< void( uint32_t, float ) > l_divide = [&l_max
				, &l_square
				, &l_diamond
				, &l_device
				, &l_divide]( uint32_t p_size
				, float p_roughness )
			{
				uint32_t l_half = p_size / 2;
				auto l_scale = p_roughness * p_size;
				std::uniform_real_distribution< float > l_distribution;

				if ( l_half < 1 ) return;

				for ( auto y = l_half; y < l_max; y += p_size )
				{
					for ( auto x = l_half; x < l_max; x += p_size )
					{
						l_square( x, y, l_half, l_distribution( l_device ) * l_scale * 2 - l_scale );
					}
				}

				for ( auto y = 0u; y <= l_max; y += l_half )
				{
					for ( auto x = ( y + l_half ) % p_size; x <= l_max; x += p_size )
					{
						l_diamond( x, y, l_half, l_distribution( l_device ) * l_scale * 2 - l_scale );
					}
				}

				l_divide( p_size / 2, p_roughness );
			};

			l_divide( l_size, l_roughness );

			auto l_submesh = p_mesh.CreateSubmesh();

			for ( auto y = 1u; y < l_max; y++ )
			{
				for ( auto x = 1u; x < l_max; x++ )
				{
					l_submesh->AddPoint( real( x ), l_map( x, y ), real( y ) );
				}
			}

			// Generate quads 
			for ( auto y = 0u; y < l_max - 2; y++ )
			{
				for ( auto x = 0u; x < l_max - 2; x++ )
				{
					l_submesh->AddFace( l_map.index( x, y, l_size - 2 )
						, l_map.index( x + 1, y, l_size - 2 )
						, l_map.index( x, y + 1, l_size - 2 ) );
					l_submesh->AddFace( l_map.index( x + 1, y, l_size - 2 )
						, l_map.index( x + 1, y + 1, l_size - 2 )
						, l_map.index( x, y + 1, l_size - 2 ) );
				}
			}

			l_submesh->ComputeNormals( true );
			p_mesh.ComputeContainers();
		}
	}
}
