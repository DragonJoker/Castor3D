/*
This file belongs to VkLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

namespace vk_renderer
{
	extern PFN_vkGetInstanceProcAddr GetInstanceProcAddr;
	/**
	*\~french
	*\brief
	*	Loads vulkan functions.
	*\~english
	*\brief
	*	Charge les fonctions de vulkan.
	*/
	class VulkanLibrary
	{
	public:
		VulkanLibrary( VulkanLibrary const & ) = delete;
		VulkanLibrary & operator =( VulkanLibrary const & ) = delete;
		VulkanLibrary( VulkanLibrary && ) = default;
		VulkanLibrary & operator =( VulkanLibrary && ) = default;
		/**
		*\~english
		*\brief
		*	Constructor.
		*	Loads the functions.
		*\~french
		*\brief
		*	Constructeur.
		*	Charge les fonctions.
		*/
		VulkanLibrary();

	private:
		renderer::DynamicLibrary m_library;
	};
}
