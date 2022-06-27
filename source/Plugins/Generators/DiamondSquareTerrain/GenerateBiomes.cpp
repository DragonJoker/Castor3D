#include "DiamondSquareTerrain/GenerateHeightMap.hpp"

#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp>

#include <CastorUtils/Noise/FractalNoise.hpp>
#include <CastorUtils/Noise/PerlinNoise.hpp>

namespace diamond_square_terrain
{
	namespace biomes
	{
		float getMin( std::pair< castor::Point2f, castor::Point3f > const & value )
		{
			return value.first->x;
		}

		float getMin( Biome const & value )
		{
			return value.range->x;
		}

		float getMax( std::pair< castor::Point2f, castor::Point3f > const & value )
		{
			return value.first->y;
		}

		float getMax( Biome const & value )
		{
			return value.range->y;
		}

		template< typename DataT >
		BlendRanges buildBlendRanges( std::vector< DataT > const & values )
		{
			BlendRanges result;
			auto cur = values.begin();
			auto prv = values.end();
			uint32_t index{};
			float prvBlendRange{};

			while ( cur != values.end() )
			{
				auto curBlendRange = ( getMax( *cur ) - getMin( *cur ) ) / 4.0f;

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
						auto rangeHi = getMax( *cur ) - curBlendRange;
						result.push_back( { 0u
							, 0u
							, castor::makeRange( 0.0f, rangeHi ) } );
					}
				}
				else
				{
					auto rangeLo = getMax( *prv ) - prvBlendRange;
					auto rangeHi = getMin( *cur ) + curBlendRange;
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
						rangeHi = getMax( *cur ) - curBlendRange;
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

		BlendRange const & findBlendRange( float height
			, BlendRanges const & ranges )
		{
			auto it = std::find_if( ranges.begin()
				, ranges.end()
				, [height]( BlendRange const & lookup )
				{
					return lookup.range.clamp( height ) == height;
				} );
			CU_Require( it != ranges.end() );
			return *it;
		}

		float alterHeight( float height
			, float zeroPoint
			, uint32_t x
			, uint32_t z
			, std::vector< Matrix > const & noiseMaps )
		{
			height += noiseMaps[0u]( x, z ) * ( height - zeroPoint );
			return std::min( 1.0f, std::max( 0.0f, height ) );
		}

		castor::Point3f getColour( float height
			, BlendRanges const & ranges
			, std::vector< std::pair< castor::Point2f, castor::Point3f > > const & colours )
		{
			auto & range = findBlendRange( height, ranges );
			castor::Point3f result{};

			if ( range.beginIndex == range.endIndex )
			{
				result = colours[range.beginIndex].second;
			}
			else
			{
				auto weight = range.range.percent( height );
				result = colours[range.beginIndex].second * ( 1.0f - weight )
					+ colours[range.endIndex].second * weight;
			}

			return result;
		}

		castor3d::PassMasks getPassMasks( float height
			, BlendRanges const & ranges
			, Biomes const & biomes )
		{
			auto & range = findBlendRange( height, ranges );
			castor3d::PassMasks result{};

			if ( range.beginIndex == range.endIndex )
			{
				result.data[range.beginIndex] = 0xFFu;
			}
			else
			{
				auto weight = range.range.percent( height );
				result.data[range.beginIndex] = uint8_t( ( 1.0f - weight ) * 255.0f );
				result.data[range.endIndex] = uint8_t( weight * 255.0f );
			}

			return result;
		}

		Matrix generateNoiseMap( std::default_random_engine engine
			, uint32_t width )
		{
			Matrix result{ width };
			auto fractal = castor::makeFractalNoise( castor3d::getMipLevels( { width, width, 1u }, VK_FORMAT_R8G8B8A8_UNORM )
				, castor::PerlinNoiseT< double >{ engine } );
			auto yMin = std::numeric_limits< float >::max();
			auto yMax = std::numeric_limits< float >::lowest();

			for ( auto x = 0u; x < width; x++ )
			{
				for ( auto y = 0u; y < width; y++ )
				{
					auto nx = float( x ) / float( width );
					auto ny = float( y ) / float( width );
					auto v = float( fractal.noise( nx, ny, 0.0f ) );
					result( x, y ) = v;
					yMin = std::min( v, yMin );
					yMax = std::max( v, yMax );
				}
			}

			auto range = castor::makeRange( yMin, yMax );

			for ( auto x = 0u; x < width; x++ )
			{
				for ( auto y = 0u; y < width; y++ )
				{
					result( x, y ) = range.percent( result( x, y ) ) - 0.5f;
				}
			}

			return result;
		}

		std::vector< Matrix > generateNoiseMaps( std::default_random_engine engine
			, size_t count
			, uint32_t width )
		{
			std::vector< Matrix > result;

			for ( size_t i = 0u; i < count; ++i )
			{
				result.push_back( generateNoiseMap( engine, width ) );
			}

			return result;
		}
	}

	void generateBiomes( std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, float heatOffset
		, float zeroPoint
		, Matrix const & heightMap
		, Biomes const & biomes
		, castor3d::TriFaceMapping const & faces
		, castor3d::Submesh & submesh )
	{
		if ( biomes.empty() )
		{
			std::vector< std::pair< castor::Point2f, castor::Point3f > > hmEarth = {
				{ { 0.0f, 0.05f }, { 0.0 / 255.0, 0.0 / 255.0, 32.0 / 255.0 } }, //Abyss
				{ { 0.05f, 0.2f }, { 0.0 / 255.0, 0.0 / 255.0, 92.0 / 255.0 } }, //Deep blue
				{ { 0.2f, 0.25f }, { 80.0 / 255.0, 110.0 / 255.0, 230.0 / 255.0 } }, //Blue
				{ { 0.25f, 0.27f }, { 255.0 / 255.0, 205.0 / 255.0, 75.0 / 255.0 } }, //Yellow
				{ { 0.27f, 0.6f }, { 90.0 / 255.0, 160.0 / 255.0, 70.0 / 255.0 } }, //Green
				{ { 0.6f, 0.75f }, { 50.0 / 255.0, 90.0 / 255.0, 50.0 / 255.0 } }, //Deep Green
				{ { 0.75f, 0.9f }, { 50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0 } }, //Rock
				{ { 0.9f, 1.0f }, { 200.0 / 255.0, 210.0 / 255.0, 190.0 / 255.0 } }, //White
			};
			auto ranges = biomes::buildBlendRanges( hmEarth );
			auto noiseMaps = biomes::generateNoiseMaps( engine, hmEarth.size(), size );
			auto & colours = submesh.createComponent< castor3d::ColoursComponent >()->getData();

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					auto height = biomes::alterHeight( heightMap( x, z ) + heatOffset
						, zeroPoint
						, x, z
						, noiseMaps );
					colours.push_back( biomes::getColour( height
						, ranges
						, hmEarth ) );
				}
			}
		}
		else
		{
			auto ranges = biomes::buildBlendRanges( biomes );
			auto noiseMaps = biomes::generateNoiseMaps( engine, biomes.size(), size );
			auto & passMasks = submesh.createComponent< castor3d::PassMasksComponent >()->getData();

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					auto height = biomes::alterHeight( heightMap( x, z ) + heatOffset
						, zeroPoint
						, x, z
						, noiseMaps );
					passMasks.push_back( biomes::getPassMasks( height
						, ranges
						, biomes ) );
				}
			}
		}
	}
}
