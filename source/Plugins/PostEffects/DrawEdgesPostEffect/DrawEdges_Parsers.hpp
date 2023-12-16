/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_Parsers_H___
#define ___C3DDE_Parsers_H___

#include "DrawEdgesUbo.hpp"

#include <Castor3D/Scene/SceneFileParser.hpp>

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/FileParser/FileParserContext.hpp>

namespace draw_edges
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

	CU_DeclareAttributeParserBlock( parserDrawEdges, castor3d::TargetContext )
	CU_DeclareAttributeParserBlock( parserNormalDepthWidth, EdgesContext )
	CU_DeclareAttributeParserBlock( parserObjectWidth, EdgesContext )
	CU_DeclareAttributeParserBlock( parserDrawEdgesEnd, EdgesContext )
}

#endif
