#include "DrawEdgesPostEffect/DrawEdges_Parsers.hpp"

#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"
#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace draw_edges
{
	CU_ImplementAttributeParserNewBlock( parserDrawEdges, castor3d::TargetContext, EdgesContext )
	{
		newBlockContext->renderTarget = blockContext->renderTarget;
	}
	CU_EndAttributePushNewBlock( Section::eRoot )

	CU_ImplementAttributeParserBlock( parserNormalDepthWidth, EdgesContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->data.normalDepthWidth = params[0]->get< int >();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserObjectWidth, EdgesContext )
	{
		if ( params.empty() )
		{
			CU_ParsingError( "Missing parameter" );
		}
		else
		{
			blockContext->data.objectWidth = params[0]->get< int >();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParserBlock( parserDrawEdgesEnd, EdgesContext )
	{
		castor3d::Parameters parameters;
		parameters.add( PostEffect::NormalDepthWidth, castor::string::toString( blockContext->data.normalDepthWidth ) );
		parameters.add( PostEffect::ObjectWidth, castor::string::toString( blockContext->data.objectWidth ) );

		auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
		effect->enable( true );
		effect->setParameters( parameters );
	}
	CU_EndAttributePop()
}
