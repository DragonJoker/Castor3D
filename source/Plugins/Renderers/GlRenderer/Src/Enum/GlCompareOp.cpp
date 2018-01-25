#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlCompareOp convert( renderer::CompareOp const & value )
	{
		switch ( value )
		{
		case renderer::CompareOp::eNever:
			return GL_COMPARE_OP_NEVER;

		case renderer::CompareOp::eLess:
			return GL_COMPARE_OP_LESS;

		case renderer::CompareOp::eEqual:
			return GL_COMPARE_OP_EQUAL;

		case renderer::CompareOp::eLessEqual:
			return GL_COMPARE_OP_LEQUAL;

		case renderer::CompareOp::eGreater:
			return GL_COMPARE_OP_GREATER;

		case renderer::CompareOp::eNotEqual:
			return GL_COMPARE_OP_NOTEQUAL;

		case renderer::CompareOp::eGreaterEqual:
			return GL_COMPARE_OP_GEQUAL;

		case renderer::CompareOp::eAlways:
			return GL_COMPARE_OP_ALWAYS;

		default:
			assert( false && "Unsupported comparison operator" );
			return GL_COMPARE_OP_ALWAYS;
		}
	}
}
