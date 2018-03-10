#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkAttachmentLoadOp convert( renderer::AttachmentLoadOp const & value )
	{
		switch ( value )
		{
		case renderer::AttachmentLoadOp::eLoad:
			return VK_ATTACHMENT_LOAD_OP_LOAD;

		case renderer::AttachmentLoadOp::eClear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;

		case renderer::AttachmentLoadOp::eDontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;

		default:
			assert( false && "Unsupported AttachmentLoadOp" );
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}
	}
}
