#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include "DiamondSquareTerrain/GenerateHeightMap.hpp"
#include "DiamondSquareTerrain/GenerateBiomes.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/PassMasksComponent.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>

namespace diamond_square_terrain
{
	namespace gen
	{
		static std::default_random_engine createRandomEngine( bool disableRandomSeed )
		{
			if ( disableRandomSeed )
			{
				return std::default_random_engine{};
			}

			std::random_device r;
			return std::default_random_engine{ r() };
		}
	}

	c3d::MbString const Generator::Name = "Diamond Square Terrain Generator";
	c3d::String const Generator::Type = cuT( "diamond_square_terrain" );
	c3d::String const Generator::Biome = cuT( "biome" );
	c3d::String const Generator::BiomeRange = cuT( "range" );
	c3d::String const Generator::BiomeLowSteepness = cuT( "lowSteepness" );
	c3d::String const Generator::BiomeMediumSteepness = cuT( "mediumSteepness" );
	c3d::String const Generator::BiomeHighSteepness = cuT( "highSteepness" );
	c3d::String const Generator::ParamRandomSeed = cuT( "disableRandomSeed" );
	c3d::String const Generator::ParamHeightRange = cuT( "heightRange" );
	c3d::String const Generator::ParamYMin = cuT( "yMin" );
	c3d::String const Generator::ParamYMax = cuT( "yMax" );
	c3d::String const Generator::ParamXzScale = cuT( "xzScale" );
	c3d::String const Generator::ParamXScale = cuT( "xScale" );
	c3d::String const Generator::ParamZScale = cuT( "zScale" );
	c3d::String const Generator::ParamUvScale = cuT( "uvScale" );
	c3d::String const Generator::ParamUScale = cuT( "uScale" );
	c3d::String const Generator::ParamVScale = cuT( "vScale" );
	c3d::String const Generator::ParamDetail = cuT( "detail" );
	c3d::String const Generator::ParamGradient = cuT( "gradient" );
	c3d::String const Generator::ParamGradientFolder = cuT( "gradientFolder" );
	c3d::String const Generator::ParamGradientRelative = cuT( "gradientRelative" );
	c3d::String const Generator::ParamHeatOffset = cuT( "heatOffset" );
	c3d::String const Generator::ParamIsland = cuT( "island" );

	Generator::Generator()
		: MeshGenerator{ cuT( "diamond_square_terrain" ) }
	{
	}

	c3d::MeshGeneratorUPtr Generator::create()
	{
		return c3d::makeUniqueDerived< c3d::MeshGenerator, Generator >();
	}

	void Generator::doGenerate( c3d::Mesh & mesh
		, c3d::Parameters const & parameters )
	{
		c3d::String param;
		uint32_t size = 0u;
		float xScale = 1.0f;
		float zScale = 1.0f;
		float uScale = 1.0f;
		float vScale = 1.0f;
		float heatOffset = 0.0f;
		c3d::Range< float > heightRange{ -500.0f, 500.0f };
		bool disableRandomSeed = false;
		bool island = false;

		if ( parameters.get( ParamRandomSeed, param ) )
			disableRandomSeed = ( param == cuT( "1" ) );
		if ( parameters.get( ParamIsland, param ) )
			island = ( param == cuT( "1" ) );
		if ( parameters.get( ParamYMin, param ) )
			heightRange = c3d::makeRange( heightRange.getMin()
				, c3d::string::toFloat( param ) );
		if ( parameters.get( ParamYMax, param ) )
			heightRange = c3d::makeRange( c3d::string::toFloat( param )
				, heightRange.getMax() );
		if ( parameters.get( ParamXScale, param ) )
			xScale = c3d::string::toFloat( param );
		if ( parameters.get( ParamZScale, param ) )
			zScale = c3d::string::toFloat( param );
		if ( parameters.get( ParamUScale, param ) )
			uScale = c3d::string::toFloat( param );
		if ( parameters.get( ParamVScale, param ) )
			vScale = c3d::string::toFloat( param );
		if ( parameters.get( ParamDetail, param ) )
			size = uint32_t( pow( 2, c3d::string::toUInt( param ) ) );
		if ( parameters.get( ParamHeatOffset, param ) )
			heatOffset = c3d::string::toFloat( param );

		if ( size )
		{
			Matrix heightMap{ size };
			auto max = size - 1;
			auto engine = gen::createRandomEngine( disableRandomSeed );
			generateHeightMap( island
				, engine
				, max
				, size
				, heightMap );

			auto zeroPoint = float( heightRange.percent( 0.0f ) );
			// Generate quads 
			c3d::SubmeshAnimationBuffer submeshBuffers;

			auto transform = [&]( uint32_t v, float s )
			{
				return s * ( float( v ) - float( max ) / 2.0f );
			};

			c3d::Map< uint32_t, uint32_t > vertexMap;
			uint32_t index{};

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					vertexMap.emplace( heightMap.getIndex( x, z ), index );
					++index;
					submeshBuffers.positions.emplace_back( transform( x, xScale ), heightRange.value( heightMap( x, z ) ), transform( z, zScale ) );
					submeshBuffers.texcoords0.emplace_back( float( x ) / uScale, float( z ) / vScale, 0.0f );
				}
			}

			c3d::FaceArray faces;

			for ( auto y = 1u; y < max - 2; y++ )
			{
				for ( auto x = 1u; x < max - 2; x++ )
				{
					faces.emplace_back( heightMap.getIndex( x, y, size - 2 )
						, heightMap.getIndex( x, y + 1, size - 2 )
						, heightMap.getIndex( x + 1, y, size - 2 ) );
					faces.emplace_back( heightMap.getIndex( x + 1, y, size - 2 )
						, heightMap.getIndex( x, y + 1, size - 2 )
						, heightMap.getIndex( x + 1, y + 1, size - 2 ) );
				}
			}

			submeshBuffers.normals.resize( submeshBuffers.positions.size() );
			submeshBuffers.tangents.resize( submeshBuffers.positions.size() );
			c3d::SubmeshUtils::computeNormals( submeshBuffers.positions
				, submeshBuffers.normals
				, faces );
			c3d::SubmeshUtils::computeTangentsFromNormals( submeshBuffers.positions
				, submeshBuffers.texcoords0
				, submeshBuffers.normals
				, submeshBuffers.tangents
				, faces );
			generateBiomes( engine
				, max
				, size
				, heatOffset
				, zeroPoint
				, heightMap
				, m_biomes
				, vertexMap
				, submeshBuffers );

			auto submesh = mesh.createSubmesh();
			submesh->createComponent< c3d::PositionsComponent >()->getData().setData( submeshBuffers.positions );
			submesh->createComponent< c3d::Texcoords0Component >()->getData().setData( submeshBuffers.texcoords0 );
			submesh->createComponent< c3d::NormalsComponent >()->getData().setData( submeshBuffers.normals );
			submesh->createComponent< c3d::TangentsComponent >()->getData().setData( submeshBuffers.tangents );
			submesh->createComponent< c3d::TriFaceMapping >()->getData().setData( c3d::move( faces ) );
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( !submeshBuffers.colours.empty() )
			{
				submesh->createComponent< c3d::ColoursComponent >()->getData().setData( submeshBuffers.colours );
			}

			if ( !submeshBuffers.passMasks.empty() )
			{
				submesh->createComponent< c3d::PassMasksComponent >()->getData().setData( submeshBuffers.passMasks );
			}
		}
	}
}
