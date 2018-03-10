/*
This file belongs to RendererLib.
See LICENSE file in root folder
*/
#pragma once

#include "VkRendererPrerequisites.hpp"

#include <Miscellaneous/DeviceMemory.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe encapsulant le stockage alloué à un tampon de données.
	*\~english
	*\brief
	*	Class wrapping a storage allocated to a data buffer.
	*/
	class DeviceMemory
		: public renderer::DeviceMemory
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] requirements
		*	Les exigences mémoire.
		*\param[in] flags
		*	Les indicateurs de propriétés voulues pour la mémoire allouée.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] requirements
		*	The memory requirements.
		*\param[in] flags
		*	The wanted memory flags.
		*/
		DeviceMemory( Device const & device
			, renderer::MemoryRequirements const & requirements
			, renderer::MemoryPropertyFlags flags );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~DeviceMemory();
		/**
		*\copydoc	renderer::DeviceMemory::lock
		*/
		uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags flags )const override;
		/**
		*\copydoc	renderer::DeviceMemory::flush
		*/
		void flush( uint32_t offset
			, uint32_t size )const override;
		/**
		*\copydoc	renderer::DeviceMemory::invalidate
		*/
		void invalidate( uint32_t offset
			, uint32_t size )const override;
		/**
		*\copydoc	renderer::DeviceMemory::unlock
		*/
		void unlock()const override;
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkDeviceMemory.
		*\~english
		*\brief
		*	VkDeviceMemory implicit cast operator.
		*/
		inline operator VkDeviceMemory const &()const
		{
			return m_memory;
		}

	private:
		Device const & m_device;
		VkDeviceMemory m_memory{};
	};
}
