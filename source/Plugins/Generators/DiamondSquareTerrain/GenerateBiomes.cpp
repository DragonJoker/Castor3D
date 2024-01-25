#include "DiamondSquareTerrain/GenerateBiomes.hpp"

#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp>

#include <CastorUtils/Noise/FractalNoise.hpp>
#include <CastorUtils/Noise/PerlinNoise.hpp>

namespace diamond_square_terrain
{
	namespace biomes
	{
		static BlendRange const & findBlendRange( float value
			, BlendRanges const & ranges )
		{
			auto it = std::find_if( ranges.begin()
				, ranges.end()
				, [value]( BlendRange const & lookup )
				{
					return lookup.range.clamp( value ) == value;
				} );
			CU_Require( it != ranges.end() );
			return *it;
		}

		static castor::Point3f getColour( float steepness
			, Biome const & biome )
		{
			auto & steepnessRange = findBlendRange( steepness, biome.steepnessRanges );
			castor::Point3f result{};

			if ( steepnessRange.beginIndex == steepnessRange.endIndex )
			{
				auto & beginBiome = biome.steepnessBiomes[steepnessRange.beginIndex];
				result = beginBiome.colour;
			}
			else
			{
				auto weight = steepnessRange.range.percent( steepness );
				auto & beginBiome = biome.steepnessBiomes[steepnessRange.beginIndex];
				auto & endBiome = biome.steepnessBiomes[steepnessRange.endIndex];
				result = beginBiome.colour * ( 1.0f - weight )
					+ endBiome.colour * weight;
			}

			return result;
		}

		static castor::Point3f getColour( float height
			, float steepness
			, BlendRanges const & ranges
			, Biomes const & biomes )
		{
			auto & heightRange = findBlendRange( height, ranges );
			castor::Point3f result{};

			if ( heightRange.beginIndex == heightRange.endIndex )
			{
				auto & biome = biomes[heightRange.beginIndex];
				result = getColour( steepness, biome );
			}
			else
			{
				auto weight = heightRange.range.percent( height );
				auto & beginBiome = biomes[heightRange.beginIndex];
				auto & endBiome = biomes[heightRange.endIndex];
				result = getColour( steepness, beginBiome ) * ( 1.0f - weight )
					+ getColour( steepness, endBiome ) * weight;
			}

			return result;
		}

		static castor::Map< uint32_t, float > getPassWeights( float steepness
			, Biome const & biome )
		{
			auto & steepnessRange = findBlendRange( steepness, biome.steepnessRanges );
			castor::Map< uint32_t, float > result{};

			if ( steepnessRange.beginIndex == steepnessRange.endIndex )
			{
				result.emplace( biome.steepnessBiomes[steepnessRange.beginIndex].passIndex, 1.0f );
			}
			else
			{
				auto weight = steepnessRange.range.percent( steepness );
				result[biome.steepnessBiomes[steepnessRange.beginIndex].passIndex] += 1.0f - weight;
				result[biome.steepnessBiomes[steepnessRange.endIndex].passIndex] += weight;
			}

			return result;
		}

		static castor::Map< uint32_t, float > mergeWeights( castor::Map< uint32_t, float > const & lhs
			, castor::Map< uint32_t, float > const & rhs
			, float weight )
		{
			castor::Map< uint32_t, float > result;

			for ( auto & pair : lhs )
			{
				result[pair.first] = pair.second * ( 1.0f - weight );
			}

			for ( auto & pair : rhs )
			{
				result[pair.first] += pair.second * weight;
			}

			return result;
		}

		static castor::Map< uint32_t, float > getPassWeights( float height
			, float steepness
			, BlendRanges const & ranges
			, Biomes const & biomes )
		{
			auto & heightRange = findBlendRange( height, ranges );
			castor::Map< uint32_t, float > result;

			if ( heightRange.beginIndex == heightRange.endIndex )
			{
				auto & biome = biomes[heightRange.beginIndex];
				result = getPassWeights( steepness, biome );
			}
			else
			{
				auto weight = heightRange.range.percent( height );
				auto beginWeights = getPassWeights( steepness, biomes[heightRange.beginIndex] );
				auto endWeights = getPassWeights( steepness, biomes[heightRange.endIndex] );
				result = mergeWeights( beginWeights, endWeights, weight );
			}

			return result;
		}

		static castor3d::PassMasks getPassMasks( float height
			, float steepness
			, BlendRanges const & ranges
			, Biomes const & biomes )
		{
			auto weights = getPassWeights( height, steepness, ranges, biomes );
			castor3d::PassMasks result{};

			for ( auto & pair : weights )
			{
				result.data[pair.first] = uint8_t( pair.second * 255.0f );
			}

			return result;
		}

		static float alterHeight( float height
			, float zeroPoint
			, uint32_t x
			, uint32_t z
			, Matrix const & noiseMap )
		{
			height += noiseMap( x, z ) * ( height - zeroPoint );
			return std::min( 1.0f, std::max( 0.0f, height ) );
		}

		static Matrix generateNoiseMap( std::default_random_engine engine
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
	}

	void generateBiomes( std::default_random_engine engine
		, uint32_t max
		, uint32_t size
		, float heatOffset
		, float zeroPoint
		, Matrix const & heightMap
		, Biomes biomes
		, castor3d::FaceArray const & faces
		, castor::Map< uint32_t, uint32_t > const & vertexMap
		, castor3d::SubmeshAnimationBuffer & submesh )
	{
		bool areMaterial = !biomes.empty();

		if ( biomes.empty() )
		{
			float grassLow = 0.0f;
			float forestLow = 0.4f;
			float snowLow = 0.7f;
			SlopeBiome rock{ { 0.7f, 1.0f }
				, false
				, {}
				, { 50.0 / 255.0, 50.0 / 255.0, 50.0 / 255.0 } };

			if ( zeroPoint > 0.0f )
			{
				SlopeBiome sand{ { 0.0f, 0.2f }
					, false
					, {}
					, { 255.0 / 255.0, 205.0 / 255.0, 75.0 / 255.0 } };
				SlopeBiome sandSea{ { 0.0f, 0.2f }
					, false
					, {}
					, { 0.0 / 255.0, 0.0 / 255.0, 92.0 / 255.0 } };
				SlopeBiome sandDirt{ { 0.2f, 0.7f }
					, false
					, {}
					, { 200.0 / 255.0, 165.0 / 255.0, 60.0 / 255.0 } };
				SlopeBiome sandSeaDirt{ { 0.2f, 0.7f }
					, false
					, {}
					, { 0.0 / 255.0, 0/ 255.0, 60.0 / 255.0 } };
				SlopeBiome rockSea{ { 0.7f, 1.0f }
					, false
					, {}
					, { 20.0 / 255.0, 20.0 / 255.0, 20.0 / 255.0 } };
				grassLow = 0.27f;
				forestLow = 0.6f;
				snowLow = 0.85f;
				biomes.push_back( { cuT( "Sea" )
					, { 0.0f, 0.2f }
					, { sandSea, sandSeaDirt, rockSea } } );
				biomes.push_back( { cuT( "Sand" )
					, { 0.2f, grassLow }
					, { sand, sandDirt, rock } } );
			}

			SlopeBiome grass{ { 0.0f, 0.2f }
				, false
				, {}
				, { 90.0 / 255.0, 160.0 / 255.0, 70.0 / 255.0 } };
			SlopeBiome forest{ { 0.0f, 0.2f }
				, false
				, {}
				, { 50.0 / 255.0, 90.0 / 255.0, 50.0 / 255.0 } };
			SlopeBiome snow{ { 0.0f, 0.2f }
				, false
				, {}
				, { 200.0 / 255.0, 210.0 / 255.0, 190.0 / 255.0 } };
			SlopeBiome dirt{ { 0.2f, 0.7f }
				, false
				, {}
				, { 100.0 / 255.0, 50.0 / 255.0, 20.0 / 255.0 } };
			SlopeBiome darkDirt{ { 0.2f, 0.7f }
				, false
				, {}
				, { 80.0 / 255.0, 40.0 / 255.0, 16.0 / 255.0 } };
			biomes.push_back( { cuT( "Grass" )
				, { grassLow, forestLow }
				, { grass, dirt, rock } } );
			biomes.push_back( { cuT( "Forest" )
				, { forestLow, snowLow }
				, { forest, darkDirt, rock } } );
			biomes.push_back( { cuT( "Snow" )
				, { snowLow, 1.0f }
				, { snow, snow, rock } } );
		}

		for ( auto & biome : biomes )
		{
			biome.steepnessRanges = buildBlendRanges( biome.steepnessBiomes );
		}

		auto ranges = buildBlendRanges( biomes );
		auto noiseMap = biomes::generateNoiseMap( engine, size );
		auto & normals = submesh.normals;

		if ( areMaterial )
		{
			auto & passMasks = submesh.passMasks;

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					auto index = heightMap.getIndex( x, z );
					auto height = biomes::alterHeight( heightMap[index] + heatOffset
						, zeroPoint
						, x, z
						, noiseMap );
					auto steepness = std::abs( normals[vertexMap.find( index )->second]->z );
					passMasks.push_back( biomes::getPassMasks( height
						, steepness
						, ranges
						, biomes ) );
				}
			}
		}
		else
		{
			auto & colours = submesh.colours;

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					auto index = heightMap.getIndex( x, z );
					auto height = biomes::alterHeight( heightMap[index] + heatOffset
						, zeroPoint
						, x, z
						, noiseMap );
					auto steepness = std::abs( normals[vertexMap.find( index )->second]->z );
					colours.push_back( biomes::getColour( height
						, steepness
						, ranges
						, biomes ) );
				}
			}
		}
	}
}
