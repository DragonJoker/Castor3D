#include "OceanRendering/Ocean_Parsers.hpp"

#include "OceanRendering/OceanRenderPass.hpp"
#include "OceanRendering/OceanUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace ocean
{
	namespace
	{
		ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( OceanRenderPass::Type ) );
		}

		void parseFilePath( castor::FileParserContext & context
			, castor::Path relative
			, castor::String const & prefix )
		{
			castor::Path folder;
			auto & waveContext = getParserContext( context );

			if ( castor::File::fileExists( context.file.getPath() / relative ) )
			{
				folder = context.file.getPath();
			}
			else if ( !castor::File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				waveContext.parameters.add( prefix + "Folder", folder );
				waveContext.parameters.add( prefix + "Relative", relative );
			}
		}
	}

	CU_ImplementAttributeParser( parserOceanRendering )
	{
	}
	CU_EndAttributePush( OceanSection::eRoot )

	CU_ImplementAttributeParser( parserTessellationFactor )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			uint32_t value{};
			params[0]->get( value );
			waveContext.config.tessellationFactor = float( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDampeningFactor )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.dampeningFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistortionFactor )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.refractionDistortionFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrHeightFactor )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.refractionHeightFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistanceFactor )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.refractionDistanceFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDepthSofteningDistance )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.depthSofteningDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFoamHeightStart )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.foamHeightStart );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFoamFadeDistance )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.foamFadeDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFoamTiling )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.foamTiling );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserFoamAngleExponent )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.foamAngleExponent );
		}
	}
	CU_EndAttribute()
		
	CU_ImplementAttributeParser( parserFoamBrightness )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.foamBrightness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormalMapScrollSpeed )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.normalMapScrollSpeed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormalMapScroll )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.normalMapScroll );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsrSettings )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.ssrSettings );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFoam )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Path relative;
			params[0]->get( relative );
			parseFilePath( context
				, std::move( relative )
				, OceanRenderPass::Foam );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormals1 )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Path relative;
			params[0]->get( relative );
			parseFilePath( context
				, std::move( relative )
				, OceanRenderPass::Normals1 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormals2 )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Path relative;
			params[0]->get( relative );
			parseFilePath( context
				, std::move( relative )
				, OceanRenderPass::Normals2 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNoise )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Path relative;
			params[0]->get( relative );
			parseFilePath( context
				, std::move( relative )
				, OceanRenderPass::Noise );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveRenderingEnd )
	{
		auto & waveContext = getParserContext( context );
		waveContext.config.numWaves = float( waveContext.wave );
		waveContext.parameters.add( OceanRenderPass::Param, waveContext.config );
		waveContext.engine->setRenderPassTypeConfiguration( OceanRenderPass::Type
			, std::move( waveContext.parameters ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserWaves )
	{
	}
	CU_EndAttributePush( OceanSection::eWaves )

	CU_ImplementAttributeParser( parserWavesEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserWave )
	{
	}
	CU_EndAttributePush( OceanSection::eWave )

	CU_ImplementAttributeParser( parserWaveDirection )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			castor::Point3f dir;
			params[0]->get( dir );
			waveContext.config.waves[waveContext.wave].direction = castor::Point4f{ dir };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveSteepness )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.waves[waveContext.wave].steepness );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveLength )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.waves[waveContext.wave].length );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveAmplitude )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.waves[waveContext.wave].amplitude );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveSpeed )
	{
		auto & waveContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waveContext.config.waves[waveContext.wave].speed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaveEnd )
	{
		auto & waveContext = getParserContext( context );
		++waveContext.wave;
	}
	CU_EndAttributePop()
}
