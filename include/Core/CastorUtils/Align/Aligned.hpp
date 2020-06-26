/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ALIGNED_H___
#define ___CASTOR_ALIGNED_H___

#include "CastorUtils/Align/AlignModule.hpp"

#include "CastorUtils/Miscellaneous/Utils.hpp"

namespace castor
{
	/**
	 *\~english
	 *\brief		Allocates aligned memory.
	 *\param[in]	alignment	The alignment value, must be a power of two.
	 *\param[in]	size		The wanted size.
	 *\return		The allocated memory, nullptr on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	size		La taille désirée.
	 *\return		La mémoire allouée, nullptr en cas d'erreur.
	 */
	CU_API void * alignedAlloc( size_t alignment, size_t size );
	/**
	 *\~english
	 *\brief		Deallocates aligned memory.
	 *\param[in]	memory	The memory.
	 *\~french
	 *\brief		Désalloue de la mémoire alignée.
	 *\param[in]	memory	La mémoire.
	 */
	CU_API void alignedFree( void * memory );
	/**
	 *\~english
	 *\brief		Allocates aligned memory.
	 *\param[in]	alignment	The alignment value, must be a power of two.
	 *\param[in]	size		The wanted size.
	 *\return		The allocated memory, nullptr on error.
	 *\~french
	 *\brief		Alloue de la mémoire alignée.
	 *\param[in]	alignment	La valeur d'alignement, doit être une puissance de deux.
	 *\param[in]	size		La taille désirée.
	 *\return		La mémoire allouée, nullptr en cas d'erreur.
	 */
	template< typename T >
	T * alignedAlloc( size_t alignment, size_t size )
	{
		return reinterpret_cast< T * >( alignedAlloc( alignment, size ) );
	}

	template< int A >
	class CU_Alignas( A ) Aligned
	{
	public:
		static const size_t AlignValue = A;

		void * operator new( size_t size )
		{
			void * storage = castor::alignedAlloc( AlignValue, size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new[]( size_t size )
		{
			void * storage = castor::alignedAlloc( AlignValue, size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new( size_t size, std::nothrow_t )
		{
			return castor::alignedAlloc( AlignValue, size );
		}

		void operator delete( void * memory )
		{
			castor::alignedFree( memory );
		}

		void operator delete[]( void * memory )
		{
			castor::alignedFree( memory );
		}
	};

	template< typename T >
	class CU_Alignas( alignof( T ) ) AlignedFrom
	{
	public:
		static const size_t AlignValue = alignof( T );

		void * operator new( size_t size )
		{
			void * storage = castor::alignedAlloc( AlignValue, size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new[]( size_t size )
		{
			void * storage = castor::alignedAlloc( AlignValue, size );

			if ( !storage )
			{
				throw std::bad_alloc();
			}

			return storage;
		}

		void * operator new( size_t size, std::nothrow_t )
		{
			return castor::alignedAlloc( AlignValue, size );
		}

		void operator delete( void * memory )
		{
			castor::alignedFree( memory );
		}

		void operator delete[]( void * memory )
		{
			castor::alignedFree( memory );
		}
	};
}

#endif
