#include "WaterRendering/Water_Parsers.hpp"

#include "WaterRendering/WaterRenderPass.hpp"
#include "WaterRendering/WaterUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace water
{
	namespace
	{
		ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( WaterRenderPass::Type ) );
		}

		void parseFilePath( castor::FileParserContext & context
			, castor::Path relative
			, castor::String const & prefix )
		{
			castor::Path folder;
			auto & waterContext = getParserContext( context );

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
				waterContext.parameters.add( prefix + "Folder", folder );
				waterContext.parameters.add( prefix + "Relative", relative );
			}
		}
	}

	CU_ImplementAttributeParser( parserWaterRendering )
	{
	}
	CU_EndAttributePush( WaterSection::eRoot )

	CU_ImplementAttributeParser( parserDampeningFactor )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.dampeningFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserDepthSofteningDistance )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.depthSofteningDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrRatio )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.refractionRatio );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistortionFactor )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.refractionDistortionFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrHeightFactor )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.refractionHeightFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRefrDistanceFactor )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.refractionDistanceFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormalMapScrollSpeed )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.normalMapScrollSpeed );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNormalMapScroll )
	{
		auto & waterContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( waterContext.config.normalMapScroll );
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
			auto & waterContext = getParserContext( context );
			params[0]->get( waterContext.config.ssrStepSize );
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
			auto & waterContext = getParserContext( context );
			params[0]->get( waterContext.config.ssrForwardStepsCount );
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
			auto & waterContext = getParserContext( context );
			params[0]->get( waterContext.config.ssrBackwardStepsCount );
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
			auto & waterContext = getParserContext( context );
			params[0]->get( waterContext.config.ssrDepthMult );
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
				, WaterRenderPass::Normals1 );
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
				, WaterRenderPass::Normals2 );
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
				, WaterRenderPass::Noise );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWaterRenderingEnd )
	{
		auto & waterContext = getParserContext( context );
		waterContext.parameters.add( WaterRenderPass::Param, waterContext.config );
		waterContext.engine->setRenderPassTypeConfiguration( WaterRenderPass::Type
			, std::move( waterContext.parameters ) );
	}
	CU_EndAttributePop()
}
