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

			addParser( result
				, uint32_t( castor3d::CSCNSection::eMesh )
				, Generator::Type
				, &parserDiamondSquareTerrain );
			addParser( result, uint32_t( DiamondSquareSection::eRoot )
				, cuT( "}" )
				, &parserDiamondSquareTerrainEnd );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamRandomSeed
				, &parserRandomSeed
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamIsland
				, &parserIsland
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamXzScale
				, &parserXzScale
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamUvScale
				, &parserUvScale
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamHeightRange
				, &parserHeightRange
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamDetail
				, &parserDetail
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamHeatOffset
				, &parserHeatOffset
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::Biome
				, &parserBiome
				, { castor::makeParameter< castor::ParameterType::eName >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eBiome )
				, cuT( "}" )
				, &parserBiomeEnd );
			addParser( result
				, uint32_t( DiamondSquareSection::eBiome )
				, Generator::BiomeRange
				, &parserBiomeRange
				, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eBiome )
				, Generator::BiomeLowSteepness
				, &parserBiomeLowSteepness
				, { castor::makeParameter< castor::ParameterType::eUInt32 >()
					, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eBiome )
				, Generator::BiomeMediumSteepness
				, &parserBiomeMedSteepness
				, { castor::makeParameter< castor::ParameterType::eUInt32 >()
					, castor::makeParameter< castor::ParameterType::ePoint2F >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eBiome )
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

		static void * createContext( castor::FileParserContext & context )
		{
			auto userContext = new ParserContext;
			userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
			return userContext;
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
			, &diamond_square_terrain::parser::createContext );
		engine->getMeshFactory().registerType( diamond_square_terrain::Generator::Type
			, &diamond_square_terrain::Generator::create );
	}

	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine )
	{
		engine->getMeshFactory().unregisterType( diamond_square_terrain::Generator::Type );
		engine->unregisterParsers( diamond_square_terrain::Generator::Type );
	}
}
