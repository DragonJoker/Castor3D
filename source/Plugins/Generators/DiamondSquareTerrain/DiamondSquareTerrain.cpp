#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>

#include <random>

namespace diamond_square_terrain
{
	namespace gen
	{
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

		struct BlendRange
		{
			uint32_t beginIndex{};
			uint32_t endIndex{};
			castor::Range< float > range;
		};
		using BlendRanges = std::vector< BlendRange >;

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

		BlendRange const & findBlendRange( float value
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

		castor::Point3f getColour( float value
			, BlendRanges const & ranges
			, std::vector< std::pair< castor::Point2f, castor::Point3f > > const & colours )
		{
			auto & range = findBlendRange( value, ranges );
			auto weight = range.range.percent( value );
			castor::Point3f result{};

			if ( range.beginIndex == range.endIndex )
			{
				result = colours[range.beginIndex].second;
			}
			else
			{
				result = colours[range.beginIndex].second * ( 1.0f - weight )
					+ colours[range.endIndex].second * weight;
			}

			return result;
		}

		castor3d::PassMasks getPassMasks( float value
			, BlendRanges const & ranges
			, Biomes const & biomes )
		{
			auto & range = findBlendRange( value, ranges );
			auto weight = range.range.percent( value );
			castor3d::PassMasks result{};

			if ( range.beginIndex == range.endIndex )
			{
				result.data[range.beginIndex] = 0xFFu;
			}
			else
			{
				result.data[range.beginIndex] = uint8_t( ( 1.0f - weight ) * 255.0f );
				result.data[range.endIndex] = uint8_t( weight * 255.0f );
			}

			return result;
		}
	}

	castor::String const Generator::Type = cuT( "diamond_square_terrain" );
	castor::String const Generator::Name = cuT( "Diamond Square Terrain Generator" );
	castor::String const Generator::Biome = cuT( "biome" );
	castor::String const Generator::BiomeRange = cuT( "range" );
	castor::String const Generator::BiomePassIndex = cuT( "passIndex" );
	castor::String const Generator::ParamRandomSeed = cuT( "disableRandomSeed" );
	castor::String const Generator::ParamHeightRange = cuT( "heightRange" );
	castor::String const Generator::ParamYMin = cuT( "yMin" );
	castor::String const Generator::ParamYMax = cuT( "yMax" );
	castor::String const Generator::ParamXzScale = cuT( "xzScale" );
	castor::String const Generator::ParamXScale = cuT( "xScale" );
	castor::String const Generator::ParamZScale = cuT( "zScale" );
	castor::String const Generator::ParamUvScale = cuT( "uvScale" );
	castor::String const Generator::ParamUScale = cuT( "uScale" );
	castor::String const Generator::ParamVScale = cuT( "vScale" );
	castor::String const Generator::ParamDetail = cuT( "detail" );
	castor::String const Generator::ParamGradient = cuT( "gradient" );
	castor::String const Generator::ParamGradientFolder = cuT( "gradientFolder" );
	castor::String const Generator::ParamGradientRelative = cuT( "gradientRelative" );
	castor::String const Generator::ParamHeatOffset = cuT( "heatOffset" );
	castor::String const Generator::ParamIsland = cuT( "island" );

	Generator::Generator()
		: MeshGenerator{ cuT( "diamond_square_terrain" ) }
	{
	}

	castor3d::MeshGeneratorSPtr Generator::create()
	{
		return std::make_shared< Generator >();
	}

	void Generator::doGenerate( castor3d::Mesh & mesh
		, castor3d::Parameters const & parameters )
	{
		castor::String param;
		uint32_t size = 0u;
		float xScale = 1.0f;
		float zScale = 1.0f;
		float uScale = 1.0f;
		float vScale = 1.0f;
		float heatOffset = 0.0f;
		castor::Range< float > yRange{ -500.0f, 500.0f };
		bool disableRandomSeed = false;
		bool island = false;

		if ( parameters.get( ParamRandomSeed, param ) )
		{
			disableRandomSeed = ( param == "1" );
		}

		if ( parameters.get( ParamIsland, param ) )
		{
			island = ( param == "1" );
		}

		if ( parameters.get( ParamYMin, param ) )
		{
			yRange = castor::makeRange( yRange.getMin(), castor::string::toFloat( param ) );
		}

		if ( parameters.get( ParamYMax, param ) )
		{
			yRange = castor::makeRange( castor::string::toFloat( param ), yRange.getMax() );
		}

		if ( parameters.get( ParamXScale, param ) )
		{
			xScale = castor::string::toFloat( param );
		}

		if ( parameters.get( ParamZScale, param ) )
		{
			zScale = castor::string::toFloat( param );
		}

		if ( parameters.get( ParamUScale, param ) )
		{
			uScale = castor::string::toFloat( param );
		}

		if ( parameters.get( ParamVScale, param ) )
		{
			vScale = castor::string::toFloat( param );
		}

		if ( parameters.get( ParamDetail, param ) )
		{
			size = uint32_t( pow( 2, castor::string::toUInt( param ) ) );
		}

		if ( parameters.get( ParamHeatOffset, param ) )
		{
			heatOffset = castor::string::toFloat( param );
		}

		if ( size )
		{
			gen::Matrix map{ size };
			auto max = size - 1;
			auto engine = gen::createRandomEngine( disableRandomSeed );

			std::function< void( uint32_t, uint32_t, uint32_t, uint32_t, float, uint32_t ) > divide = [&map
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
						float a = map( i - level, j - level );
						float b = map( i, j - level );
						float c = map( i - level, j );
						float d = map( i, j );
						map( i - level / 2u, j - level / 2u ) = ( a + b + c + d ) / 4 + distribution( engine ) * range;
					}
				}

				// squares
				for ( unsigned i = x1 + 2u * level; i < x2; i += level )
				{
					for ( unsigned j = y1 + 2u * level; j < y2; j += level )
					{
						float a = map( i - level, j - level );
						float b = map( i, j - level );
						float c = map( i - level, j );
						float d = map( i - level / 2, j - level / 2u );

						map( i - level, j - level / 2u ) = ( a + c + d + map( i - 3 * level / 2u, j - level / 2u ) ) / 4 + distribution( engine ) * range;
						map( i - level / 2u, j - level ) = ( a + b + d + map( i - level / 2u, j - 3 * level / 2u ) ) / 4 + distribution( engine ) * range;
					}
				}

				divide( x1, y1, x2, y2, range / 2.0f, level / 2u );
			};

			divide( 1u, 1u, max, max, 1.0f, size );

			auto rescale = [&map, max]()
			{
				auto yMin = std::numeric_limits< float >::max();
				auto yMax = std::numeric_limits< float >::lowest();

				for ( auto z = 0u; z <= max; z++ )
				{
					for ( auto x = 0u; x <= max; x++ )
					{
						yMin = std::min( yMin, map( x, z ) );
						yMax = std::max( yMax, map( x, z ) );
					}
				}

				auto range = castor::makeRange( yMin, yMax );

				for ( auto z = 0u; z <= max; z++ )
				{
					for ( auto x = 0u; x <= max; x++ )
					{
						map( x, z ) = range.percent( map( x, z ) );
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
						map( x, z ) = map( x, z ) * distance( x, z );
					}
				}

				rescale();
			}

			auto submesh = mesh.createSubmesh();
			submesh->createComponent< castor3d::PositionsComponent >();
			submesh->createComponent< castor3d::Texcoords0Component >();
			submesh->createComponent< castor3d::NormalsComponent >();
			submesh->createComponent< castor3d::TangentsComponent >();

			auto transform = [&]( uint32_t v, float s )
			{
				return s * ( float( v ) - float( max ) / 2.0f );
			};

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					submesh->addPoint( castor3d::InterleavedVertex{}
						.position( castor::Point3f{ transform( x, xScale ), yRange.value( map( x, z ) ), transform( z, zScale ) } )
						.texcoord( castor::Point2f{ float( x ) / uScale, float( z ) / vScale } ) );
				}
			}

			if ( m_biomes.empty() )
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
				auto ranges = gen::buildBlendRanges( hmEarth );
				auto & colours = submesh->createComponent< castor3d::ColoursComponent >()->getData();

				for ( auto z = 1u; z < max; z++ )
				{
					for ( auto x = 1u; x < max; x++ )
					{
						colours.push_back( gen::getColour( map( x, z ) + heatOffset, ranges, hmEarth ) );
					}
				}
			}
			else
			{
				auto ranges = gen::buildBlendRanges( m_biomes );
				auto & passMasks = submesh->createComponent< castor3d::PassMasksComponent >()->getData();

				for ( auto z = 1u; z < max; z++ )
				{
					for ( auto x = 1u; x < max; x++ )
					{
						passMasks.push_back( gen::getPassMasks( map( x, z ) + heatOffset, ranges, m_biomes ) );
					}
				}
			}

			// Generate quads 
			auto mapping = submesh->createComponent< castor3d::TriFaceMapping >();

			for ( auto y = 1u; y < max - 2; y++ )
			{
				for ( auto x = 1u; x < max - 2; x++ )
				{
					mapping->addFace( map.getIndex( x, y, size - 2 )
						, map.getIndex( x, y + 1, size - 2 )
						, map.getIndex( x + 1, y, size - 2 ) );
					mapping->addFace( map.getIndex( x + 1, y, size - 2 )
						, map.getIndex( x, y + 1, size - 2 )
						, map.getIndex( x + 1, y + 1, size - 2 ) );
				}
			}

			mapping->computeNormals( true );
		}
	}
}
