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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.refractionRatio );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.refractionDistortionFactor );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.refractionHeightFactor );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.refractionDistanceFactor );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.depthSofteningDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrSettings )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.ssrSettings );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.config.density );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftConfig )
	{
	}
	CU_EndAttributePush( OceanSection::eFFT )

	CU_ImplementAttributeParser( parserOceanRenderingEnd )
	{
		auto & waveContext = getParserContext( context );
		waveContext.parameters.add( OceanRenderPass::Param, waveContext.config );
		waveContext.engine->setRenderPassTypeConfiguration( OceanRenderPass::Type
			, std::move( waveContext.parameters ) );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.disableRandomSeed );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.size );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.heightMapSamples );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.displacementDownsample );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.normalFreqMod );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftMaxWaveLength )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.maxWaveLength );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.amplitude );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.windDirection );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.windVelocity );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.patchSize );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.blocksCount );
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
			auto & waveContext = getParserContext( context );
			params[0]->get( waveContext.fftConfig.lod0Distance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFftConfigEnd )
	{
		auto & waveContext = getParserContext( context );
		waveContext.parameters.add( OceanRenderPass::ParamFFT, waveContext.fftConfig );
	}
	CU_EndAttributePop()
}
