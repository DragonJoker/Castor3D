/*
See LICENSE file in root folder
*/
#ifndef ___CU_ALIGNED_MEMORY_ALLOCATOR_H___
#define ___CU_ALIGNED_MEMORY_ALLOCATOR_H___

#include "CastorUtils/Align/Aligned.hpp"

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		14/01/2016
	\~english
	\brief		Memory allocator.
	\remarks	Allocates aligned memory.
	\~french
	\brief		Allocateur de mémoire.
	\remarks	Alloue de la mémoire alignée.
	*/
	template< size_t Align >
	class AlignedMemoryAllocator
	{
	public:
		/**
		 *\~english
		 *\brief		Allocates memory.
		 *\return		The allocated space memory address.
		 *\~french
		 *\brief		Alloue de la mémoire.
		 *\return		L'adresse mémoire de l'espace alloué.
		 */
		static uint8_t * allocate( size_t size )
		{
			return reinterpret_cast< uint8_t * >( alignedAlloc( Align, size ) );
		}
		/**
		 *\~english
		 *\brief		Frees the given memory.
		 *\param[in]	space	The memory to free.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\param[in]	space	La mémoire à libérer.
		 */
		static void deallocate( uint8_t * space )noexcept
		{
			return alignedFree( space );
		}
	};
}

#endif
