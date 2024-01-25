#include "DiamondSquareTerrain/GenerateHeightMap.hpp"

namespace diamond_square_terrain
{
	void generateHeightMap( bool island
		, std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, Matrix & heightMap )
	{
		castor::Function< void( uint32_t, uint32_t, uint32_t, uint32_t, float, uint32_t ) > divide = [&heightMap
			, &engine
			, &divide]( uint32_t x1
				, uint32_t y1
				, uint32_t x2
				, uint32_t y2
				, float range
				, uint32_t level )
		{
			std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

			if ( level < 1 ) return;

			// diamonds
			for ( unsigned i = x1 + level; i < x2; i += level )
			{
				for ( unsigned j = y1 + level; j < y2; j += level )
				{
					float a = heightMap( i - level, j - level );
					float b = heightMap( i, j - level );
					float c = heightMap( i - level, j );
					float d = heightMap( i, j );
					heightMap( i - level / 2u, j - level / 2u ) = ( a + b + c + d ) / 4 + distribution( engine ) * range;
				}
			}

			// squares
			for ( unsigned i = x1 + 2u * level; i < x2; i += level )
			{
				for ( unsigned j = y1 + 2u * level; j < y2; j += level )
				{
					float a = heightMap( i - level, j - level );
					float b = heightMap( i, j - level );
					float c = heightMap( i - level, j );
					float d = heightMap( i - level / 2, j - level / 2u );

					heightMap( i - level, j - level / 2u ) = ( a + c + d + heightMap( i - 3 * level / 2u, j - level / 2u ) ) / 4 + distribution( engine ) * range;
					heightMap( i - level / 2u, j - level ) = ( a + b + d + heightMap( i - level / 2u, j - 3 * level / 2u ) ) / 4 + distribution( engine ) * range;
				}
			}

			divide( x1, y1, x2, y2, range / 2.0f, level / 2u );
		};

		divide( 1u, 1u, max, max, 1.0f, size );

		auto rescale = [&heightMap, max]()
		{
			auto yMin = std::numeric_limits< float >::max();
			auto yMax = std::numeric_limits< float >::lowest();

			for ( auto z = 0u; z <= max; z++ )
			{
				for ( auto x = 0u; x <= max; x++ )
				{
					yMin = std::min( yMin, heightMap( x, z ) );
					yMax = std::max( yMax, heightMap( x, z ) );
				}
			}

			auto range = castor::makeRange( yMin, yMax );

			for ( auto z = 0u; z <= max; z++ )
			{
				for ( auto x = 0u; x <= max; x++ )
				{
					heightMap( x, z ) = range.percent( heightMap( x, z ) );
				}
			}
		};
		rescale();

		if ( island )
		{
			auto center = float( max / 2u );
			auto maxDist = sqrt( center * center + center * center );

			auto distance = [center, maxDist]( uint32_t x, uint32_t z )
			{
				float distX = std::abs( float( x ) - center );
				float distZ = std::abs( float( z ) - center );
				return 1.0f - sqrt( distX * distX + distZ * distZ ) / maxDist;
			};

			for ( auto z = 0u; z <= max; z++ )
			{
				for ( auto x = 0u; x <= max; x++ )
				{
					heightMap( x, z ) = float( heightMap( x, z ) * distance( x, z ) );
				}
			}

			rescale();
		}
	}
}
