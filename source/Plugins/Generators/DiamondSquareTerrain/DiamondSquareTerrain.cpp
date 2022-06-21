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
		float xScale = 1.0f;
		float zScale = 1.0f;
		float uScale = 1.0f;
		float vScale = 1.0f;
		castor::Range< float > yRange{ -500.0f, 500.0f };
		bool disableRandomSeed = false;

		if ( p_parameters.get( ParamRandomSeed, param ) )
		{
			disableRandomSeed = true;
		}

		if ( p_parameters.get( ParamYMin, param ) )
		{
			yRange = castor::makeRange( yRange.getMin(), castor::string::toFloat( param ) );
		}

		if ( p_parameters.get( ParamYMax, param ) )
		{
			yRange = castor::makeRange( castor::string::toFloat( param ), yRange.getMax() );
		}

		if ( p_parameters.get( ParamXScale, param ) )
		{
			xScale = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamZScale, param ) )
		{
			zScale = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamUScale, param ) )
		{
			uScale = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamVScale, param ) )
		{
			vScale = castor::string::toFloat( param );
		}

		if ( p_parameters.get( ParamDetail, param ) )
		{
			size = uint32_t( pow( 2, castor::string::toUInt( param ) ) );
		}

		if ( size )
		{
			Matrix map{ size };
			auto max = size - 1;
			auto engine = createRandomEngine( disableRandomSeed );

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

				for ( auto z = 1u; z < max; z++ )
				{
					for ( auto x = 1u; x < max; x++ )
					{
						yMin = std::min( yMin, map( x, z ) );
						yMax = std::max( yMax, map( x, z ) );
					}
				}

				auto range = castor::makeRange( yMin, yMax );

				for ( auto z = 1u; z < max; z++ )
				{
					for ( auto x = 1u; x < max; x++ )
					{
						map( x, z ) = range.percent( map( x, z ) );
					}
				}
			};
			rescale();

			auto submesh = p_mesh.createSubmesh();
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

			// Generate quads 
			auto mapping = submesh->createComponent< castor3d::TriFaceMapping >();

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
