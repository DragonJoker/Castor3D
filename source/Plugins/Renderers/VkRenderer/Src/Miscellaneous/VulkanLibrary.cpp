/*
This file belongs to VkLib.
See LICENSE file in root folder.
*/
#include "Miscellaneous/VulkanLibrary.hpp"

#include "VkError.hpp"

namespace vk_renderer
{
	PFN_vkGetInstanceProcAddr GetInstanceProcAddr;

	VulkanLibrary::VulkanLibrary()
#if defined( _WIN32 )
		: m_library{ "vulkan-1.dll" }
#else
		: m_library{ "libvulkan.so.1" }
#endif
	{
		try
		{
			m_library.getFunction( "vkGetInstanceProcAddr", GetInstanceProcAddr );

#define VK_LIB_FUNCTION( fun )\
			if ( !( m_library.getFunction( "vk"#fun, vk::fun ) ) )\
			{\
				throw std::runtime_error{ std::string{ "Couldn't load function " } + "vk"#fun };\
			}
#include "VulkanFunctionsList.inl"
		}
		catch ( std::exception & error )
		{
			std::cerr << error.what() << std::endl;
		}
	}
}
