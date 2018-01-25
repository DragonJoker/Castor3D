/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	namespace vk
	{
#define VK_LIB_FUNCTION( fun ) PFN_vk##fun fun;
#include "VulkanFunctionsList.inl"
	}
}
