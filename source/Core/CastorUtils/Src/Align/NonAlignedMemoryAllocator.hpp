/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CU_NON_ALIGNED_MEMORY_ALLOCATOR_H___
#define ___CU_NON_ALIGNED_MEMORY_ALLOCATOR_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		14/01/2016
	\~english
	\brief		Memory allocator.
	\remarks	Allocates non aligned memory.
	\~french
	\brief		Allocateur de mémoire.
	\remarks	Alloue de la mémoire non alignée.
	*/
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
		static uint8_t * Allocate( size_t p_size )
		{
			return new uint8_t[p_size];;
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
			delete [] p_space;
		}
	};
}

#endif
