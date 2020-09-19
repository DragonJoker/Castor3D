/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferBuffyAllocator_H___
#define ___C3D_GpuBufferBuffyAllocator_H___

#include "BufferModule.hpp"

namespace castor3d
{
	struct GpuBufferBuddyAllocatorTraits
	{
		using PointerType = VkDeviceSize;
		struct Block
		{
			PointerType data;
		};
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	size	The allocator size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	size	La taille de l'allocateur.
		 */
		inline explicit GpuBufferBuddyAllocatorTraits( size_t size )
			: m_allocatedSize{ size }
		{
		}
		/**
		 *\~english
		 *\return		The allocator size.
		 *\~french
		 *\return		La taille de l'allocateur.
		 */
		inline size_t getSize()const
		{
			return m_allocatedSize;
		}
		/**
		 *\~english
		 *\brief		Creates a memory block.
		 *\param[in]	offset	The block memory offset.
		 *\return		The block.
		 *\~french
		 *\brief		Crée un bloc de mémoire.
		 *\param[in]	offset	Le décalage en mémoire du bloc.
		 *\return		Le block.
		 */
		inline PointerType getPointer( uint32_t offset )
		{
			return offset;
		}
		/**
		 *\~english
		 *\brief		Retrieves the offset from a memory block.
		 *\param[in]	pointer	The memory block.
		 *\return		The offset.
		 *\~french
		 *\brief		Récupère le décalage en mémoire d'un block.
		 *\param[in]	pointer	Le bloc mémoire.
		 *\return		Le décalage.
		 */
		inline size_t getOffset( PointerType pointer )const
		{
			return size_t( pointer );
		}
		/**
		 *\~english
		 *\return		The null memory block.
		 *\~french
		 *\return		Le block mémoire nul.
		 */
		inline Block getNull()const
		{
			static Block result{ ~0u };
			return result;
		}
		/**
		 *\~english
		 *\return		The null memory block.
		 *\~french
		 *\return		Le block mémoire nul.
		 */
		inline bool isNull( PointerType pointer )const
		{
			return pointer == getNull().data;
		}

	private:
		size_t m_allocatedSize;
	};
}

#endif
