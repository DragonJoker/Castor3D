/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DiamondSquareTerrainPrerequisites_H___
#define ___C3D_DiamondSquareTerrainPrerequisites_H___

#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Range.hpp>

#include <random>
#include <vector>

namespace diamond_square_terrain
{
	struct SlopeBiome
	{
		castor::Range< float > steepnessRange{ 0.0f, 1.0f };
		bool isMaterial{};
		uint32_t passIndex{};
		castor::Point3f colour;
	};

	struct BlendRange
	{
		uint32_t beginIndex{};
		uint32_t endIndex{};
		castor::Range< float > range{ 0.0f, 1.0f };
	};

	struct Biome
	{
		castor::String name{};
		castor::Range< float > heightRange{ 0.0f, 1.0f };
		castor::Vector< SlopeBiome > steepnessBiomes{ 3u, SlopeBiome{} };
		castor::Vector< BlendRange > steepnessRanges{};
	};

	using Biomes = castor::Vector< Biome >;
	using BlendRanges = castor::Vector< BlendRange >;

	struct Matrix
	{
		explicit Matrix( uint32_t size )
			: m_size{ size }
		{
			m_map.resize( size_t( m_size ) * m_size, 0.0f );
		}

		uint32_t getIndex( uint32_t x, uint32_t y, uint32_t size )const
		{
			return x * size + y;
		}

		uint32_t getIndex( uint32_t x, uint32_t y )const
		{
			return getIndex( x, y, m_size );
		}

		float & operator[]( uint32_t index )
		{
			if ( index >= m_map.size() )
			{
				static float dummy = -1;
				return dummy;
			}

			return m_map[index];
		}

		float & operator()( uint32_t x, uint32_t y )
		{
			return operator[]( getIndex( x, y ) );
		}

		float const & operator[]( uint32_t index )const
		{
			if ( index >= m_map.size() )
			{
				static float dummy = -1;
				return dummy;
			}

			return m_map[index];
		}

		float const & operator()( uint32_t x, uint32_t y )const
		{
			return operator[]( getIndex( x, y ) );
		}

	private:
		castor::Vector< float > m_map;
		uint32_t m_size;
	};
}

#endif
