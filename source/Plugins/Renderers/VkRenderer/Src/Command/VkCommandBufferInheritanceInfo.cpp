#include "VkRendererPrerequisites.hpp"

#include "RenderPass/VkFrameBuffer.hpp"
#include "RenderPass/VkRenderPass.hpp"

namespace vk_renderer
{
	VkCommandBufferInheritanceInfo convert( renderer::CommandBufferInheritanceInfo const & value )
	{
		return VkCommandBufferInheritanceInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			nullptr,
			value.renderPass ? VkRenderPass( static_cast< RenderPass const & >( *value.renderPass ) ) : VK_NULL_HANDLE,
			value.subpass,
			value.framebuffer ? VkFramebuffer( static_cast< FrameBuffer const & >( *value.framebuffer ) ) : VK_NULL_HANDLE,
			value.occlusionQueryEnable,
			convert( value.queryFlags ),
			convert( value.pipelineStatistics )
		};
	}
}
