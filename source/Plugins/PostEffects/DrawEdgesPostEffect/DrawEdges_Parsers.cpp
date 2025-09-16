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
			c3d::RenderTargetRPtr renderTarget{};
			DrawEdgesUboConfiguration data{};
		};

		enum class Section
			: c3d::SectionId
		{
			eRoot = c3d::makeSectionName( 'D', 'R', 'E', 'D' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserDrawEdges, c3d::TargetContext, EdgesContext )
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
			c3d::Parameters parameters;
			parameters.add( PostEffect::NormalDepthWidth, c3d::string::toString( blockContext->data.normalDepthWidth ) );
			parameters.add( PostEffect::ObjectWidth, c3d::string::toString( blockContext->data.objectWidth ) );

			auto effect = blockContext->renderTarget->getPostEffect( PostEffect::Type );
			effect->enable( true );
			effect->setParameters( parameters );
		}
		CU_EndAttributePop()
	}

	c3d::AttributeParsers createParsers()
	{
		using namespace draw_edges;
		c3d::AttributeParsers result;

		addParserT( result
			, c3d::CSCNSection::eRenderTarget
			, parse::Section::eRoot
			, PostEffect::Type
			, &parse::parserDrawEdges );
		addParserT( result
			, parse::Section::eRoot
			, PostEffect::NormalDepthWidth
			, &parse::parserNormalDepthWidth
			, { c3d::makeParameter< c3d::ParameterType::eInt32 >( c3d::makeRange( 0, 1000 ) ) } );
		addParserT( result
			, parse::Section::eRoot
			, PostEffect::ObjectWidth
			, &parse::parserObjectWidth
			, { c3d::makeParameter< c3d::ParameterType::eInt32 >( c3d::makeRange( 0, 1000 ) ) } );
		addParserT( result
			, parse::Section::eRoot
			, c3d::CSCNSection::eRenderTarget
			, cuT( "}" )
			, &parse::parserDrawEdgesEnd );
		return result;
	}

	c3d::StrSectionIdMap createSections()
	{
		return
		{
			{ c3d::SectionId( draw_edges::parse::Section::eRoot ), draw_edges::PostEffect::Type },
		};
	}
}
