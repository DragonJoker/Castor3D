#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkClearValue convert( renderer::ClearValue const & value )
	{
		VkClearValue result;

		if ( value.isColour() )
		{
			result.color = convert( value.colour() );
		}
		else
		{
			result.depthStencil = convert( value.depthStencil() );
		}

		return result;
	}

	VkClearColorValue convert( renderer::RgbaColour const & colour )
	{
		return VkClearColorValue
		{
			{ colour[0], colour[1], colour[2], colour[3] }
		};
	}

	renderer::RgbaColour convert( VkClearColorValue const & colour )
	{
		return renderer::RgbaColour
		{
			colour.float32[0],
			colour.float32[1],
			colour.float32[2],
			colour.float32[3],
		};
	}

	VkClearDepthStencilValue convert( renderer::DepthStencilClearValue const & value )
	{
		return VkClearDepthStencilValue{ value.depth, value.stencil };
	}

	renderer::DepthStencilClearValue convert( VkClearDepthStencilValue const & value )
	{
		return renderer::DepthStencilClearValue{ value.depth, value.stencil };
	}
}
