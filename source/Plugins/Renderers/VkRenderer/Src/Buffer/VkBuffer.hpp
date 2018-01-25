/**
*\file
*	RenderingResources.h
*\author
*	Sylvain Doremus
*/
#ifndef ___VkRenderer_Buffer_HPP___
#define ___VkRenderer_Buffer_HPP___
#pragma once

#include "Miscellaneous/VkMemoryStorage.hpp"

#include <Buffer/Buffer.hpp>

namespace vk_renderer
{
	/**
	*\~french
	*\brief
	*	Classe regroupant les ressources de rendu nécessaires au dessin d'une image.
	*/
	class Buffer
		: public renderer::BufferBase
	{
	public:
		/**
		*\~french
		*\brief
		*	Constructeur.
		*\param[in] device
		*	Le périphérique logique.
		*\param[in] count
		*	Le nombre d'éléments du tampon.
		*\param[in] target
		*	Les indicateurs d'utilisation du tampon.
		*\param[in] memoryFlags
		*	Les indicateurs de mémoire du tampon.
		*\~english
		*\brief
		*	Constructor.
		*\param[in] device
		*	The logical connection to the GPU.
		*\param[in] size
		*	The buffer data size.
		*\param[in] target
		*	The buffer usage flags.
		*\param[in] memoryFlags
		*	The buffer memory flags.
		*/
		Buffer( Device const & device
			, uint32_t size
			, renderer::BufferTargets target
			, renderer::MemoryPropertyFlags memoryFlags );
		/**
		*\~french
		*\brief
		*	Destructeur.
		*\~english
		*\brief
		*	Destructor.
		*/
		~Buffer();
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
		uint8_t * lock( uint32_t offset
			, uint32_t size
			, renderer::MemoryMapFlags flags )const override;
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
		void unlock( uint32_t size
			, bool modified )const override;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de destination de transfert.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier to transfer destination layout.
		*\return
		*	The memory barrier.
		*/
		renderer::BufferMemoryBarrier makeTransferDestination()const override;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de source de transfert.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier to transfer source layout.
		*\return
		*	The memory barrier.
		*/
		renderer::BufferMemoryBarrier makeTransferSource()const override;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un vertex shader.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier to make a transition to vertex shader input resource (read only).
		*\return
		*	The memory barrier.
		*/
		renderer::BufferMemoryBarrier makeVertexShaderInputResource()const override;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout de ressource d'entrée (lecture seule) d'un shader.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier to make a transition to shader input resource (read only).
		*\return
		*	The memory barrier.
		*/
		renderer::BufferMemoryBarrier makeUniformBufferInput()const override;
		/**
		*\~french
		*\brief
		*	Prépare une barrière mémoire de transition vers un layout mémoire donné.
		*\param[in] dstAccess
		*	Les indicateurs d'accès voulus après la transition.
		*\return
		*	La barrière mémoire.
		*\~english
		*\brief
		*	Prepares a memory barrier to a given memory layout.
		*\param[in] dstAccess
		*	The access flags waanted after the transition.
		*\return
		*	The memory barrier.
		*/
		renderer::BufferMemoryBarrier makeMemoryTransitionBarrier( renderer::AccessFlags dstAccess )const override;
		/**
		*\~french
		*\brief
		*	Opérateur de conversion implicite vers VkBuffer.
		*\~english
		*\brief
		*	VkBuffer implicit cast operator.
		*/
		inline operator VkBuffer const &( )const
		{
			return m_buffer;
		}

	private:
		Device const & m_device;
		uint32_t m_size{ 0u };
		VkBuffer m_buffer{ VK_NULL_HANDLE };
		BufferStoragePtr m_storage;
		mutable VkAccessFlags m_currentAccessMask{ VK_ACCESS_MEMORY_WRITE_BIT };
	};
}

#endif
