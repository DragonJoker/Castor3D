#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlBlendOp value )
	{
		switch ( value )
		{
		case gl_renderer::GL_BLEND_OP_ADD:
			return "GL_ADD";

		case gl_renderer::GL_BLEND_OP_MIN:
			return "GL_MIN";

		case gl_renderer::GL_BLEND_OP_MAX:
			return "GL_MAX";

		case gl_renderer::GL_BLEND_OP_SUBTRACT:
			return "GL_SUBTRACT";

		case gl_renderer::GL_BLEND_OP_REVERSE_SUBTRACT:
			return "GL_REVERSE_SUBTRACT";

		default:
			assert( false && "Unsupported GlBlendOp" );
			return "GlBlendOp_UNKNOWN";
		}
	}

	GlBlendOp convert( renderer::BlendOp const & value )
	{
		switch ( value )
		{
		case renderer::BlendOp::eAdd:
			return GL_BLEND_OP_ADD;

		case renderer::BlendOp::eSubtract:
			return GL_BLEND_OP_SUBTRACT;

		case renderer::BlendOp::eReverseSubtract:
			return GL_BLEND_OP_REVERSE_SUBTRACT;

		case renderer::BlendOp::eMin:
			return GL_BLEND_OP_MIN;

		case renderer::BlendOp::eMax:
			return GL_BLEND_OP_MAX;

		default:
			assert( false && "Unsupported GlBlendOp" );
			return GL_BLEND_OP_ADD;
		}
	}
}
