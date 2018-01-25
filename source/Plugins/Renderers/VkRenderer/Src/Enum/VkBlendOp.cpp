#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBlendOp convert( renderer::BlendOp const & value )
	{
		switch ( value )
		{
		case renderer::BlendOp::eAdd:
			return VK_BLEND_OP_ADD;

		case renderer::BlendOp::eSubtract:
			return VK_BLEND_OP_SUBTRACT;

		case renderer::BlendOp::eReverseSubtract:
			return VK_BLEND_OP_REVERSE_SUBTRACT;

		case renderer::BlendOp::eMin:
			return VK_BLEND_OP_MIN;

		case renderer::BlendOp::eMax:
			return VK_BLEND_OP_MAX;

		default:
			assert( false && "Unsupported blend operator" );
			return VK_BLEND_OP_ADD;
		}
	}
}
