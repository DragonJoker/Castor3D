#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include "DiamondSquareTerrain/GenerateHeightMap.hpp"
#include "DiamondSquareTerrain/GenerateBiomes.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
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

	castor::String const Generator::Type = cuT( "diamond_square_terrain" );
	castor::String const Generator::Name = cuT( "Diamond Square Terrain Generator" );
	castor::String const Generator::Biome = cuT( "biome" );
	castor::String const Generator::BiomeRange = cuT( "range" );
	castor::String const Generator::BiomeLowSteepness = cuT( "lowSteepness" );
	castor::String const Generator::BiomeMediumSteepness = cuT( "mediumSteepness" );
	castor::String const Generator::BiomeHighSteepness = cuT( "highSteepness" );
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

	castor3d::MeshGeneratorUPtr Generator::create()
	{
		return castor::makeUniqueDerived< castor3d::MeshGenerator, Generator >();
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
		castor::Range< float > heightRange{ -500.0f, 500.0f };
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
			heightRange = castor::makeRange( heightRange.getMin()
				, castor::string::toFloat( param ) );
		}

		if ( parameters.get( ParamYMax, param ) )
		{
			heightRange = castor::makeRange( castor::string::toFloat( param )
				, heightRange.getMax() );
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
			Matrix heightMap{ size };
			auto max = size - 1;
			auto engine = gen::createRandomEngine( disableRandomSeed );
			generateHeightMap( island
				, engine
				, max
				, size
				, heightMap );

			auto zeroPoint = heightRange.percent( 0.0f );
			// Generate quads 
			castor3d::SubmeshAnimationBuffer submeshBuffers;

			auto transform = [&]( uint32_t v, float s )
			{
				return s * ( float( v ) - float( max ) / 2.0f );
			};

			std::map< uint32_t, uint32_t > vertexMap;
			uint32_t index{};

			for ( auto z = 1u; z < max; z++ )
			{
				for ( auto x = 1u; x < max; x++ )
				{
					vertexMap.emplace( heightMap.getIndex( x, z ), index++ );
					submeshBuffers.positions.emplace_back( transform( x, xScale ), heightRange.value( heightMap( x, z ) ), transform( z, zScale ) );
					submeshBuffers.texcoords0.emplace_back( float( x ) / uScale, float( z ) / vScale, 0.0f );
				}
			}

			castor3d::FaceArray faces;

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
			castor3d::SubmeshUtils::computeNormals( submeshBuffers.positions
				, submeshBuffers.normals
				, faces );
			castor3d::SubmeshUtils::computeTangentsFromNormals( submeshBuffers.positions
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
				, faces
				, vertexMap
				, submeshBuffers );

			auto submesh = mesh.createSubmesh();
			submesh->createComponent< castor3d::PositionsComponent >()->setData( std::move( submeshBuffers.positions ) );
			submesh->createComponent< castor3d::Texcoords0Component >()->setData( std::move( submeshBuffers.texcoords0 ) );
			submesh->createComponent< castor3d::NormalsComponent >()->setData( std::move( submeshBuffers.normals ) );
			submesh->createComponent< castor3d::TangentsComponent >()->setData( std::move( submeshBuffers.tangents ) );
			submesh->createComponent< castor3d::TriFaceMapping >()->setData( std::move( faces ) );

			if ( !submeshBuffers.colours.empty() )
			{
				submesh->createComponent< castor3d::ColoursComponent >()->setData( std::move( submeshBuffers.colours ) );
			}

			if ( !submeshBuffers.passMasks.empty() )
			{
				submesh->createComponent< castor3d::PassMasksComponent >()->setData( std::move( submeshBuffers.passMasks ) );
			}
		}
	}
}
