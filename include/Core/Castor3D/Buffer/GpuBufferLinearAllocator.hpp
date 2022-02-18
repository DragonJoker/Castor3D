/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferLinearAllocator_H___
#define ___C3D_GpuBufferLinearAllocator_H___

#include "BufferModule.hpp"

#include <vector>

namespace castor3d
{
	struct GpuBufferLinearAllocator
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	count		The allocator elements count.
		 *\param[in]	alignment	The alignment for one element.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	count		Le nombre d'éléments de l'allocateur.
		 *\param[in]	alignment	L'alignement de base d'un élément.
		 */
		C3D_API GpuBufferLinearAllocator( size_t count
			, uint32_t alignment );
		/**
		 *\~english
		 *\brief		Allocates memory.
		 *\param[in]	size	The requested memory size.
		 *\return		The memory chunk.
		 *\~french
		 *\brief		Alloue de la mémoire.
		 *\param[in]	size	La taille requiese pour la mémoire.
		 *\return		La zone mémoire.
		 */
		C3D_API VkDeviceSize allocate( size_t size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	pointer	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	pointer	La zone mémoire.
		 */
		C3D_API void deallocate( VkDeviceSize pointer );
		/**
		 *\~english
		 *\return		The allocator size.
		 *\~french
		 *\return		La taille de l'allocateur.
		 */
		size_t getTotalSize()const
		{
			return m_allocatedSize;
		}
		/**
		 *\~english
		 *\return		The element aligned size.
		 *\~french
		 *\return		La taille  alignée d'un élément.
		 */
		size_t getAlignSize()const
		{
			return m_alignSize;
		}
		/**
		 *\~english
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		bool hasAvailable( size_t )const
		{
			return !m_free.empty();
		}

	private:
		uint32_t m_alignSize;
		size_t m_allocatedSize;
		std::vector< VkDeviceSize > m_free;
	};
}

#endif
