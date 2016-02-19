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
#ifndef ___CASTOR_ALIGNED_H___
#define ___CASTOR_ALIGNED_H___

#include "CastorUtilsPrerequisites.hpp"

#include "Utils.hpp"

#if defined( _MSC_VER )
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
	 *\return		The allocated memory, NULL on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	p_alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	p_size		La taille désirée.
	 *\return		La mémoire allouée, NULL en cas d'erreur.
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
	 *\return		The allocated memory, NULL on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	p_alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	p_size		La taille désirée.
	 *\return		La mémoire allouée, NULL en cas d'erreur.
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
