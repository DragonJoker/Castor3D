/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ALIGNED_H___
#define ___CASTOR_ALIGNED_H___

#include "CastorUtilsPrerequisites.hpp"

#include "Miscellaneous/Utils.hpp"

#if defined( CU_CompilerMSVC )
#	define CU_Alignas( a ) alignas( a )
#else
#	define CU_Alignas( a )
#endif

namespace castor
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
	CU_API void * alignedAlloc( size_t p_alignment, size_t p_size );
	/**
	 *\~english
	 *\brief		Deallocates aligned memory.
	 *\param[in]	p_memory	The memory.
	 *\~french
	 *\brief		Désalloue de la mémoire alignée.
	 *\param[in]	p_memory	La mémoire.
	 */
	CU_API void alignedFree( void * p_memory );
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
	T * alignedAlloc( size_t p_alignment, size_t p_size )
	{
		return reinterpret_cast< T * >( alignedAlloc( p_alignment, p_size ) );
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
	class CU_Alignas( A ) Aligned
	{
	public:
		static const size_t AlignValue = A;

		void * operator new( size_t p_size )
		{
			void * storage = castor::alignedAlloc( AlignValue, p_size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new[]( size_t p_size )
		{
			void * storage = castor::alignedAlloc( AlignValue, p_size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new( size_t p_size, std::nothrow_t )
		{
			return castor::alignedAlloc( AlignValue, p_size );
		}

		void operator delete( void * p_memory )
		{
			castor::alignedFree( p_memory );
		}

		void operator delete[]( void * p_memory )
		{
			castor::alignedFree( p_memory );
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
	class CU_Alignas( alignof( T ) ) AlignedFrom
	{
	public:
		static const size_t AlignValue = alignof( T );

		void * operator new( size_t p_size )
		{
			void * storage = castor::alignedAlloc( AlignValue, p_size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new[]( size_t p_size )
		{
			void * storage = castor::alignedAlloc( AlignValue, p_size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new( size_t p_size, std::nothrow_t )
		{
			return castor::alignedAlloc( AlignValue, p_size );
		}

		void operator delete( void * p_memory )
		{
			castor::alignedFree( p_memory );
		}

		void operator delete[]( void * p_memory )
		{
			castor::alignedFree( p_memory );
		}
	};
}

#endif
