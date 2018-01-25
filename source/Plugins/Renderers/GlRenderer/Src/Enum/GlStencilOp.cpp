#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlStencilOp convert( renderer::StencilOp const & value )
	{
		switch ( value )
		{
		case renderer::StencilOp::eKeep:
			return GL_STENCIL_OP_KEEP;

		case renderer::StencilOp::eZero:
			return GL_STENCIL_OP_ZERO;

		case renderer::StencilOp::eReplace:
			return GL_STENCIL_OP_REPLACE;

		case renderer::StencilOp::eIncrementAndClamp:
			return GL_STENCIL_OP_INCR;

		case renderer::StencilOp::eDecrementAndClamp:
			return GL_STENCIL_OP_DECR;

		case renderer::StencilOp::eInvert:
			return GL_STENCIL_OP_INVERT;

		case renderer::StencilOp::eIncrementAndWrap:
			return GL_STENCIL_OP_INCR_WRAP;

		case renderer::StencilOp::eDecrementAndWrap:
			return GL_STENCIL_OP_DECR_WRAP;

		default:
			assert( false && "Unsupported stencil operator");
			return GL_STENCIL_OP_KEEP;
		}
	}
}
