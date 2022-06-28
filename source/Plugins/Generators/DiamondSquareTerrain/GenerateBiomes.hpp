/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GenerateBiomes_H___
#define ___C3D_GenerateBiomes_H___

#include "DiamondSquareTerrainPrerequisites.hpp"

#include <Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/ComponentModule.hpp>

namespace diamond_square_terrain
{
	inline float getMinHeight( Biome const & value )
	{
		return value.heightRange.getMin();
	}

	inline float getMaxHeight( Biome const & value )
	{
		return value.heightRange.getMax();
	}

	inline float getMinHeight( SlopeBiome const & value )
	{
		return value.steepnessRange.getMin();
	}

	inline float getMaxHeight( SlopeBiome const & value )
	{
		return value.steepnessRange.getMax();
	}

	template< typename DataT >
	inline BlendRanges buildBlendRanges( std::vector< DataT > const & values )
	{
		BlendRanges result;
		auto cur = values.begin();
		auto prv = values.end();
		uint32_t index{};
		float prvBlendRange{};

		while ( cur != values.end() )
		{
			auto curBlendRange = ( getMaxHeight( *cur ) - getMinHeight( *cur ) ) / 4.0f;

			if ( index == 0u )
			{
				if ( index == values.size() - 1u )
				{
					result.push_back( { 0u
						, 0u
						, castor::makeRange( 0.0f, 1.0f ) } );
				}
				else
				{
					auto rangeHi = getMaxHeight( *cur ) - curBlendRange;
					result.push_back( { 0u
						, 0u
						, castor::makeRange( 0.0f, rangeHi ) } );
				}
			}
			else
			{
				auto rangeLo = getMaxHeight( *prv ) - prvBlendRange;
				auto rangeHi = getMinHeight( *cur ) + curBlendRange;
				result.push_back( { index - 1u
					, index
					, castor::makeRange( rangeLo, rangeHi ) } );
				rangeLo = rangeHi;

				if ( index == values.size() - 1u )
				{
					rangeHi = 1.0f;
				}
				else
				{
					rangeHi = getMaxHeight( *cur ) - curBlendRange;
				}

				result.push_back( { index
					, index
					, castor::makeRange( rangeLo, rangeHi ) } );
			}

			prvBlendRange = curBlendRange;
			prv = cur;
			++cur;
			++index;
		}

		return result;
	}

	void generateBiomes( std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, float heatOffset
		, float zeroPoint
		, Matrix const & heightMap
		, Biomes biomes
		, castor3d::FaceArray const & faces
		, std::map< uint32_t, uint32_t > const & vertexMap
		, castor3d::SubmeshAnimationBuffer & submesh );
}

#endif
