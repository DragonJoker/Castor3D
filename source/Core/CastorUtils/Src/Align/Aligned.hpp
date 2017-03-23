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
#ifndef ___CASTOR_ALIGNED_H___
#define ___CASTOR_ALIGNED_H___

#include "CastorUtilsPrerequisites.hpp"

#include "Miscellaneous/Utils.hpp"

#if defined( CASTOR_COMPILER_MSVC )
#	define CASTOR_ALIGNED_DECL( a ) alignas( a )
#else
#	define CASTOR_ALIGNED_DECL( a )
#endif

namespace Castor
{
	/**
	 *\~english
	 *\brief		Allocates aligned memory.
	 *\param[in]	p_alignment	The alignment value, must be a power of two.
	 *\param[in]	p_size		The wanted size.
	 *\return		The allocated memory, nullptr on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	p_alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	p_size		La taille désirée.
	 *\return		La mémoire allouée, nullptr en cas d'erreur.
	 */
	CU_API void * AlignedAlloc( size_t p_alignment, size_t p_size );
	/**
	 *\~english
	 *\brief		Deallocates aligned memory.
	 *\param[in]	p_memory	The memory.
	 *\~french
	 *\brief		Désalloue de la mémoire alignée.
	 *\param[in]	p_memory	La mémoire.
	 */
	CU_API void AlignedFree( void * p_memory );
	/**
	 *\~english
	 *\brief		Allocates aligned memory.
	 *\param[in]	p_alignment	The alignment value, must be a power of two.
	 *\param[in]	p_size		The wanted size.
	 *\return		The allocated memory, nullptr on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	p_alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	p_size		La taille désirée.
	 *\return		La mémoire allouée, nullptr en cas d'erreur.
	 */
	template< typename T >
	T * AlignedAlloc( size_t p_alignment, size_t p_size )
	{
		return reinterpret_cast< T * >( AlignedAlloc( p_alignment, p_size ) );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		03/10/2015
	\~english
	\brief		Templated class that provide alignment support.
	\~french
	\brief		Classe template qui fournit le support de l'alignement.
	*/
	template< int A >
	class CASTOR_ALIGNED_DECL( A ) Aligned
	{
	public:
		static const size_t align_value = A;

		void * operator new( size_t p_size )
		{
			void * l_storage = Castor::AlignedAlloc( align_value, p_size );

			if ( !l_storage )
			{
				throw std::bad_alloc();
			}

			return l_storage;
		}

		void * operator new[]( size_t p_size )
		{
			void * l_storage = Castor::AlignedAlloc( align_value, p_size );

			if ( !l_storage )
			{
				throw std::bad_alloc();
			}

			return l_storage;
		}

		void * operator new( size_t p_size, std::nothrow_t )
		{
			return Castor::AlignedAlloc( align_value, p_size );
		}

		void operator delete( void * p_memory )
		{
			Castor::AlignedFree( p_memory );
		}

		void operator delete[]( void * p_memory )
		{
			Castor::AlignedFree( p_memory );
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		03/10/2015
	\~english
	\brief		Templated class that provide alignment support.
	\~french
	\brief		Classe template qui fournit le support de l'alignement.
	*/
	template< typename T >
	class CASTOR_ALIGNED_DECL( alignof( T ) ) AlignedFrom
	{
	public:
		static const size_t align_value = alignof( T );

		void * operator new( size_t p_size )
		{
			void * l_storage = Castor::AlignedAlloc( align_value, p_size );

			if ( !l_storage )
			{
				throw std::bad_alloc();
			}

			return l_storage;
		}

		void * operator new[]( size_t p_size )
		{
			void * l_storage = Castor::AlignedAlloc( align_value, p_size );

			if ( !l_storage )
			{
				throw std::bad_alloc();
			}

			return l_storage;
		}

		void * operator new( size_t p_size, std::nothrow_t )
		{
			return Castor::AlignedAlloc( align_value, p_size );
		}

		void operator delete( void * p_memory )
		{
			Castor::AlignedFree( p_memory );
		}

		void operator delete[]( void * p_memory )
		{
			Castor::AlignedFree( p_memory );
		}
	};
}

#endif
