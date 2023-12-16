#include "AtmosphereScattering/AtmosphereScattering_Parsers.hpp"

#include "AtmosphereScattering/AtmosphereBackground.hpp"
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
	CU_ImplementAttributeParserNewBlock( parserAtmosphereScattering, castor3d::SceneContext, AtmosphereContext )
	{
		newBlockContext->scene = blockContext->scene;
		newBlockContext->background = castor::makeUnique< AtmosphereBackground >( *blockContext->scene->getEngine()
			, *blockContext->scene );
	}
	CU_EndAttributePushNewBlock( AtmosphereSection::eRoot )

	CU_ImplementAttributeParserBlock( parserAtmosphereScatteringEnd, AtmosphereContext )
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
			blockContext->background->setAtmosphereCfg( std::move( blockContext->atmosphere ) );
			blockContext->background->setCloudsCfg( std::move( blockContext->clouds ) );
			blockContext->background->setWeatherCfg( std::move( blockContext->weather ) );
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

		CU_ImplementAttributeParserBlock( parserSunNode, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
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

		CU_ImplementAttributeParserBlock( parserPlanetNode, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
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

	CU_ImplementAttributeParserBlock( parserTransmittanceResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->transmittanceDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMultiScatterResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->multiScatterDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserAtmosphereVolumeResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphereVolumeDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSkyViewResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->skyViewDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSunIlluminance, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.sunIlluminance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSunIlluminanceScale, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.sunIlluminanceScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserRayMarchMinSPP, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->atmosphere.rayMarchMinMaxSPP[0] = float( params[0]->get< uint32_t >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserRayMarchMaxSPP, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->atmosphere.rayMarchMinMaxSPP[1] = float( params[0]->get< uint32_t >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMultipleScatteringFactor, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.multipleScatteringFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSolarIrradiance, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.solarIrradiance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSunAngularRadius, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.sunAngularRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserAbsorptionExtinction, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.absorptionExtinction );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMaxSunZenithAngle, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->atmosphere.muSMin = float( castor::Angle::fromDegrees( params[0]->get< float >() ).cos() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserRayleighScattering, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.rayleighScattering );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMieScattering, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.mieScattering );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMiePhaseFunctionG, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.miePhaseFunctionG );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMieExtinction, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.mieExtinction );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserBottomRadius, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.bottomRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserTopRadius, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.topRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserGroundAlbedo, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->atmosphere.groundAlbedo );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMinRayleighDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.rayleighDensity[0];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserMaxRayleighDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.rayleighDensity[1];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserMinMieDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.mieDensity[0];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserMaxMieDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.mieDensity[1];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserMinAbsorptionDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.absorptionDensity[0];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserMaxAbsorptionDensity, AtmosphereContext )
	{
		blockContext->densityLayer = &blockContext->atmosphere.absorptionDensity[1];
	}
	CU_EndAttributePushBlock( AtmosphereSection::eDensity, blockContext )

	CU_ImplementAttributeParserBlock( parserDensityLayerWidth, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->densityLayer->layerWidth );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDensityExpTerm, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->densityLayer->expTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDensityExpScale, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->densityLayer->expScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDensityLinearTerm, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->densityLayer->linearTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDensityConstantTerm, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->densityLayer->constantTerm );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDensityEnd, AtmosphereContext )
	{
		blockContext->densityLayer = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParserBlock( parserWeather, AtmosphereContext )
	{
	}
	CU_EndAttributePushBlock( AtmosphereSection::eWeather, blockContext )

	CU_ImplementAttributeParserBlock( parserWeatherAmplitude, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->weather.perlinAmplitude );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserWeatherFrequency, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->weather.perlinFrequency );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserWeatherScale, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->weather.perlinScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserWeatherOctaves, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->weather.perlinOctaves );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserWorleyResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->worleyDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserPerlinWorleyResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->perlinWorleyDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCurlResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->curlDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserWeatherResolution, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->weatherDim );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserClouds, AtmosphereContext )
	{
	}
	CU_EndAttributePushBlock( AtmosphereSection::eClouds, blockContext )

	CU_ImplementAttributeParserBlock( parserCloudsWindDirection, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.windDirection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsSpeed, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.speed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsCoverage, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.coverage );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsCrispiness, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.crispiness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsCurliness, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.curliness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsDensity, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.density );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsAbsorption, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.absorption );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsInnerRadius, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.innerRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsOuterRadius, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.outerRadius );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsTopColour, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.colorTop );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsBottomColour, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.colorBottom );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsEnablePowder, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->clouds.enablePowder = params[0]->get< bool >() ? 1 : 0;;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserCloudsTopOffset, AtmosphereContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->clouds.topOffset );
		}
	}
	CU_EndAttribute()
}
