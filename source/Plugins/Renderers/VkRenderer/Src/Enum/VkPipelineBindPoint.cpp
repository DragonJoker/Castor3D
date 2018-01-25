#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkPipelineBindPoint convert( renderer::PipelineBindPoint const & point )
	{
		switch ( point )
		{
		case renderer::PipelineBindPoint::eGraphics:
			return VK_PIPELINE_BIND_POINT_GRAPHICS;

		case renderer::PipelineBindPoint::eCompute:
			return VK_PIPELINE_BIND_POINT_COMPUTE;

		default:
			assert( false && "Unsupported bind point" );
			return VkPipelineBindPoint( 0 );
		}
	}
}
