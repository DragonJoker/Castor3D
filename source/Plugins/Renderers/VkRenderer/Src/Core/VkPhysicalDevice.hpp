/*
This file belongs to VkLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

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
		*\~french
		*\brief
		*	Déduit le type de mémoire à allouer, en fonction des informations données.
		*\param[in] typeBits
		*	Il y a 32 types de mémoire décrits dans VkPhysicalDeviceMemoryProperties.
		*	Ce paramètre est une combinaison bit à bit décrivant les propriétés dans lesquelles
		*	il faut chercher.
		*\param[in] requirements
		*	Les pré-requis pour le type de mémoire.
		*\param[in] typeIndex
		*	Reçoit l'index du type de mémoire déduit.
		*\return
		*	\p true si le type a pu être déduit, \p false sinon.
		*\~english
		*\brief
		*	Deduces the memory type, from the given informations.
		*\param[in] typeBits
		*	There are 32 memory types described in VkPhysicalDeviceMemoryProperties.
		*	This parameter is a bitwise combinartion describing the properties to look for.
		*\param[in] requirements
		*	The memory quirements.
		*\param[in] typeIndex
		*	Receives the deduced memory type index.
		*\return
		*	\p false if the memory type could not be deduced.
		*/ 
		bool deduceMemoryType( uint32_t typeBits
			, VkMemoryPropertyFlags requirements
			, uint32_t & typeIndex )const;
		/**
		*\~french
		*\return
		*	Les propriétés des files du GPU.
		*\~english
		*\return
		*	The GPU's queues properties.
		*/
		inline auto const & getQueueProperties()const
		{
			return m_queueProperties;
		}
		/**
		*\~french
		*\return
		*	Les propriétés du GPU.
		*\~english
		*\return
		*	The GPU's properties.
		*/
		inline auto const & getProperties()const
		{
			return m_properties;
		}
		/**
		*\~french
		*\return
		*	Les propriétés de mémoire du GPU.
		*\~english
		*\return
		*	The GPU's memory properties.
		*/
		inline auto const & getMemoryProperties()const
		{
			return m_memoryProperties;
		}
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
		VkPhysicalDeviceMemoryProperties m_memoryProperties{};
		VkPhysicalDeviceFeatures m_features{};
		VkPhysicalDeviceProperties m_properties{};
		std::vector< VkQueueFamilyProperties > m_queueProperties;
		std::vector< char const * > m_deviceExtensionNames;
		std::vector< char const * > m_deviceLayerNames;
	};
}
