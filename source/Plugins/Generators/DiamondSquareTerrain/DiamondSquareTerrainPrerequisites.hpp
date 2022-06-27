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
	struct Biome
	{
		castor::String name{};
		castor::Point2f range{};
		uint32_t passIndex{};
	};

	struct BlendRange
	{
		uint32_t beginIndex{};
		uint32_t endIndex{};
		castor::Range< float > range;
	};

	using Biomes = std::vector< Biome >;
	using BlendRanges = std::vector< BlendRange >;

	struct Matrix
	{
		inline explicit Matrix( uint32_t size )
			: m_size{ size }
		{
			m_map.resize( size_t( m_size ) * m_size, 0.0f );
		}

		inline uint32_t getIndex( uint32_t x, uint32_t y, uint32_t size )const
		{
			return x * size + y;
		}

		inline uint32_t getIndex( uint32_t x, uint32_t y )const
		{
			return getIndex( x, y, m_size );
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

		inline float const & operator()( uint32_t x, uint32_t y )const
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
}

#endif
