#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSubpassDescription convert( renderer::SubpassDescription const & value
		, std::vector< VkAttachmentReference > const & inputAttachments
		, std::vector< VkAttachmentReference > const & colorAttachments
		, std::vector< VkAttachmentReference > const & resolveAttachments
		, VkAttachmentReference const & depthStencilAttachment )
	{
		return VkSubpassDescription
		{
			convert( value.flags ),
			convert( value.pipelineBindPoint ),
			uint32_t( inputAttachments.size() ),
			inputAttachments.data(),
			uint32_t( colorAttachments.size() ),
			colorAttachments.data(),
			resolveAttachments.data(),
			depthStencilAttachment.attachment != VK_ATTACHMENT_UNUSED
				? &depthStencilAttachment
				: nullptr,
			uint32_t( value.reserveAttachments.size() ),
			value.reserveAttachments.data(),
		};
	}
}
