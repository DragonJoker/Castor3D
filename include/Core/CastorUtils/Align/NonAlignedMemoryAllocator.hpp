/*
See LICENSE file in root folder
*/
#ifndef ___CU_NonAlignedMemoryAllocator_H___
#define ___CU_NonAlignedMemoryAllocator_H___

#include "CastorUtils/Align/AlignModule.hpp"

namespace castor
{
	class NonAlignedMemoryAllocator
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
			return new uint8_t[size];;
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
			delete [] space;
		}
	};
}

#endif
