#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkAttachmentReference convert( renderer::AttachmentReference const & value )
	{
		return VkAttachmentReference
		{
			value.attachment,
			convert( value.layout )
		};
	}
}
