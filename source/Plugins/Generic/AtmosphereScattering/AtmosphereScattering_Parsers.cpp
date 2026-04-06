#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsVolumePlugin.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentRegister.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace atmosphere_scattering
{
	namespace parse
	{
		struct AtmosphereContext
		{
			c3d::SceneContext * sceneContext{};
			c3d::SceneRPtr scene{};
			c3d::Point2ui transmittanceDim{ 256u, 64u };
			uint32_t multiScatterDim{ 32u };
			uint32_t atmosphereVolumeDim{ 32u };
			c3d::Point2ui skyViewDim{ 192u, 108u };
			uint32_t worleyDim{ 32u };
			uint32_t perlinWorleyDim{ 128u };
			uint32_t curlDim{ 128u };
			uint32_t weatherDim{ 1024u };
			AtmosphereScatteringConfig atmosphere{};
			WeatherConfig weather{};
			CloudsConfig clouds{};
			DensityProfileLayer * densityLayer{};
			AtmosphereBackgroundUPtr background{};
			CloudsVolumePlugin * plugin{};
		};

		static c3d::String getPrefix( AtmosphereContext const & context )
		{
			return getPrefix( *context.sceneContext );
		}

		enum class AtmosphereSection
			: c3d::SectionId
		{
			eRoot = c3d::makeSectionName( 'A', 'T', 'S', 'C' ),
			eDensity = c3d::makeSectionName( 'A', 'T', 'D', 'S' ),
			eWeather = c3d::makeSectionName( 'A', 'T', 'W', 'T' ),
			eClouds = c3d::makeSectionName( 'A', 'T', 'C', 'L' ),
		};
		
		static CU_ImplementAttributeParserNewBlock( parserAtmosphereScattering, c3d::SceneContext, AtmosphereContext )
		{
			newBlockContext->sceneContext = blockContext;
			newBlockContext->scene = blockContext->scene;
			newBlockContext->plugin = &static_cast< CloudsVolumePlugin & >( blockContext->scene->getEngine()->getVolumeComponentsRegister().getPlugin( CloudsVolumePlugin::TypeName ) );
			newBlockContext->plugin->enable();
			newBlockContext->background = c3d::makeUnique< AtmosphereBackground >( *blockContext->scene->getEngine()
				, *blockContext->scene
				, *newBlockContext->plugin );
		}
		CU_EndAttributePushNewBlock( AtmosphereSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserAtmosphereScatteringEnd, AtmosphereContext )
		{
			if ( !blockContext->background->getSunNode() )
				CU_ParsingError( cuT( "No node to attach the sun to..." ) );
			else if ( !blockContext->background->getPlanetNode() )
				CU_ParsingError( cuT( "No node to attach the planet to..." ) );
			else
			{
				blockContext->atmosphere.multiScatteringLUTRes = float( blockContext->multiScatterDim );
				blockContext->background->setAtmosphereCfg( c3d::move( blockContext->atmosphere ) );
				blockContext->background->setCloudsCfg( c3d::move( blockContext->clouds ) );
				blockContext->background->setWeatherCfg( c3d::move( blockContext->weather ) );
				blockContext->background->getVolumeData().loadTransmittance( blockContext->transmittanceDim );
				blockContext->background->getVolumeData().loadMultiScatter( blockContext->multiScatterDim );
				blockContext->background->getVolumeData().loadAtmosphereVolume( blockContext->atmosphereVolumeDim );
				blockContext->background->getVolumeData().loadSkyView( blockContext->skyViewDim );
				blockContext->background->getVolumeData().loadWorley( blockContext->worleyDim );
				blockContext->background->getVolumeData().loadPerlinWorley( blockContext->perlinWorleyDim );
				blockContext->background->getVolumeData().loadCurl( blockContext->curlDim );
				blockContext->background->getVolumeData().loadWeather( blockContext->weatherDim );
				blockContext->scene->setBackground( c3d::ptrRefCast< c3d::SceneBackground >( blockContext->background ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSunNode, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
			{
				auto name = c3d::getPrefixedName( params[0]->get< c3d::String >(), *blockContext );

				if ( auto node = blockContext->scene->findSceneNode( name ) )
					blockContext->background->setSunNode( *node );
				else
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPlanetNode, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
			{
				auto name = c3d::getPrefixedName( params[0]->get< c3d::String >(), *blockContext );

				if ( auto node = blockContext->scene->findSceneNode( name ) )
					blockContext->background->setPlanetNode( *node );
				else
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTransmittanceResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->transmittanceDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMultiScatterResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->multiScatterDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAtmosphereVolumeResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphereVolumeDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyViewResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->skyViewDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunIlluminance, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.sunIlluminance );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunIlluminanceScale, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.sunIlluminanceScale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayMarchMinSPP, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				blockContext->atmosphere.rayMarchMinMaxSPP[0] = float( params[0]->get< uint32_t >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayMarchMaxSPP, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				blockContext->atmosphere.rayMarchMinMaxSPP[1] = float( params[0]->get< uint32_t >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMultipleScatteringFactor, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.multipleScatteringFactor );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSolarIrradiance, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.solarIrradiance );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunAngularRadius, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.sunAngularRadius );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAbsorptionExtinction, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.absorptionExtinction );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxSunZenithAngle, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				blockContext->atmosphere.muSMin = float( c3d::Angle::fromDegrees( params[0]->get< float >() ).cos() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayleighScattering, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.rayleighScattering );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMieScattering, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.mieScattering );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMiePhaseFunctionG, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.miePhaseFunctionG );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMieExtinction, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.mieExtinction );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBottomRadius, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.bottomRadius );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTopRadius, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.topRadius );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGroundAlbedo, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->atmosphere.groundAlbedo );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMinRayleighDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.rayleighDensity[0];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserMaxRayleighDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.rayleighDensity[1];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserMinMieDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.mieDensity[0];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserMaxMieDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.mieDensity[1];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserMinAbsorptionDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.absorptionDensity[0];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserMaxAbsorptionDensity, AtmosphereContext )
		{
			blockContext->densityLayer = &blockContext->atmosphere.absorptionDensity[1];
		}
		CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

		static CU_ImplementAttributeParserBlock( parserDensityLayerWidth, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->densityLayer->layerWidth );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityExpTerm, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->densityLayer->expTerm );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityExpScale, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->densityLayer->expScale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityLinearTerm, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->densityLayer->linearTerm );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityConstantTerm, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->densityLayer->constantTerm );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityEnd, AtmosphereContext )
		{
			blockContext->densityLayer = nullptr;
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserWeather, AtmosphereContext )
		{
			// Nothing else to do than to push the block
		}
		CU_EndAttributePushBlock( AtmosphereSection::eWeather, blockContext )

		static CU_ImplementAttributeParserBlock( parserWeatherAmplitude, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->weather.perlinAmplitude );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherFrequency, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->weather.perlinFrequency );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherScale, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->weather.perlinScale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherOctaves, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->weather.perlinOctaves );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWorleyResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->worleyDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPerlinWorleyResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->perlinWorleyDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCurlResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->curlDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherResolution, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->weatherDim );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClouds, AtmosphereContext )
		{
			// Nothing else to do than to push the block
		}
		CU_EndAttributePushBlock( AtmosphereSection::eClouds, blockContext )

		static CU_ImplementAttributeParserBlock( parserCloudsWindDirection, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.windDirection );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsSpeed, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.speed );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCoverage, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.coverage );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCrispiness, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.crispiness );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCurliness, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.curliness );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsDensity, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.density );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsAbsorption, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.absorption );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsInnerRadius, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.innerRadius );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsOuterRadius, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.outerRadius );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsTopColour, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.colorTop );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsBottomColour, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.colorBottom );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsEnablePowder, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				blockContext->clouds.enablePowder = params[0]->get< bool >() ? 1 : 0;
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsTopOffset, AtmosphereContext )
		{
			if ( params.empty() )
				CU_ParsingError( cuT( "Missing parameter" ) );
			else
				params[0]->get( blockContext->clouds.topOffset );
		}
		CU_EndAttribute()
	}

	c3d::AttributeParsers createParsers()
	{
		c3d::AttributeParsers result;

		addParserT( result
			, c3d::CSCNSection::eScene
			, parse::AtmosphereSection::eRoot
			, cuT( "atmospheric_scattering" )
			, &parse::parserAtmosphereScattering );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, c3d::CSCNSection::eScene
			, cuT( "}" )
			, &parse::parserAtmosphereScatteringEnd );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunNode" )
			, &parse::parserSunNode
			, { c3d::makeParameter< c3d::ParameterType::eName >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "planetNode" )
			, &parse::parserPlanetNode
			, { c3d::makeParameter< c3d::ParameterType::eName >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "transmittanceResolution" )
			, &parse::parserTransmittanceResolution
			, { c3d::makeParameter< c3d::ParameterType::ePoint2U >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "multiScatterResolution" )
			, &parse::parserMultiScatterResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "atmosphereVolumeResolution" )
			, &parse::parserAtmosphereVolumeResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "skyViewResolution" )
			, &parse::parserSkyViewResolution
			, { c3d::makeParameter< c3d::ParameterType::ePoint2U >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunIlluminance" )
			, &parse::parserSunIlluminance
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunIlluminanceScale" )
			, &parse::parserSunIlluminanceScale
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayMarchMinSPP" )
			, &parse::parserRayMarchMinSPP
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayMarchMaxSPP" )
			, &parse::parserRayMarchMaxSPP
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "multipleScatteringFactor" )
			, &parse::parserMultipleScatteringFactor
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "solarIrradiance" )
			, &parse::parserSolarIrradiance
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunAngularRadius" )
			, &parse::parserSunAngularRadius
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "absorptionExtinction" )
			, &parse::parserAbsorptionExtinction
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "maxSunZenithAngle" )
			, &parse::parserMaxSunZenithAngle
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayleighScattering" )
			, &parse::parserRayleighScattering
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "mieScattering" )
			, &parse::parserMieScattering
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "miePhaseFunctionG" )
			, &parse::parserMiePhaseFunctionG
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "mieExtinction" )
			, &parse::parserMieExtinction
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "bottomRadius" )
			, &parse::parserBottomRadius
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "topRadius" )
			, &parse::parserTopRadius
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "groundAlbedo" )
			, &parse::parserGroundAlbedo
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "minRayleighDensity" )
			, &parse::parserMinRayleighDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "maxRayleighDensity" )
			, &parse::parserMaxRayleighDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "minMieDensity" )
			, &parse::parserMinMieDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "maxMieDensity" )
			, &parse::parserMaxMieDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "minAbsorptionDensity" )
			, &parse::parserMinAbsorptionDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eDensity
			, cuT( "maxAbsorptionDensity" )
			, &parse::parserMaxAbsorptionDensity );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eWeather
			, cuT( "weather" )
			, &parse::parserWeather );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, parse::AtmosphereSection::eClouds
			, cuT( "clouds" )
			, &parse::parserClouds );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "layerWidth" )
			, &parse::parserDensityLayerWidth
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "expTerm" )
			, &parse::parserDensityExpTerm
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "expScale" )
			, &parse::parserDensityExpScale
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "linearTerm" )
			, &parse::parserDensityLinearTerm
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "constantTerm" )
			, &parse::parserDensityConstantTerm
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &parse::parserDensityEnd );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "worleyResolution" )
			, &parse::parserWorleyResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "perlinWorleyResolution" )
			, &parse::parserPerlinWorleyResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "weatherResolution" )
			, &parse::parserWeatherResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "curlResolution" )
			, &parse::parserCurlResolution
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "amplitude" )
			, &parse::parserWeatherAmplitude
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "frequency" )
			, &parse::parserWeatherFrequency
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "scale" )
			, &parse::parserWeatherScale
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "octaves" )
			, &parse::parserWeatherOctaves
			, { c3d::makeParameter< c3d::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &c3d::parserDefaultEnd );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "windDirection" )
			, &parse::parserCloudsWindDirection
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "speed" )
			, &parse::parserCloudsSpeed
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "coverage" )
			, &parse::parserCloudsCoverage
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "crispiness" )
			, &parse::parserCloudsCrispiness
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "curliness" )
			, &parse::parserCloudsCurliness
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "density" )
			, &parse::parserCloudsDensity
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "absorption" )
			, &parse::parserCloudsAbsorption
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "innerRadius" )
			, &parse::parserCloudsInnerRadius
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "outerRadius" )
			, &parse::parserCloudsOuterRadius
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "topColour" )
			, &parse::parserCloudsTopColour
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "bottomColour" )
			, &parse::parserCloudsBottomColour
			, { c3d::makeParameter< c3d::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "enablePowder" )
			, &parse::parserCloudsEnablePowder
			, { c3d::makeParameter< c3d::ParameterType::eBool >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "topOffset" )
			, &parse::parserCloudsTopOffset
			, { c3d::makeParameter< c3d::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &c3d::parserDefaultEnd );

		return result;
	}

	c3d::StrSectionIdMap createSections()
	{
		return
		{
			{ c3d::SectionId( parse::AtmosphereSection::eRoot ), AtmosphereBackgroundModel::PluginType },
			{ c3d::SectionId( parse::AtmosphereSection::eDensity ), cuT( "density" ) },
			{ c3d::SectionId( parse::AtmosphereSection::eWeather ), cuT( "weather" ) },
			{ c3d::SectionId( parse::AtmosphereSection::eClouds ), cuT( "clouds" ) },
		};
	}
}
