#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlBlendFactor value )
	{
		switch ( value )
		{
		case gl_renderer::GL_BLEND_FACTOR_ZERO:
			return "GL_ZERO";

		case gl_renderer::GL_BLEND_FACTOR_ONE:
			return "GL_ONE";

		case gl_renderer::GL_BLEND_FACTOR_SRC_COLOR:
			return "GL_SRC_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR:
			return "GL_ONE_MINUS_SRC_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_SRC_ALPHA:
			return "GL_SRC_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
			return "GL_ONE_MINUS_SRC_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_DST_ALPHA:
			return "GL_DST_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA:
			return "GL_ONE_MINUS_DST_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_DST_COLOR:
			return "GL_DST_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_DST_COLOR:
			return "GL_ONE_MINUS_DST_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_SRC_ALPHA_SATURATE:
			return "GL_SRC_ALPHA_SATURATE";

		case gl_renderer::GL_BLEND_FACTOR_CONSTANT_COLOR:
			return "GL_CONSTANT_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR:
			return "GL_ONE_MINUS_CONSTANT_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_CONSTANT_ALPHA:
			return "GL_CONSTANT_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA:
			return "GL_ONE_MINUS_CONSTANT_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_SRC1_ALPHA:
			return "GL_SRC1_ALPHA";

		case gl_renderer::GL_BLEND_FACTOR_SRC1_COLOR:
			return "GL_SRC1_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR:
			return "GL_ONE_MINUS_SRC1_COLOR";

		case gl_renderer::GL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA:
			return "GL_ONE_MINUS_SRC1_ALPHA";

		default:
			assert( false && "Unsupported GlBlendFactor" );
			return "GlBlendFactor_UNKNOWN";
		}
	}

	GlBlendFactor convert( renderer::BlendFactor const & value )
	{
		switch ( value )
		{
		case renderer::BlendFactor::eZero:
			return GL_BLEND_FACTOR_ZERO;

		case renderer::BlendFactor::eOne:
			return GL_BLEND_FACTOR_ONE;

		case renderer::BlendFactor::eSrcColour:
			return GL_BLEND_FACTOR_SRC_COLOR;

		case renderer::BlendFactor::eInvSrcColour:
			return GL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

		case renderer::BlendFactor::eDstColour:
			return GL_BLEND_FACTOR_DST_COLOR;

		case renderer::BlendFactor::eInvDstColour:
			return GL_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

		case renderer::BlendFactor::eSrcAlpha:
			return GL_BLEND_FACTOR_SRC_ALPHA;

		case renderer::BlendFactor::eInvSrcAlpha:
			return GL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

		case renderer::BlendFactor::eDstAlpha:
			return GL_BLEND_FACTOR_DST_ALPHA;

		case renderer::BlendFactor::eInvDstAlpha:
			return GL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

		case renderer::BlendFactor::eConstantColour:
			return GL_BLEND_FACTOR_CONSTANT_COLOR;

		case renderer::BlendFactor::eInvConstantColour:
			return GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

		case renderer::BlendFactor::eConstantAlpha:
			return GL_BLEND_FACTOR_CONSTANT_ALPHA;

		case renderer::BlendFactor::eInvConstantAlpha:
			return GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

		case renderer::BlendFactor::eSrc1Colour:
			return GL_BLEND_FACTOR_SRC1_COLOR;

		case renderer::BlendFactor::eInvSrc1Colour:
			return GL_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

		case renderer::BlendFactor::eSrc1Alpha:
			return GL_BLEND_FACTOR_SRC1_ALPHA;

		case renderer::BlendFactor::eInvSrc1Alpha:
			return GL_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

		default:
			assert( false && "Unsupported blend factor" );
			return GL_BLEND_FACTOR_ONE;
		}
	}
}
