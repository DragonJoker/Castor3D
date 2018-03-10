#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkAttachmentDescription convert( renderer::AttachmentDescription const & value )
	{
		return VkAttachmentDescription
		{
			0u,                                            // flags
			convert( value.format ),                       // format
			convert( value.samples ),                      // samples
			convert( value.loadOp ),                       // loadOp
			convert( value.storeOp ),                      // storeOp
			convert( value.stencilLoadOp ),                // stencilLoadOp
			convert( value.stencilStoreOp ),               // stencilStoreOp
			convert( value.initialLayout ),                // initialLayout
			convert( value.finalLayout )                   // finalLayout
		};
	}
}
