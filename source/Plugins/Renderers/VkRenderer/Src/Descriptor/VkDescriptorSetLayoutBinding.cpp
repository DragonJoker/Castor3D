/*
This file belongs to RendererLib.
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
			binding.getDescriptorsCount(),
			convert( binding.getShaderStageFlags() ),
		};
	}
}
