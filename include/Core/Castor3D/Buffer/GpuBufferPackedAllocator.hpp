/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferPackedAllocator_H___
#define ___C3D_GpuBufferPackedAllocator_H___

#include "BufferModule.hpp"

#include <vector>

namespace castor3d
{
	struct GpuBufferPackedAllocator
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	size	The allocated size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	size	La taille allouée.
		 */
		C3D_API GpuBufferPackedAllocator( size_t size );
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
			return 1u;
		}
		/**
		 *\~english
		 *\return		\p true if there is enough remaining memory for given size.
		 *\~french
		 *\return		\p true s'il y a assez de mémoire restante pour la taille donnée.
		 */
		bool hasAvailable( size_t size )const
		{
			return size < m_allocatedSize
				&& m_allocatedSize - size >= m_currentAllocated;
		}

	private:
		size_t m_allocatedSize{};
		struct MemChunkCompare
		{
			bool operator()( MemChunk const & lhs, MemChunk const & rhs )const
			{
				return lhs.offset < rhs.offset;
			}
		};
		std::set< MemChunk, MemChunkCompare > m_allocated;
		std::vector< MemChunk > m_deallocated;
		VkDeviceSize m_currentAllocated{};
	};
}

#endif
