/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Core/PhysicalDevice.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe contenant les informations liées au GPU physique.
	*\~english
	*\brief
	*	Wraps informations about physical GPU.
	*/
	class PhysicalDevice
		: public renderer::PhysicalDevice
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] vulkan
		*	L'instance Vulkan.
		*\param[in] gpu
		*	Le GPU physique.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] vulkan
		*	The Vulkan instance.
		*\param[in] gpu
		*	The Vulkan physical GPU.
		*/
		PhysicalDevice( Renderer & renderer
			, VkPhysicalDevice gpu );
		/**
		*\copydoc	renderer::PhysicalDevice::initialise
		*/
		void initialise()override;
		/**
		*\~french
		*\return
		*	Les noms des extensions supportées par le GPU.
		*\~english
		*\return
		*	The GPU's supported extensions names.
		*/
		inline auto const & getExtensionNames()const
		{
			return m_deviceExtensionNames;
		}
		/**
		*\~french
		*\return
		*	Les noms des couches supportées par le GPU.
		*\~english
		*\return
		*	The GPU's supported layers names.
		*/
		inline auto const & getLayerNames()const
		{
			return m_deviceLayerNames;
		}
		/**
		*\copydoc	renderer::PhysicalDevice::isSPIRVSupported
		*/
		inline bool isSPIRVSupported()const override
		{
			return true;
		}
		/**
		*\copydoc	renderer::PhysicalDevice::isGLSLSupported
		*/
		inline bool isGLSLSupported()const override
		{
			return VKRENDERER_GLSL_TO_SPV != 0;
		}
		/**
		*\~french
		*\brief
		*	Conversion implicite vers VkPhysicalDevice.
		*\~english
		*\brief
		*	VkPhysicalDevice implicit cast operator.
		*/
		inline operator VkPhysicalDevice const &()const
		{
			return m_gpu;
		}

	private:
		Renderer & m_renderer;
		VkPhysicalDevice m_gpu{ VK_NULL_HANDLE };
		std::vector< char const * > m_deviceExtensionNames;
		std::vector< char const * > m_deviceLayerNames;
	};
}
