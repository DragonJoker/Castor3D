/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CU_ALIGNED_MEMORY_ALLOCATOR_H___
#define ___CU_ALIGNED_MEMORY_ALLOCATOR_H___

#include "Aligned.hpp"

namespace Castor
{
	/*!
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
		static uint8_t * Allocate( uint32_t p_size )
		{
			return reinterpret_cast< uint8_t * >( AlignedAlloc( Align, p_size ) );
		}		
		/**
		 *\~english
		 *\brief		Frees the given memory.
		 *\param[in]	p_space	The memory to free.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\param[in]	p_space	La mémoire à libérer.
		 */
		static void Deallocate( uint8_t * p_space )noexcept
		{
			return AlignedFree( p_space );
		}
	};
}

#endif
