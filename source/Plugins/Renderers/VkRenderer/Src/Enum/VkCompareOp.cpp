#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkCompareOp convert( renderer::CompareOp const & value )
	{
		switch ( value )
		{
		case renderer::CompareOp::eNever:
			return VK_COMPARE_OP_NEVER;

		case renderer::CompareOp::eLess:
			return VK_COMPARE_OP_LESS;

		case renderer::CompareOp::eEqual:
			return VK_COMPARE_OP_EQUAL;

		case renderer::CompareOp::eLessEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;

		case renderer::CompareOp::eGreater:
			return VK_COMPARE_OP_GREATER;

		case renderer::CompareOp::eNotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;

		case renderer::CompareOp::eGreaterEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;

		case renderer::CompareOp::eAlways:
			return VK_COMPARE_OP_ALWAYS;

		default:
			assert( false && "Unsupported comparison operator" );
			return VK_COMPARE_OP_ALWAYS;
		}
	}
}
