#include "FFTOceanRendering/OceanFFT_Parsers.hpp"

#include "FFTOceanRendering/OceanFFTRenderPass.hpp"
#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace ocean_fft
{
	namespace
	{
		ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( OceanRenderPass::Type ) );
		}
	}

	CU_ImplementAttributeParser( parserOceanRendering )
	{
	}
	CU_EndAttributePush( OceanSection::eRoot )

	CU_ImplementAttributeParser( parserRefrRatio )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.refractionRatio );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistortionFactor )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.refractionDistortionFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrHeightFactor )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.refractionHeightFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistanceFactor )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.refractionDistanceFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDepthSofteningDistance )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.depthSofteningDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrStepSize )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.ssrStepSize );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrFwdStepCount )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.ssrForwardStepsCount );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrBckStepCount )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.ssrBackwardStepsCount );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrDepthMult )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.ssrDepthMult );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDensity )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.config.density );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftConfig )
	{
	}
	CU_EndAttributePush( OceanSection::eFFT )

	CU_ImplementAttributeParser( parserOceanRenderingEnd )
	{
		auto & oceanContext = getParserContext( context );
		oceanContext.parameters.add( OceanRenderPass::Param, oceanContext.config );
		oceanContext.engine->setRenderPassTypeConfiguration( OceanRenderPass::Type
			, std::move( oceanContext.parameters ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserFftDisableRandomSeed )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.disableRandomSeed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftSize )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.size );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftHeightMapSamples )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.heightMapSamples );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftDisplacementDownsample )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.displacementDownsample );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftNormalMapFreqMod )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.normalFreqMod );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftAmplitude )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.amplitude );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftWindDirection )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.windDirection );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftWindVelocity )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.windVelocity );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftPatchSize )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.patchSize );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftBlocksCount )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.blocksCount );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftLOD0Distance )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & oceanContext = getParserContext( context );
			params[0]->get( oceanContext.fftConfig.lod0Distance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftConfigEnd )
	{
		auto & oceanContext = getParserContext( context );
		oceanContext.parameters.add( OceanRenderPass::ParamFFT, oceanContext.fftConfig );
	}
	CU_EndAttributePop()
}
