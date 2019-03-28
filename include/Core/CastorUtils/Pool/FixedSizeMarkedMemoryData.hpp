/*
See LICENSE file in root folder
*/
#ifndef ___CU_FIXED_SIZE_MARKED_MEMORY_DATA_H___
#define ___CU_FIXED_SIZE_MARKED_MEMORY_DATA_H___

#include "CastorUtils/Pool/MemoryDataTyper.hpp"

#include <cstddef>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Memory allocation policy.
	\remarks	Allocates an additional byte, marks it, to be an overview of memory leaks.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire.
	\remarks	Alloue un octet de plus par objet, et marque cet octet, afin d'avoir un aperçu plus fin des fuites mémoires.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object >
	class FixedSizeMarkedMemoryData
	{
		using Namer = MemoryDataNamer< MemoryDataType::eMarked >;

		static const uint8_t NEVER_ALLOCATED = 0x00;
		static const uint8_t ALLOCATED = 0xAD;
		static const uint8_t FREED = 0xFE;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the pool with given objects count.
		 *\param[in]	count	The object's count.
		 *\~french
		 *\brief		Initialise le pool avec le nombre d'objets donné.
		 *\param[in]	count	Le compte des objets.
		 */
		void initialise( size_t count )noexcept
		{
			m_total = count;
			m_buffer = new uint8_t[m_total + m_total * sizeof( Object )];
			m_free = new uint8_t * [m_total];
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;
			size_t size = sizeof( Object ) + 1;

			for ( size_t i = 0; i < m_total; ++i )
			{
				*m_freeIndex++ = m_bufferEnd;
				// On marque le premier octet de la mémoire
				*m_bufferEnd = NEVER_ALLOCATED;
				m_bufferEnd += size;
			}

			m_freeEnd = m_freeIndex;
		}
		/**
		 *\~english
		 *\brief		Cleans the pool up, reports memory leaks.
		 *\~french
		 *\brief		Nettoie le pool, rapporte les fuites de mémoire.
		 */
		void cleanup()noexcept
		{
			if ( m_freeIndex != m_freeEnd )
			{
				reportError< PoolErrorType::eCommonMemoryLeaksDetected >( Namer::Name, size_t( ( m_freeEnd - m_freeIndex ) * sizeof( Object ) ) );
				uint8_t * buffer = m_buffer;
				size_t size = sizeof( Object ) + 1;

				for ( size_t i = 0; i < m_total; ++i )
				{
					if ( *buffer == ALLOCATED )
					{
						reportError< PoolErrorType::eMarkedLeakAddress >( Namer::Name, ( void * )( buffer + 1 ) );
					}

					buffer += size;
				}
			}

			delete [] m_free;
			delete [] m_buffer;
			m_free = nullptr;
			m_buffer = nullptr;
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;
		}
		/**
		 *\~english
		 *\brief		Gives the address an available chunk.
		 *\remarks		set the marked byte to "Allocated" state.
		 *\return		nullptr if no memory available, the memory address if not.
		 *\~french
		 *\brief		donne un chunk mémoire disponible.
		 *\remarks		Met l'octet de marquage dans l'état "Alloué".
		 *\return		nullptr s'il n'y a plus de place disponible, l'adresse mémoire sinon.
		 */
		Object * allocate()noexcept
		{
			if ( m_freeIndex == m_free )
			{
				reportError< PoolErrorType::eCommonOutOfMemory >( Namer::Name );
				return nullptr;
			}

			uint8_t * space = *--m_freeIndex;
			* space = ALLOCATED;
			return reinterpret_cast< Object * >( ++space );
		}
		/**
		 *\~english
		 *\brief		Frees the given memory.
		 *\remarks		Checks if the given address comes from the pool, and if it has been allocated by the pool, via the marked byte.
		 *\remarks		set the marked byte to "deallocate" state.
		 *\param[in]	space	The memory to free.
		 *\return		nullptr if no memory available, the memory address if not.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\remarks		Vérifie si la mémoire fait bien partie du pool, et si elle a bien été allouée par le pool, via l'octet de marquage.
						Met l'octet de marquage dans l'état "Libre".
		 *\param[in]	space	La mémoire à libérer.
		 *\return		true si la mémoire faisait partie du pool.
		 */
		bool deallocate( void * space )noexcept
		{
			if ( space )
			{
				if ( m_freeIndex == m_freeEnd )
				{
					reportError< PoolErrorType::eCommonPoolIsFull >( Namer::Name, ( void * )space );
					return false;
				}

				if ( ptrdiff_t( space ) < ptrdiff_t( m_buffer ) || ptrdiff_t( space ) >= ptrdiff_t( m_bufferEnd ) )
				{
					reportError< PoolErrorType::eCommonNotFromRange >( Namer::Name, ( void * )space );
					return false;
				}

				uint8_t * marked = reinterpret_cast< uint8_t * >( space );
				--marked;

				if ( *marked != ALLOCATED )
				{
					if ( *marked == FREED )
					{
						reportError< PoolErrorType::eMarkedDoubleDelete >( Namer::Name, ( void * )space );
						return false;
					}

					reportError< PoolErrorType::eMarkedNotFromPool >( Namer::Name, ( void * )space );
					return false;
				}

				*marked = FREED;
				*m_freeIndex++ = marked;
				return true;
			}
			else
			{
				return true;
			}
		}

	private:
		//!\~english	The buffer.
		//!\~french		Le tampon.
		uint8_t * m_buffer = nullptr;
		//!\~english	Pointer to the buffer's end.
		//!\~french		Pointeur sur la fin du tampon.
		uint8_t * m_bufferEnd = nullptr;
		//!\~english	The free chunks.
		//!\~french		Les chunks libres.
		uint8_t ** m_free = nullptr;
		//!\~english	The free chunks' end.
		//!\~french		La fin des chunks libres.
		uint8_t ** m_freeEnd = nullptr;
		//!\~english	The last allocated chunk.
		//!\~french		Le dernier chunk alloué.
		uint8_t ** m_freeIndex = nullptr;
		//!\~english	The total pool capacity.
		//!\~french		Le nombre total possible d'éléments.
		size_t m_total = 0;
	};
}

#endif // ___FIXED_SIZE_MARKED_MEMORY_DATA_H___
