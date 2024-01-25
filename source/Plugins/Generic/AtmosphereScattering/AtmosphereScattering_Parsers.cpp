#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace atmosphere_scattering
{
	namespace parse
	{
		struct AtmosphereContext
		{
			castor3d::SceneRPtr scene{};
			castor::Point2ui transmittanceDim{ 256u, 64u };
			uint32_t multiScatterDim{ 32u };
			uint32_t atmosphereVolumeDim{ 32u };
			castor::Point2ui skyViewDim{ 192u, 108u };
			uint32_t worleyDim{ 32u };
			uint32_t perlinWorleyDim{ 128u };
			uint32_t curlDim{ 128u };
			uint32_t weatherDim{ 1024u };
			AtmosphereScatteringConfig atmosphere{};
			WeatherConfig weather{};
			CloudsConfig clouds{};
			DensityProfileLayer * densityLayer{};
			AtmosphereBackgroundUPtr background{};
		};

		enum class AtmosphereSection
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'A', 'T', 'S', 'C' ),
			eDensity = CU_MakeSectionName( 'A', 'T', 'D', 'S' ),
			eWeather = CU_MakeSectionName( 'A', 'T', 'W', 'T' ),
			eClouds = CU_MakeSectionName( 'A', 'T', 'C', 'L' ),
		};
		
		static CU_ImplementAttributeParserNewBlock( parserAtmosphereScattering, castor3d::SceneContext, AtmosphereContext )
		{
			newBlockContext->scene = blockContext->scene;
			newBlockContext->background = castor::makeUnique< AtmosphereBackground >( *blockContext->scene->getEngine()
				, *blockContext->scene );
		}
		CU_EndAttributePushNewBlock( AtmosphereSection::eRoot )

		static CU_ImplementAttributeParserBlock( parserAtmosphereScatteringEnd, AtmosphereContext )
		{
			if ( !blockContext->background->getSunNode() )
			{
				CU_ParsingError( cuT( "No node to attach the sun to..." ) );
			}
			else if ( !blockContext->background->getPlanetNode() )
			{
				CU_ParsingError( cuT( "No node to attach the planet to..." ) );
			}
			else
			{
				blockContext->atmosphere.multiScatteringLUTRes = float( blockContext->multiScatterDim );
				blockContext->background->setAtmosphereCfg( castor::move( blockContext->atmosphere ) );
				blockContext->background->setCloudsCfg( castor::move( blockContext->clouds ) );
				blockContext->background->setWeatherCfg( castor::move( blockContext->weather ) );
				blockContext->background->loadTransmittance( blockContext->transmittanceDim );
				blockContext->background->loadMultiScatter( blockContext->multiScatterDim );
				blockContext->background->loadAtmosphereVolume( blockContext->atmosphereVolumeDim );
				blockContext->background->loadSkyView( blockContext->skyViewDim );
				blockContext->background->loadWorley( blockContext->worleyDim );
				blockContext->background->loadPerlinWorley( blockContext->perlinWorleyDim );
				blockContext->background->loadCurl( blockContext->curlDim );
				blockContext->background->loadWeather( blockContext->weatherDim );
				blockContext->scene->setBackground( castor::ptrRefCast< castor3d::SceneBackground >( blockContext->background ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSunNode, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::String name;

				if ( auto node = blockContext->scene->findSceneNode( params[0]->get( name ) ) )
				{
					blockContext->background->setSunNode( *node );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPlanetNode, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				castor::String name;

				if ( auto node = blockContext->scene->findSceneNode( params[0]->get( name ) ) )
				{
					blockContext->background->setPlanetNode( *node );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTransmittanceResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->transmittanceDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMultiScatterResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->multiScatterDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAtmosphereVolumeResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphereVolumeDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyViewResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->skyViewDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunIlluminance, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.sunIlluminance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunIlluminanceScale, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.sunIlluminanceScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayMarchMinSPP, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->atmosphere.rayMarchMinMaxSPP[0] = float( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayMarchMaxSPP, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->atmosphere.rayMarchMinMaxSPP[1] = float( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMultipleScatteringFactor, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.multipleScatteringFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSolarIrradiance, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.solarIrradiance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSunAngularRadius, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.sunAngularRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAbsorptionExtinction, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.absorptionExtinction );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaxSunZenithAngle, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->atmosphere.muSMin = float( castor::Angle::fromDegrees( params[0]->get< float >() ).cos() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRayleighScattering, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.rayleighScattering );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMieScattering, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.mieScattering );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMiePhaseFunctionG, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.miePhaseFunctionG );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMieExtinction, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.mieExtinction );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBottomRadius, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.bottomRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTopRadius, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.topRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGroundAlbedo, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->atmosphere.groundAlbedo );
			}
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
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->densityLayer->layerWidth );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityExpTerm, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->densityLayer->expTerm );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityExpScale, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->densityLayer->expScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityLinearTerm, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->densityLayer->linearTerm );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityConstantTerm, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->densityLayer->constantTerm );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDensityEnd, AtmosphereContext )
		{
			blockContext->densityLayer = nullptr;
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserWeather, AtmosphereContext )
		{
		}
		CU_EndAttributePushBlock( AtmosphereSection::eWeather, blockContext )

		static CU_ImplementAttributeParserBlock( parserWeatherAmplitude, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->weather.perlinAmplitude );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherFrequency, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->weather.perlinFrequency );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherScale, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->weather.perlinScale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherOctaves, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->weather.perlinOctaves );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWorleyResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->worleyDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserPerlinWorleyResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->perlinWorleyDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCurlResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->curlDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWeatherResolution, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->weatherDim );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserClouds, AtmosphereContext )
		{
		}
		CU_EndAttributePushBlock( AtmosphereSection::eClouds, blockContext )

		static CU_ImplementAttributeParserBlock( parserCloudsWindDirection, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.windDirection );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsSpeed, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.speed );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCoverage, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.coverage );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCrispiness, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.crispiness );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsCurliness, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.curliness );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsDensity, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.density );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsAbsorption, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.absorption );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsInnerRadius, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.innerRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsOuterRadius, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.outerRadius );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsTopColour, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.colorTop );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsBottomColour, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.colorBottom );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsEnablePowder, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->clouds.enablePowder = params[0]->get< bool >() ? 1 : 0;
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCloudsTopOffset, AtmosphereContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->clouds.topOffset );
			}
		}
		CU_EndAttribute()
	}

	castor::AttributeParsers createParsers()
	{
		castor::AttributeParsers result;

		addParserT( result
			, castor3d::CSCNSection::eScene
			, parse::AtmosphereSection::eRoot
			, cuT( "atmospheric_scattering" )
			, &parse::parserAtmosphereScattering );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, castor3d::CSCNSection::eScene
			, cuT( "}" )
			, &parse::parserAtmosphereScatteringEnd );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunNode" )
			, &parse::parserSunNode
			, { castor::makeParameter< castor::ParameterType::eName >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "planetNode" )
			, &parse::parserPlanetNode
			, { castor::makeParameter< castor::ParameterType::eName >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "transmittanceResolution" )
			, &parse::parserTransmittanceResolution
			, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "multiScatterResolution" )
			, &parse::parserMultiScatterResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "atmosphereVolumeResolution" )
			, &parse::parserAtmosphereVolumeResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "skyViewResolution" )
			, &parse::parserSkyViewResolution
			, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunIlluminance" )
			, &parse::parserSunIlluminance
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunIlluminanceScale" )
			, &parse::parserSunIlluminanceScale
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayMarchMinSPP" )
			, &parse::parserRayMarchMinSPP
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayMarchMaxSPP" )
			, &parse::parserRayMarchMaxSPP
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "multipleScatteringFactor" )
			, &parse::parserMultipleScatteringFactor
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "solarIrradiance" )
			, &parse::parserSolarIrradiance
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "sunAngularRadius" )
			, &parse::parserSunAngularRadius
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "absorptionExtinction" )
			, &parse::parserAbsorptionExtinction
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "maxSunZenithAngle" )
			, &parse::parserMaxSunZenithAngle
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "rayleighScattering" )
			, &parse::parserRayleighScattering
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "mieScattering" )
			, &parse::parserMieScattering
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "miePhaseFunctionG" )
			, &parse::parserMiePhaseFunctionG
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "mieExtinction" )
			, &parse::parserMieExtinction
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "bottomRadius" )
			, &parse::parserBottomRadius
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "topRadius" )
			, &parse::parserTopRadius
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eRoot
			, cuT( "groundAlbedo" )
			, &parse::parserGroundAlbedo
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
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
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "expTerm" )
			, &parse::parserDensityExpTerm
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "expScale" )
			, &parse::parserDensityExpScale
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "linearTerm" )
			, &parse::parserDensityLinearTerm
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, cuT( "constantTerm" )
			, &parse::parserDensityConstantTerm
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eDensity
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &parse::parserDensityEnd );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "worleyResolution" )
			, &parse::parserWorleyResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "perlinWorleyResolution" )
			, &parse::parserPerlinWorleyResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "weatherResolution" )
			, &parse::parserWeatherResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "curlResolution" )
			, &parse::parserCurlResolution
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "amplitude" )
			, &parse::parserWeatherAmplitude
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "frequency" )
			, &parse::parserWeatherFrequency
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "scale" )
			, &parse::parserWeatherScale
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, cuT( "octaves" )
			, &parse::parserWeatherOctaves
			, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParserT( result
			, parse::AtmosphereSection::eWeather
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &castor3d::parserDefaultEnd );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "windDirection" )
			, &parse::parserCloudsWindDirection
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "speed" )
			, &parse::parserCloudsSpeed
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "coverage" )
			, &parse::parserCloudsCoverage
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "crispiness" )
			, &parse::parserCloudsCrispiness
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "curliness" )
			, &parse::parserCloudsCurliness
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "density" )
			, &parse::parserCloudsDensity
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "absorption" )
			, &parse::parserCloudsAbsorption
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "innerRadius" )
			, &parse::parserCloudsInnerRadius
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "outerRadius" )
			, &parse::parserCloudsOuterRadius
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "topColour" )
			, &parse::parserCloudsTopColour
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "bottomColour" )
			, &parse::parserCloudsBottomColour
			, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "enablePowder" )
			, &parse::parserCloudsEnablePowder
			, { castor::makeParameter< castor::ParameterType::eBool >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, cuT( "topOffset" )
			, &parse::parserCloudsTopOffset
			, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParserT( result
			, parse::AtmosphereSection::eClouds
			, parse::AtmosphereSection::eRoot
			, cuT( "}" )
			, &castor3d::parserDefaultEnd );

		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( parse::AtmosphereSection::eRoot ), AtmosphereBackgroundModel::PluginType },
			{ uint32_t( parse::AtmosphereSection::eDensity ), cuT( "density" ) },
			{ uint32_t( parse::AtmosphereSection::eWeather ), cuT( "weather" ) },
			{ uint32_t( parse::AtmosphereSection::eClouds ), cuT( "clouds" ) },
		};
	}
}
