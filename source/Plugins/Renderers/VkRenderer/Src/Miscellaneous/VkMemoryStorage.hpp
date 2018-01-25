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
	*	Classe encapsulant le stockage alloué à un tampon de données.
	*\~english
	*\brief
	*	Class wrapping a storage allocated to a data buffer.
	*/
	template< typename VkType, bool Image >
	class MemoryStorage
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le LogicalDevice parent.
		*\param[in] buffer
		*	Le descripteur du tampon Vulkan.
		*\param[in] flags
		*	Les indicateurs de propriétés voulues pour la mémoire allouée.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] buffer
		*	The Vulkan buffer handle.
		*\param[in] flags
		*	The wanted memory flags.
		*/
		inline MemoryStorage( Device const & device
			, VkType buffer
			, VkMemoryPropertyFlags flags );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		inline ~MemoryStorage();
		/**
		*\~french
		*\brief
		*	Mappe la mémoire du tampon en RAM.
		*\param[in] offset
		*	L'offset à partir duquel la mémoire du tampon est mappée.
		*\param[in] size
		*	La taille en octets de la mémoire à mapper.
		*\param[in] flags
		*	Indicateurs de configuration du mapping.
		*\return
		*	\p nullptr si le mapping a échoué.
		*\~english
		*\brief
		*	Maps the buffer's memory in RAM.
		*\param[in] offset
		*	The memory mapping starting offset.
		*\param[in] size
		*	The memory mappping size.
		*\param[in] flags
		*	The memory mapping flags.
		*\return
		*	\p nullptr if the mapping failed.
		*/
		inline uint8_t * lock( uint32_t offset
			, uint32_t size
			, VkMemoryMapFlags flags )const;
		/**
		*\~french
		*\brief
		*	Unmappe la mémoire du tampon de la RAM.
		*\param[in] size
		*	La taille en octets de la mémoire mappée.
		*\param[in] modified
		*	Dit si le tampon a été modifié, et donc si la VRAM doit être mise à jour.
		*\~english
		*\brief
		*	Unmaps the buffer's memory from the RAM.
		*\param[in] size
		*	The mapped memory size.
		*\param[in] modified
		*	Tells it the buffer has been modified, and whether the VRAM must be updated or not.
		*/
		inline void unlock( uint32_t size
			, bool modified )const;
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

#include "VkMemoryStorage.inl"
