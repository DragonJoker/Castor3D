/*
See LICENSE file in root folder
*/
#ifndef ___CU_FIXED_SIZE_MEMORY_DATA_H___
#define ___CU_FIXED_SIZE_MEMORY_DATA_H___

#include "CastorUtils/Pool/MemoryDataTyper.hpp"

#include <cstddef>

namespace castor
{
	template< typename Object, typename MemoryAllocator >
	class FixedSizeMemoryData
	{
		using Namer = MemoryDataNamer< MemoryDataType::eFixed >;

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
			m_buffer = MemoryAllocator::allocate( m_total * sizeof( Object ) );
			m_free = new Object * [m_total];
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;

			for ( size_t i = 0; i < m_total; ++i )
			{
				*m_freeIndex++ = reinterpret_cast< Object * >( m_bufferEnd );
				m_bufferEnd += sizeof( Object );
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
				reportError< PoolErrorType::eCommonMemoryLeaksDetected >( Namer::Name
					, size_t( ( m_freeEnd - m_freeIndex ) * sizeof( Object ) ) );
			}

			delete [] m_free;
			MemoryAllocator::deallocate( m_buffer );
			m_free = nullptr;
			m_buffer = nullptr;
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;
		}
		/**
		 *\~english
		 *\brief		Gives the address an available chunk.
		 *\return		nullptr if no memory available, the memory address if not.
		 *\~french
		 *\brief		donne un chunk mémoire disponible.
		 *\return		nullptr s'il n'y a plus de place disponible, l'adresse mémoire sinon.
		 */
		Object * allocate()noexcept
		{
			if ( m_freeIndex == m_free )
			{
				reportError< PoolErrorType::eCommonOutOfMemory >( Namer::Name );
				return nullptr;
			}

			return *--m_freeIndex;
		}
		/**
		 *\~english
		 *\brief		Frees the given memory.
		 *\remarks		Checks if the given address comes from the pool.
		 *\param[in]	space	The memory to free.
		 *\return		nullptr if no memory available, the memory address if not.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\remarks		Vérifie si la mémoire fait bien partie du pool.
		 *\param[in]	space	La mémoire à libérer.
		 *\return		true si la mémoire faisait partie du pool.
		 */
		bool deallocate( void * space )noexcept
		{
			if ( space )
			{
				if ( m_freeIndex == m_freeEnd )
				{
					reportError< PoolErrorType::eCommonPoolIsFull >( Namer::Name
						, reinterpret_cast< void * >( space ) );
					return false;
				}

				if ( ptrdiff_t( space ) < ptrdiff_t( m_buffer ) || ptrdiff_t( space ) >= ptrdiff_t( m_bufferEnd ) )
				{
					reportError< PoolErrorType::eCommonNotFromRange >( Namer::Name
						, reinterpret_cast< void * >( space ) );
					return false;
				}

				*m_freeIndex++ = reinterpret_cast< Object * >( space );
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
		Object ** m_free = nullptr;
		//!\~english	The free chunks' end.
		//!\~french		La fin des chunks libres.
		Object ** m_freeEnd = nullptr;
		//!\~english	The last allocated chunk.
		//!\~french		Le dernier chunk alloué.
		Object ** m_freeIndex = nullptr;
		//!\~english	The total pool capacity.
		//!\~french		Le nombre total possible d'éléments.
		size_t m_total = 0;
	};
}

#endif
