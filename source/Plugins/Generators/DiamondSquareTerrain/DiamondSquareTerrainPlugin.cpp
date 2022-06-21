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
		static void parseError( castor::String const & error )
		{
			castor::StringStream stream{ castor::makeStringStream() };
			stream << cuT( "Error, : " ) << error;
			castor::Logger::logError( stream.str() );
		}

		static void addParser( castor::AttributeParsers & parsers
			, uint32_t section
			, castor::String const & name
			, castor::ParserFunction function
			, castor::ParserParameterArray && array = castor::ParserParameterArray{} )
		{
			auto nameIt = parsers.find( name );

			if ( nameIt != parsers.end()
				&& nameIt->second.find( section ) != nameIt->second.end() )
			{
				parseError( cuT( "Parser " ) + name + cuT( " for section " ) + castor::string::toString( section ) + cuT( " already exists." ) );
			}
			else
			{
				parsers[name][section] = { function, array };
			}
		}

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
				, Generator::ParamRoughness
				, &parserRoughness
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamRandomSeed
				, &parserRandomSeed
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamScale
				, &parserScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamDetail
				, &parserDetail
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( DiamondSquareSection::eRoot )
				, Generator::ParamMinY
				, &parserMinY
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );

			return result;
		}

		static castor::StrUInt32Map createSections()
		{
			return
			{
				{ uint32_t( DiamondSquareSection::eRoot ), Generator::Type },
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
	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * p_type );
	C3D_DiamondSquareTerrain_API void getName( char const ** p_name );
	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_DiamondSquareTerrain_API void OnUnload( castor3d::Engine * engine );

	C3D_DiamondSquareTerrain_API void getRequiredVersion( castor3d::Version * p_version )
	{
		*p_version = castor3d::Version();
	}

	C3D_DiamondSquareTerrain_API void getType( castor3d::PluginType * p_type )
	{
		*p_type = castor3d::PluginType::eGenerator;
	}

	C3D_DiamondSquareTerrain_API void getName( char const ** p_name )
	{
		*p_name = diamond_square_terrain::Generator::Name.c_str();
	}

	C3D_DiamondSquareTerrain_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin )
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
