#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkBlendFactor convert( renderer::BlendFactor const & value )
	{
		switch ( value )
		{
		case renderer::BlendFactor::eZero:
			return VK_BLEND_FACTOR_ZERO;

		case renderer::BlendFactor::eOne:
			return VK_BLEND_FACTOR_ONE;

		case renderer::BlendFactor::eSrcColour:
			return VK_BLEND_FACTOR_SRC_COLOR;

		case renderer::BlendFactor::eInvSrcColour:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;

		case renderer::BlendFactor::eDstColour:
			return VK_BLEND_FACTOR_DST_COLOR;

		case renderer::BlendFactor::eInvDstColour:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

		case renderer::BlendFactor::eSrcAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;

		case renderer::BlendFactor::eInvSrcAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

		case renderer::BlendFactor::eDstAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;

		case renderer::BlendFactor::eInvDstAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

		case renderer::BlendFactor::eConstantColour:
			return VK_BLEND_FACTOR_CONSTANT_COLOR;

		case renderer::BlendFactor::eInvConstantColour:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;

		case renderer::BlendFactor::eConstantAlpha:
			return VK_BLEND_FACTOR_CONSTANT_ALPHA;

		case renderer::BlendFactor::eInvConstantAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;

		case renderer::BlendFactor::eSrc1Colour:
			return VK_BLEND_FACTOR_SRC1_COLOR;

		case renderer::BlendFactor::eInvSrc1Colour:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;

		case renderer::BlendFactor::eSrc1Alpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;

		case renderer::BlendFactor::eInvSrc1Alpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;

		default:
			assert( false && "Unsupported blend factor" );
			return VK_BLEND_FACTOR_ONE;
		}
	}
}
