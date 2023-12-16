#include "AtmosphereScattering/AtmosphereScatteringPrerequisites.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereLightingModel.hpp"
#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

namespace atmosphere_scattering
{
	namespace parser
	{
		static castor::AttributeParsers createParsers()
		{
			castor::AttributeParsers result;

			addParserT( result
				, castor3d::CSCNSection::eScene
				, AtmosphereSection::eRoot
				, cuT( "atmospheric_scattering" )
				, &parserAtmosphereScattering );
			addParserT( result
				, AtmosphereSection::eRoot
				, castor3d::CSCNSection::eScene
				, cuT( "}" )
				, &parserAtmosphereScatteringEnd );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "sunNode" )
				, &parserSunNode
				, { castor::makeParameter< castor::ParameterType::eName >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "planetNode" )
				, &parserPlanetNode
				, { castor::makeParameter< castor::ParameterType::eName >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "transmittanceResolution" )
				, &parserTransmittanceResolution
				, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "multiScatterResolution" )
				, &parserMultiScatterResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "atmosphereVolumeResolution" )
				, &parserAtmosphereVolumeResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "skyViewResolution" )
				, &parserSkyViewResolution
				, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "sunIlluminance" )
				, &parserSunIlluminance
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "sunIlluminanceScale" )
				, &parserSunIlluminanceScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "rayMarchMinSPP" )
				, &parserRayMarchMinSPP
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "rayMarchMaxSPP" )
				, &parserRayMarchMaxSPP
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "multipleScatteringFactor" )
				, &parserMultipleScatteringFactor
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "solarIrradiance" )
				, &parserSolarIrradiance
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "sunAngularRadius" )
				, &parserSunAngularRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "absorptionExtinction" )
				, &parserAbsorptionExtinction
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "maxSunZenithAngle" )
				, &parserMaxSunZenithAngle
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "rayleighScattering" )
				, &parserRayleighScattering
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "mieScattering" )
				, &parserMieScattering
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "miePhaseFunctionG" )
				, &parserMiePhaseFunctionG
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "mieExtinction" )
				, &parserMieExtinction
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "bottomRadius" )
				, &parserBottomRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "topRadius" )
				, &parserTopRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, cuT( "groundAlbedo" )
				, &parserGroundAlbedo
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "minRayleighDensity" )
				, &parserMinRayleighDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "maxRayleighDensity" )
				, &parserMaxRayleighDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "minMieDensity" )
				, &parserMinMieDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "maxMieDensity" )
				, &parserMaxMieDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "minAbsorptionDensity" )
				, &parserMinAbsorptionDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eDensity
				, cuT( "maxAbsorptionDensity" )
				, &parserMaxAbsorptionDensity );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eWeather
				, cuT( "weather" )
				, &parserWeather );
			addParserT( result
				, AtmosphereSection::eRoot
				, AtmosphereSection::eClouds
				, cuT( "clouds" )
				, &parserClouds );
			addParserT( result
				, AtmosphereSection::eDensity
				, cuT( "layerWidth" )
				, &parserDensityLayerWidth
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eDensity
				, cuT( "expTerm" )
				, &parserDensityExpTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eDensity
				, cuT( "expScale" )
				, &parserDensityExpScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eDensity
				, cuT( "linearTerm" )
				, &parserDensityLinearTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eDensity
				, cuT( "constantTerm" )
				, &parserDensityConstantTerm
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eDensity
				, AtmosphereSection::eRoot
				, cuT( "}" )
				, &castor3d::parserdefaultEnd );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "worleyResolution" )
				, &parserWorleyResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "perlinWorleyResolution" )
				, &parserPerlinWorleyResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "weatherResolution" )
				, &parserWeatherResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "curlResolution" )
				, &parserCurlResolution
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "amplitude" )
				, &parserWeatherAmplitude
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "frequency" )
				, &parserWeatherFrequency
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "scale" )
				, &parserWeatherScale
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, cuT( "octaves" )
				, &parserWeatherOctaves
				, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
			addParserT( result
				, AtmosphereSection::eWeather
				, AtmosphereSection::eRoot
				, cuT( "}" )
				, &castor3d::parserdefaultEnd );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "windDirection" )
				, &parserCloudsWindDirection
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "speed" )
				, &parserCloudsSpeed
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "coverage" )
				, &parserCloudsCoverage
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "crispiness" )
				, &parserCloudsCrispiness
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "curliness" )
				, &parserCloudsCurliness
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "density" )
				, &parserCloudsDensity
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "absorption" )
				, &parserCloudsAbsorption
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "innerRadius" )
				, &parserCloudsInnerRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "outerRadius" )
				, &parserCloudsOuterRadius
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "topColour" )
				, &parserCloudsTopColour
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "bottomColour" )
				, &parserCloudsBottomColour
				, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "enablePowder" )
				, &parserCloudsEnablePowder
				, { castor::makeParameter< castor::ParameterType::eBool >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, cuT( "topOffset" )
				, &parserCloudsTopOffset
				, { castor::makeParameter< castor::ParameterType::eFloat >() } );
			addParserT( result
				, AtmosphereSection::eClouds
				, AtmosphereSection::eRoot
				, cuT( "}" )
				, &castor3d::parserdefaultEnd );

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
	C3D_AtmosphereScattering_API void getRequiredVersion( castor3d::Version * version );
	C3D_AtmosphereScattering_API void getType( castor3d::PluginType * type );
	C3D_AtmosphereScattering_API void getName( char const ** name );
	C3D_AtmosphereScattering_API void OnLoad( castor3d::Engine * engine, castor3d::Plugin * plugin );
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
		auto backgroundModelId = engine->registerBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name
			, atmosphere_scattering::AtmosphereBackgroundModel::create );
		engine->registerPassModel( backgroundModelId
			, { atmosphere_scattering::AtmospherePhongLightingModel::getName()
				, castor3d::PhongPass::create
				, &atmosphere_scattering::AtmospherePhongLightingModel::create
				, false } );
		engine->registerPassModel( backgroundModelId
			, { atmosphere_scattering::AtmospherePbrLightingModel::getName()
				, castor3d::PbrPass::create
				, &atmosphere_scattering::AtmospherePbrLightingModel::create
				, true } );
		engine->registerParsers( atmosphere_scattering::PluginType
			, atmosphere_scattering::parser::createParsers()
			, atmosphere_scattering::parser::createSections()
			, nullptr );
	}

	C3D_AtmosphereScattering_API void OnUnload( castor3d::Engine * engine )
	{
		auto backgroundModelId = engine->unregisterBackgroundModel( atmosphere_scattering::AtmosphereBackgroundModel::Name );
		engine->unregisterParsers( atmosphere_scattering::PluginType );
		engine->unregisterPassModel( backgroundModelId
			, engine->getPassFactory().getNameId( atmosphere_scattering::AtmospherePhongLightingModel::getName() ) );
		engine->unregisterPassModel( backgroundModelId
			, engine->getPassFactory().getNameId( atmosphere_scattering::AtmospherePbrLightingModel::getName() ) );
	}
}
