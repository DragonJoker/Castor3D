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
#ifndef ___CU_BuddyAllocator_HPP___
#define ___CU_BuddyAllocator_HPP___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		Buddy allocator implementation.
	\~french
	\brief		Implémentation d'un buddy allocator.
	*/
	class BuddyAllocator
	{
	private:
		struct Block
		{
			uint8_t * data;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	numLevels		The allocator maximum tree size.
		 *\param[in]	minBlockSize	The minimum size for a block.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	numLevels		La taille maximale de l'arbre de l'allocateur.
		 *\param[in]	minBlockSize	La taille minimale d'un bloc.
		 */
		CU_API BuddyAllocator( uint32_t numLevels
			, uint32_t minBlockSize );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		CU_API ~BuddyAllocator();
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
		CU_API uint8_t * allocate( size_t size );
		/**
		 *\~english
		 *\brief		Deallocates memory.
		 *\param[in]	pointer	The memory chunk.
		 *\~french
		 *\brief		Désalloue de la mémoire.
		 *\param[in]	pointer	La zone mémoire.
		 */
		CU_API void deallocate( void * pointer );
		/**
		 *\~english
		 *\return		The allocator size.
		 *\~french
		 *\return		La taille de l'allocateur.
		 */
		inline size_t getSize()
		{
			return m_memory.size();
		}
		/**
		 *\~english
		 *\return		The allocator memory pointer.
		 *\~french
		 *\return		Le pointeur sur la mémoire de l'allocateur.
		 */
		inline uint8_t const * const constPtr()const
		{
			return m_memory.data();
		}

	private:
		uint32_t doGetLevel( size_t size );
		size_t doGetLevelSize( uint32_t level );
		Block doAllocate( uint32_t order );
		void doMergeLevel( uint32_t level );

	private:
		uint32_t m_numLevels;
		uint32_t m_minBlockSize;
		std::vector< uint8_t > m_memory;
		using FreeList = std::list< Block >;
		std::vector< FreeList > m_freeLists;
		using PointerLevel = std::pair< size_t, uint32_t >;
		std::vector< PointerLevel > m_allocated;
	};
}

#endif
