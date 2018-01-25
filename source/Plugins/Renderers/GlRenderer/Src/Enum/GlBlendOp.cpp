#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
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
			assert( false && "Unsupported blend operator" );
			return GL_BLEND_OP_ADD;
		}
	}
}
