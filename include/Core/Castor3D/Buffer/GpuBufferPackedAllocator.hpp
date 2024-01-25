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
		 *\param[in]	size		The allocated size.
		 *\param[in]	alignSize	The alignment used for buffer chunks.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	size		La taille allouée.
		 *\param[in]	alignSize	L'alignement utilisé pour les chunks du buffer.
		 */
		C3D_API explicit GpuBufferPackedAllocator( size_t size
			, size_t alignSize = 1u );
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
		 *\return		The alignment used for buffer chunks.
		 *\~french
		 *\return		L'alignement utilisé pour les chunks du buffer.
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
		bool hasAvailable( size_t size )const
		{
			return size <= m_allocatedSize
				&& m_allocatedSize - size >= m_currentAllocated;
		}

	private:
		size_t m_allocatedSize{};
		size_t m_alignSize{};
		struct MemChunkCompare
		{
			bool operator()( MemChunk const & lhs, MemChunk const & rhs )const
			{
				return lhs.offset < rhs.offset;
			}
		};
		castor::Set< MemChunk, MemChunkCompare > m_allocated;
		castor::Vector< MemChunk > m_deallocated;
		VkDeviceSize m_currentAllocated{};
	};
}

#endif
