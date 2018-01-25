#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkLogicOp convert( renderer::LogicOp const & value )
	{
		switch ( value )
		{
		case renderer::LogicOp::eClear:
			return VK_LOGIC_OP_CLEAR;

		case renderer::LogicOp::eAnd:
			return VK_LOGIC_OP_AND;

		case renderer::LogicOp::eAndReverse:
			return VK_LOGIC_OP_AND_REVERSE;

		case renderer::LogicOp::eCopy:
			return VK_LOGIC_OP_COPY;

		case renderer::LogicOp::eAndInverted:
			return VK_LOGIC_OP_AND_INVERTED;

		case renderer::LogicOp::eNoOp:
			return VK_LOGIC_OP_NO_OP;

		case renderer::LogicOp::eXor:
			return VK_LOGIC_OP_XOR;

		case renderer::LogicOp::eOr:
			return VK_LOGIC_OP_OR;

		case renderer::LogicOp::eNor:
			return VK_LOGIC_OP_NOR;

		case renderer::LogicOp::eEquivalent:
			return VK_LOGIC_OP_EQUIVALENT;

		case renderer::LogicOp::eInvert:
			return VK_LOGIC_OP_INVERT;

		case renderer::LogicOp::eOrReverse:
			return VK_LOGIC_OP_OR_REVERSE;

		case renderer::LogicOp::eCopyInverted:
			return VK_LOGIC_OP_COPY_INVERTED;

		case renderer::LogicOp::eOrInverted:
			return VK_LOGIC_OP_OR_INVERTED;

		case renderer::LogicOp::eNand:
			return VK_LOGIC_OP_NAND;

		case renderer::LogicOp::eSet:
			return VK_LOGIC_OP_SET;

		default:
			assert( false && "Unsupported logic operator");
			return VK_LOGIC_OP_COPY;
		}
	}
}
