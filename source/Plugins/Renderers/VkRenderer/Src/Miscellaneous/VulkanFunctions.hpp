/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#pragma once

namespace vk_renderer
{
	namespace vk
	{
#define VK_LIB_FUNCTION( fun ) extern PFN_vk##fun fun;
#include "VulkanFunctionsList.inl"
	}
}
