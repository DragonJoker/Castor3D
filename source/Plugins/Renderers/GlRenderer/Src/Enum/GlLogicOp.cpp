#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlLogicOp convert( renderer::LogicOp const & value )
	{
		switch ( value )
		{
		case renderer::LogicOp::eClear:
			return GL_LOGIC_OP_CLEAR;

		case renderer::LogicOp::eAnd:
			return GL_LOGIC_OP_AND;

		case renderer::LogicOp::eAndReverse:
			return GL_LOGIC_OP_AND_REVERSE;

		case renderer::LogicOp::eCopy:
			return GL_LOGIC_OP_COPY;

		case renderer::LogicOp::eAndInverted:
			return GL_LOGIC_OP_AND_INVERTED;

		case renderer::LogicOp::eNoOp:
			return GL_LOGIC_OP_NOOP;

		case renderer::LogicOp::eXor:
			return GL_LOGIC_OP_XOR;

		case renderer::LogicOp::eOr:
			return GL_LOGIC_OP_OR;

		case renderer::LogicOp::eNor:
			return GL_LOGIC_OP_NOR;

		case renderer::LogicOp::eEquivalent:
			return GL_LOGIC_OP_EQUIV;

		case renderer::LogicOp::eInvert:
			return GL_LOGIC_OP_INVERT;

		case renderer::LogicOp::eOrReverse:
			return GL_LOGIC_OP_OR_REVERSE;

		case renderer::LogicOp::eCopyInverted:
			return GL_LOGIC_OP_COPY_INVERTED;

		case renderer::LogicOp::eOrInverted:
			return GL_LOGIC_OP_OR_INVERTED;

		case renderer::LogicOp::eNand:
			return GL_LOGIC_OP_NAND;

		case renderer::LogicOp::eSet:
			return GL_LOGIC_OP_SET;

		default:
			assert( false && "Unsupported logic operator");
			return GL_LOGIC_OP_COPY;
		}
	}
}
