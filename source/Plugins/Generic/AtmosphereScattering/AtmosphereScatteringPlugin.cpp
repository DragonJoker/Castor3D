#include "AtmosphereScattering/AtmosphereScatteringPrerequisites.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereLightingModel.hpp"
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
				, cuT( "node" )
				, &parserNode
				, { castor::makeParameter< castor::ParameterType::eName >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "transmittanceResolution" )
				, &parserTransmittanceResolution
				, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "multiScatterResolution" )
				, &parserMultiScatterResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "atmosphereVolumeResolution" )
				, &parserAtmosphereVolumeResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "skyViewResolution" )
				, &parserSkyViewResolution
				, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "sunIlluminance" )
				, &parserSunIlluminance
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "sunIlluminanceScale" )
				, &parserSunIlluminanceScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "rayMarchMinSPP" )
				, &parserRayMarchMinSPP
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "rayMarchMaxSPP" )
				, &parserRayMarchMaxSPP
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "multipleScatteringFactor" )
				, &parserMultipleScatteringFactor
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "solarIrradiance" )
				, &parserSolarIrradiance
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "sunAngularRadius" )
				, &parserSunAngularRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "absorptionExtinction" )
				, &parserAbsorptionExtinction
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "maxSunZenithAngle" )
				, &parserMaxSunZenithAngle
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "rayleighScattering" )
				, &parserRayleighScattering
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "mieScattering" )
				, &parserMieScattering
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "miePhaseFunctionG" )
				, &parserMiePhaseFunctionG
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "mieExtinction" )
				, &parserMieExtinction
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "bottomRadius" )
				, &parserBottomRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "topRadius" )
				, &parserTopRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "groundAlbedo" )
				, &parserGroundAlbedo
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "minRayleighDensity" )
				, &parserMinRayleighDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "maxRayleighDensity" )
				, &parserMaxRayleighDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "minMieDensity" )
				, &parserMinMieDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "maxMieDensity" )
				, &parserMaxMieDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "minAbsorptionDensity" )
				, &parserMinAbsorptionDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "maxAbsorptionDensity" )
				, &parserMaxAbsorptionDensity );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "weather" )
				, &parserWeather );
			addParser( result
				, uint32_t( AtmosphereSection::eRoot )
				, cuT( "clouds" )
				, &parserClouds );
			addParser( result
				, uint32_t( AtmosphereSection::eDensity )
				, cuT( "layerWidth" )
				, &parserDensityLayerWidth
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eDensity )
				, cuT( "expTerm" )
				, &parserDensityExpTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eDensity )
				, cuT( "expScale" )
				, &parserDensityExpScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eDensity )
				, cuT( "linearTerm" )
				, &parserDensityLinearTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eDensity )
				, cuT( "constantTerm" )
				, &parserDensityConstantTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "worleyResolution" )
				, &parserWorleyResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "perlinWorleyResolution" )
				, &parserPerlinWorleyResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "weatherResolution" )
				, &parserWeatherResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "curlResolution" )
				, &parserCurlResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "amplitude" )
				, &parserWeatherAmplitude
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "frequency" )
				, &parserWeatherFrequency
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "scale" )
				, &parserWeatherScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eWeather )
				, cuT( "octaves" )
				, &parserWeatherOctaves
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "windDirection" )
				, &parserCloudsWindDirection
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "speed" )
				, &parserCloudsSpeed
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "coverage" )
				, &parserCloudsCoverage
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "cripsiness" )
				, &parserCloudsCrispiness
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "curliness" )
				, &parserCloudsCurliness
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "density" )
				, &parserCloudsDensity
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "absorption" )
				, &parserCloudsAbsorption
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "innerRadius" )
				, &parserCloudsInnerRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "outerRadius" )
				, &parserCloudsOuterRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "topColour" )
				, &parserCloudsTopColour
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "bottomColour" )
				, &parserCloudsBottomColour
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "enablePowder" )
				, &parserCloudsEnablePowder
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParser( result
				, uint32_t( AtmosphereSection::eClouds )
				, cuT( "topOffset" )
				, &parserCloudsTopOffset
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );

			return result;
		}

		static castor::StrUInt32Map createSections()
		{
			return
			{
				{ uint32_t( AtmosphereSection::eRoot ), PluginType },
				{ uint32_t( AtmosphereSection::eDensity ), "density" },
				{ uint32_t( AtmosphereSection::eWeather ), "weather" },
				{ uint32_t( AtmosphereSection::eClouds ), "clouds" },
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
		engine->registerLightingModel( atmosphere_scattering::AtmospherePhongLightingModel::getName()
			, &atmosphere_scattering::AtmospherePhongLightingModel::create );
		engine->registerLightingModel( atmosphere_scattering::AtmosphereBlinnPhongLightingModel::getName()
			, &atmosphere_scattering::AtmosphereBlinnPhongLightingModel::create );
		engine->registerLightingModel( atmosphere_scattering::AtmospherePbrLightingModel::getName()
			, &atmosphere_scattering::AtmospherePbrLightingModel::create );
		engine->registerParsers( atmosphere_scattering::PluginType
			, atmosphere_scattering::parser::createParsers()
			, atmosphere_scattering::parser::createSections()
			, &atmosphere_scattering::parser::createContext );
		engine->registerBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name
			, atmosphere_scattering::AtmosphereBackgroundModel::create );
	}

	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine )
	{
		engine->unregisterBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name );
		engine->unregisterParsers( atmosphere_scattering::PluginType );
		engine->unregisterLightingModel( atmosphere_scattering::AtmospherePhongLightingModel::getName() );
		engine->unregisterLightingModel( atmosphere_scattering::AtmosphereBlinnPhongLightingModel::getName() );
		engine->unregisterLightingModel( atmosphere_scattering::AtmospherePbrLightingModel::getName() );
	}
}
