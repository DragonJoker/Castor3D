#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlCompareOp value )
	{
		switch ( value )
		{
		case gl_renderer::GL_COMPARE_OP_NEVER:
			return "GL_NEVER";

		case gl_renderer::GL_COMPARE_OP_LESS:
			return "GL_LESS";

		case gl_renderer::GL_COMPARE_OP_EQUAL:
			return "GL_EQUAL";

		case gl_renderer::GL_COMPARE_OP_LEQUAL:
			return "GL_LEQUAL";

		case gl_renderer::GL_COMPARE_OP_GREATER:
			return "GL_GREATER";

		case gl_renderer::GL_COMPARE_OP_NOTEQUAL:
			return "GL_NOTEQUAL";

		case gl_renderer::GL_COMPARE_OP_GEQUAL:
			return "GL_GEQUAL";

		case gl_renderer::GL_COMPARE_OP_ALWAYS:
			return "GL_ALWAYS";

		default:
			assert( false && "Unsupported GlCompareOp" );
			return "GlCompareOp_UNKNOWN";
		}
	}

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
