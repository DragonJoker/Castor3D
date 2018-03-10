#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkAttachmentStoreOp convert( renderer::AttachmentStoreOp const & value )
	{
		switch ( value )
		{
		case renderer::AttachmentStoreOp::eStore:
			return VK_ATTACHMENT_STORE_OP_STORE;

		case renderer::AttachmentStoreOp::eDontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;

		default:
			assert( false && "Unsupported AttachmentStoreOp" );
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}
	}
}
