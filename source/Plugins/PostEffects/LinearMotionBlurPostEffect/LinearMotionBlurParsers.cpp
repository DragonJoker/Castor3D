#include "LinearMotionBlurPostEffect/LinearMotionBlurParsers.hpp"

#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Event/Frame/FrameListener.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Overlay/BorderPanelOverlay.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/PostEffect/PostEffectFactory.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

#include <stack>

namespace motion_blur
{
	CU_ImplementAttributeParserNewBlock( parserMotionBlur, castor3d::TargetContext, BlurContext )
	{
		newBlockContext->renderTarget = blockContext->renderTarget;
	}
	CU_EndAttributePushNewBlock( MotionBlurSection::eRoot )

	CU_ImplementAttributeParserBlock( parserDivider, BlurContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.vectorDivider );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserSamples, BlurContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->data.samplesCount );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserFpsScale, BlurContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			params[0]->get( blockContext->fpsScale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserMotionBlurEnd, BlurContext )
	{
		castor3d::Parameters parameters;
		parameters.add( cuT( "vectorDivider" ), blockContext->data.vectorDivider );
		parameters.add( cuT( "samplesCount" ), blockContext->data.samplesCount );
		parameters.add( cuT( "fpsScale" ), blockContext->fpsScale );

		auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );
	}
	CU_EndAttributePop()
}
