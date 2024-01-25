#include "DrawEdgesPostEffect/DrawEdges_Parsers.hpp"

#include "DrawEdgesPostEffect/DrawEdgesPostEffect.hpp"
#include "DrawEdgesPostEffect/DrawEdgesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/TargetCache.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>

namespace draw_edges
{
	namespace parse
	{
		struct EdgesContext
		{
			castor3d::RenderTargetRPtr renderTarget{};
			DrawEdgesUboConfiguration data{};
		};

		enum class Section
			: uint32_t
		{
			eRoot = CU_MakeSectionName( 'D', 'R', 'E', 'D' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserDrawEdges, castor3d::TargetContext, EdgesContext )
		{
			newBlockContext->renderTarget = blockContext->renderTarget;
		}
		CU_EndAttributePushNewBlock( Section::eRoot )

		static CU_ImplementAttributeParserBlock( parserNormalDepthWidth, EdgesContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->data.normalDepthWidth = params[0]->get< int >();
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectWidth, EdgesContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->data.objectWidth = params[0]->get< int >();
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDrawEdgesEnd, EdgesContext )
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

	castor::AttributeParsers createParsers()
	{
		using namespace draw_edges;
		castor::AttributeParsers result;

		addParserT( result
			, castor3d::CSCNSection::eRenderTarget
			, parse::Section::eRoot
			, PostEffect::Type
			, &parse::parserDrawEdges );
		addParserT( result
			, parse::Section::eRoot
			, PostEffect::NormalDepthWidth
			, &parse::parserNormalDepthWidth
			, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 1000 ) ) } );
		addParserT( result
			, parse::Section::eRoot
			, PostEffect::ObjectWidth
			, &parse::parserObjectWidth
			, { castor::makeParameter< castor::ParameterType::eInt32 >( castor::makeRange( 0, 1000 ) ) } );
		addParserT( result
			, parse::Section::eRoot
			, castor3d::CSCNSection::eRenderTarget
			, cuT( "}" )
			, &parse::parserDrawEdgesEnd );
		return result;
	}

	castor::StrUInt32Map createSections()
	{
		return
		{
			{ uint32_t( draw_edges::parse::Section::eRoot ), draw_edges::PostEffect::Type },
		};
	}
}
