/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	VkDescriptorSetLayoutBinding convert( renderer::DescriptorSetLayoutBinding const & binding )
	{
		return VkDescriptorSetLayoutBinding
		{
			binding.getBindingPoint(),
			convert( binding.getDescriptorType() ),
			1u,
			convert( binding.getShaderStageFlags() ),
		};
	}
}
