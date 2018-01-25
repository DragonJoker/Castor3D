#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkStencilOp convert( renderer::StencilOp const & value )
	{
		switch ( value )
		{
		case renderer::StencilOp::eKeep:
			return VK_STENCIL_OP_KEEP;

		case renderer::StencilOp::eZero:
			return VK_STENCIL_OP_ZERO;

		case renderer::StencilOp::eReplace:
			return VK_STENCIL_OP_REPLACE;

		case renderer::StencilOp::eIncrementAndClamp:
			return VK_STENCIL_OP_INCREMENT_AND_CLAMP;

		case renderer::StencilOp::eDecrementAndClamp:
			return VK_STENCIL_OP_DECREMENT_AND_CLAMP;

		case renderer::StencilOp::eInvert:
			return VK_STENCIL_OP_INVERT;

		case renderer::StencilOp::eIncrementAndWrap:
			return VK_STENCIL_OP_INCREMENT_AND_WRAP;

		case renderer::StencilOp::eDecrementAndWrap:
			return VK_STENCIL_OP_DECREMENT_AND_WRAP;

		default:
			assert( false && "Unsupported stencil operator");
			return VK_STENCIL_OP_KEEP;
		}
	}
}
