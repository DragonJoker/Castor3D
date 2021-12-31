#include "AtmosphereScattering/AtmosphereScatteringPrerequisites.hpp"

#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace atmosphere_scattering
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
				, uint32_t( castor3d::CSCNSection::eScene )
				, cuT( "atmospheric_scattering" )
				, &parserAtmosphereScattering );
			addParser( result, uint32_t( AtmosphereSection::eRoot )
				, cuT( "}" )
				, &parserAtmosphereScatteringEnd );

			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "transmittance" )
				, &parserTransmittance );
			addParser( result
				, uint32_t( AtmosphereSection::eTransmittance )
				, cuT( "image" )
				, &parserTransmittanceImage
				, { castor::makeParameter< castor::ParameterType::ePath >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eTransmittance )
				, cuT( "dimensions" )
				, &parserTransmittanceDimensions
				, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eTransmittance )
				, cuT( "format" )
				, &parserTransmittanceFormat
				, { castor::makeParameter< castor::ParameterType::ePixelFormat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eTransmittance )
				, cuT( "}" )
				, &parserTransmittanceEnd );

			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "inscatter" )
				, &parserInscatter );
			addParser( result
				, uint32_t( AtmosphereSection::eInscatter )
				, cuT( "image" )
				, &parserInscatterImage
				, { castor::makeParameter< castor::ParameterType::ePath >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eInscatter )
				, cuT( "dimensions" )
				, &parserInscatterDimensions
				, { castor::makeParameter< castor::ParameterType::ePoint3U >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eInscatter )
				, cuT( "format" )
				, &parserInscatterFormat
				, { castor::makeParameter< castor::ParameterType::ePixelFormat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eInscatter )
				, cuT( "}" )
				, &parserInscatterEnd );

			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "irradiance" )
				, &parserIrradiance );
			addParser( result
				, uint32_t( AtmosphereSection::eIrradiance )
				, cuT( "image" )
				, &parserIrradianceImage
				, { castor::makeParameter< castor::ParameterType::ePath >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eIrradiance )
				, cuT( "dimensions" )
				, &parserIrradianceDimensions
				, { castor::makeParameter< castor::ParameterType::ePoint3U >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eIrradiance )
				, cuT( "format" )
				, &parserIrradianceFormat
				, { castor::makeParameter< castor::ParameterType::ePixelFormat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eIrradiance )
				, cuT( "}" )
				, &parserIrradianceEnd );

			return result;
		}

		static castor::StrUInt32Map createSections()
		{
			return
			{
				{ uint32_t( AtmosphereSection::eRoot ), PluginType },
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

#ifndef CU_PlatformWindows
#	define C3D_AtmosphereScattering_API
#else
#	ifdef AtmosphereScattering_EXPORTS
#		define C3D_AtmosphereScattering_API __declspec( dllexport )
#	else
#		define C3D_AtmosphereScattering_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	C3D_AtmosphereScattering_API void getRequiredVersion( castor3d::Version * p_version );
	C3D_AtmosphereScattering_API void getType( castor3d::PluginType * p_type );
	C3D_AtmosphereScattering_API void getName( char const ** p_name );
	C3D_AtmosphereScattering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * p_plugin );
	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine );

	C3D_AtmosphereScattering_API void getRequiredVersion( castor3d::Version * version )
	{
		*version = castor3d::Version();
	}

	C3D_AtmosphereScattering_API void getType( castor3d::PluginType * type )
	{
		*type = castor3d::PluginType::eGeneric;
	}

	C3D_AtmosphereScattering_API void getName( char const ** name )
	{
		*name = atmosphere_scattering::PluginName.c_str();
	}

	C3D_AtmosphereScattering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin )
	{
		engine->registerParsers( atmosphere_scattering::PluginType
			, atmosphere_scattering::parser::createParsers()
			, atmosphere_scattering::parser::createSections()
			, &atmosphere_scattering::parser::createContext );
	}

	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterParsers( atmosphere_scattering::PluginType );
	}
}
