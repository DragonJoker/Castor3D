#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkSubpassContents convert( renderer::SubpassContents const & value )
	{
		switch ( value )
		{
		case renderer::SubpassContents::eInline:
			return VK_SUBPASS_CONTENTS_INLINE;

		case renderer::SubpassContents::eSecondaryCommandBuffers:
			return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

		default:
			assert( false && "Unsupported SubpassContents" );
			return VK_SUBPASS_CONTENTS_INLINE;
		}
	}
}
