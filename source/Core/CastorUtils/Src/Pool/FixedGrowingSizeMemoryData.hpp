/*
See LICENSE file in root folder
*/
#ifndef ___CU_FIXED_GROWING_SIZE_MEMORY_DATA_H___
#define ___CU_FIXED_GROWING_SIZE_MEMORY_DATA_H___

#include "MemoryDataTyper.hpp"

#include <algorithm>
#include <cstddef>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/01/2016
	\~english
	\brief		Memory allocation policy. It can grow of a fixed objects count.
	\remarks	Variable size, basic memory leaks check.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire, pouvant grandir, d'un nombre fixe d'objets.
	\remarks	Taille variable, check basique des fuites mémoire.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object, typename MemoryAllocator >
	class FixedGrowingSizeMemoryData
	{
		using Namer = MemoryDataNamer< MemoryDataType::eFixedGrowing >;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the pool with given objects count.
		 *\param[in]	p_count	The object's count, also defines the pool's growing size, if it is empty.
		 *\~french
		 *\brief		Initialise le pool avec le nombre d'objets donné.
		 *\param[in]	p_count	Le compte des objets, détermine aussi de combien le pool va grandir, s'il est vide.
		 */
		void initialise( size_t p_count )noexcept
		{
			m_step = p_count;
			m_total = 0;
			m_free = nullptr;
			m_freeIndex = m_free;
			m_buffers = nullptr;
			m_buffersEnd = m_buffers;
			doCreateBuffer();
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
			}

			delete [] m_free;

			for ( auto buffer = m_buffers; buffer != m_buffersEnd; ++buffer )
			{
				MemoryAllocator::deallocate( buffer->m_data );
			}

			m_free = nullptr;
			m_buffers = nullptr;
			m_freeIndex = m_free;
			m_buffersEnd = m_buffers;
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
				doCreateBuffer();
			}

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
		 *\param[in]	p_space	The memory to free.
		 *\return		nullptr if no memory available, the memory address if not.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\remarks		Vérifie si la mémoire fait bien partie du pool.
		 *\param[in]	p_space	La mémoire à libérer.
		 *\return		true si la mémoire faisait partie du pool.
		 */
		bool deallocate( void * p_space )noexcept
		{
			if ( p_space )
			{
				if ( m_freeIndex == m_freeEnd )
				{
					reportError< PoolErrorType::eCommonPoolIsFull >( Namer::Name, ( void * )p_space );
					return false;
				}

				if ( m_buffersEnd == std::find_if(	m_buffers, m_buffersEnd,
													[&p_space]( buffer const & buffer )
													{
														return ptrdiff_t( p_space ) >= ptrdiff_t( buffer.m_data ) && ptrdiff_t( p_space ) < ptrdiff_t( buffer.m_end );
													} ) )
				{
					reportError< PoolErrorType::eGrowingNotFromRanges >( Namer::Name, ( void * )p_space );
					return false;
				}

				*m_freeIndex++ = reinterpret_cast< Object * >( p_space );
				return true;
			}

			return true;
		}

	private:
		/**
		 *\~english
		 *\brief		Creates a buffer which size is the size given to Initialise method.
		 *\~french
		 *\brief		Crée un tampon de la taille donnée à la méthode Initialise.
		 */
		void doCreateBuffer()noexcept
		{
			m_total += m_step;
			ptrdiff_t count = m_buffersEnd - m_buffers;
			m_buffers = reinterpret_cast< buffer * >( realloc( m_buffers, ( count + 1 ) * sizeof( buffer ) ) );
			m_buffersEnd = m_buffers + count;
			m_buffersEnd->m_data = MemoryAllocator::allocate( m_step * sizeof( Object ) );
			m_buffersEnd->m_end = nullptr;
			auto buffer = m_buffersEnd->m_data;
			m_free = reinterpret_cast< Object ** >( realloc( m_free, m_total * sizeof( Object * ) ) );
			m_freeEnd = m_free + m_total;
			m_freeIndex = m_free;

			for ( size_t i = 0; i < m_step; ++i )
			{
				*m_freeIndex++ = reinterpret_cast< Object * >( buffer );
				buffer += sizeof( Object );
			}

			m_buffersEnd->m_end = buffer;
			m_buffersEnd++;
		}

	private:
		struct buffer
		{
			//!\~english The buffer.	\~french Le tampon.
			uint8_t * m_data = nullptr;
			//!\~english Pointer to the buffer's end.	\~french Pointeur sur la fin du tampon.
			uint8_t * m_end = nullptr;
		};
		//!\~english The buffers.	\~french Les tampons.
		buffer * m_buffers = nullptr;
		//!\~english Pointer to the buffers' end.	\~french Pointeur sur la fin des tampons.
		buffer * m_buffersEnd = nullptr;
		//!\~english The free chunks.	\~french Les chunks libres.
		Object ** m_free = nullptr;
		//!\~english The free chunks' end.	\~french La fin des chunks libres.
		Object ** m_freeEnd = nullptr;
		//!\~english The las allocated chunk.	\~french Le dernier chunk alloué.
		Object ** m_freeIndex = nullptr;
		//!\~english The size increment.	\~french L'incrément de taille.
		size_t m_step = 0;
		//!\~english The total allocated size.	\~french La taille totale allouée.
		size_t m_total = 0;
	};
}

#endif
