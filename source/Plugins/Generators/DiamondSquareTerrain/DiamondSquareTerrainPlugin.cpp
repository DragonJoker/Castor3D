#include "DiamondSquareTerrain/DiamondSquareTerrain.hpp"

#include "DiamondSquareTerrain/DiamondSquare_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

#ifndef CU_PlatformWindows
#	define C3D_DiamondSquareTerrain_API
#else
#	ifdef DiamondSquareTerrain_EXPORTS
#		define C3D_DiamondSquareTerrain_API __declspec( dllexport )
#	else
#		define C3D_DiamondSquareTerrain_API __declspec( dllimport )
#	endif
#endif

namespace diamond_square_terrain
{
	namespace parser
	{
		static castor::AttributeParsers createParsers()
		{
			castor::AttributeParsers result;

			addParserT( result
				, castor3d::CSCNSection::eMesh
				, DiamondSquareSection::eRoot
				, Generator::Type
				, &parserDiamondSquareTerrain );
			addParserT( result
				, DiamondSquareSection::eRoot
				, castor3d::CSCNSection::eMesh
				, cuT( "}" )
				, &parserDiamondSquareTerrainEnd );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamRandomSeed
				, &parserRandomSeed
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamIsland
				, &parserIsland
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamXzScale
				, &parserXzScale
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamUvScale
				, &parserUvScale
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamHeightRange
				, &parserHeightRange
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamDetail
				, &parserDetail
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, Generator::ParamHeatOffset
				, &parserHeatOffset
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, DiamondSquareSection::eRoot
				, DiamondSquareSection::eBiome
				, Generator::Biome
				, &parserBiome
				, { castor::makeParameter< castor::ParameterType::eName >() } );
			addParserT( result
				, DiamondSquareSection::eBiome
				, DiamondSquareSection::eRoot
				, cuT( "}" )
				, &parserBiomeEnd );
			addParserT( result
				, DiamondSquareSection::eBiome
				, Generator::BiomeRange
				, &parserBiomeRange
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eBiome
				, Generator::BiomeLowSteepness
				, &parserBiomeLowSteepness
				, { castor::makeParameter< castor::ParameterType::eUInt32 >()
					, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eBiome
				, Generator::BiomeMediumSteepness
				, &parserBiomeMedSteepness
				, { castor::makeParameter< castor::ParameterType::eUInt32 >()
					, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParserT( result
				, DiamondSquareSection::eBiome
				, Generator::BiomeHighSteepness
				, &parserBiomeHigSteepness
				, { castor::makeParameter< castor::ParameterType::eUInt32 >()
					, castor::makeParameter< castor::ParameterType::ePoint2F >() } );

			return result;
		}

		static castor::StrUInt32Map createSections()
		{
			return
			{
				{ uint32_t( DiamondSquareSection::eRoot ), Generator::Type },
				{ uint32_t( DiamondSquareSection::eBiome ), Generator::Biome },
			};
		}
	}
}

extern "C"
{
	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * version );
	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * type );
	C3D_DiamondSquareTerrain_API void getName( char const ** name );
	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine );

	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGenerator;
	}

	C3D_DiamondSquareTerrain_API void getName( char const ** name )
	{
		*name = diamond_square_terrain::Generator::Name.c_str();
	}

	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerParsers( diamond_square_terrain::Generator::Type
			, diamond_square_terrain::parser::createParsers()
			, diamond_square_terrain::parser::createSections()
			, nullptr );
		engine->getMeshFactory().registerType( diamond_square_terrain::Generator::Type
			, &diamond_square_terrain::Generator::create );
	}

	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getMeshFactory().unregisterType( diamond_square_terrain::Generator::Type );
		engine->unregisterParsers( diamond_square_terrain::Generator::Type );
	}
}
