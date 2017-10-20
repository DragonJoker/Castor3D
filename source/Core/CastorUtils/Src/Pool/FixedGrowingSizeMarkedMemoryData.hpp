/*
See LICENSE file in root folder
*/
#ifndef ___CU_FIXED_GROWING_SIZE_MARKED_MEMORY_DATA_H___
#define ___CU_FIXED_GROWING_SIZE_MARKED_MEMORY_DATA_H___

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
	\remarks	Allocates an additional byte, marks it, to be an overview of memory leaks.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire, pouvant grandir, d'un nombre fixe d'objets.
	\remarks	Alloue un octet de plus par objet, et marque cet octet, afin d'avoir un aperçu plus fin des fuites mémoires.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object >
	class FixedGrowingSizeMarkedMemoryData
	{
		using Namer = MemoryDataNamer< MemoryDataType::eFixedGrowingMarked >;

		static const uint8_t NEVER_ALLOCATED = 0x00;
		static const uint8_t ALLOCATED = 0xAD;
		static const uint8_t FREED = 0xFE;

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
				size_t size = sizeof( Object ) + 1;

				for ( auto buffer = m_buffers; buffer != m_buffersEnd; ++buffer )
				{
					uint8_t * data = buffer->m_data;

					for ( size_t i = 0; i < m_step; ++i )
					{
						if ( *data == ALLOCATED )
						{
							reportError< PoolErrorType::eMarkedLeakAddress >( Namer::Name, ( void * )( data + 1 ) );
						}

						data += size;
					}
				}
			}

			delete [] m_free;

			for ( auto buffer = m_buffers; buffer != m_buffersEnd; ++buffer )
			{
				delete [] buffer->m_data;
			}
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
				doCreateBuffer();
			}

			if ( m_freeIndex == m_free )
			{
				reportError< PoolErrorType::eCommonOutOfMemory >( Namer::Name );
				return nullptr;
			}

			uint8_t * space = *--m_freeIndex;
			* space++ = ALLOCATED;
			return reinterpret_cast< Object * >( space );
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

				uint8_t * marked = reinterpret_cast< uint8_t * >( p_space );
				marked--;

				if ( *marked != ALLOCATED )
				{
					if ( *marked == FREED )
					{
						reportError< PoolErrorType::eMarkedDoubleDelete >( Namer::Name, ( void * )p_space );
						return false;
					}

					reportError< PoolErrorType::eMarkedNotFromPool >( Namer::Name, ( void * )p_space );
					return false;
				}

				if ( m_buffersEnd == std::find_if(	m_buffers, m_buffersEnd,
													[&marked]( buffer const & buffer )
													{
														return ptrdiff_t( marked ) >= ptrdiff_t( buffer.m_data ) && ptrdiff_t( marked ) < ptrdiff_t( buffer.m_end );
													} ) )
				{
					reportError< PoolErrorType::eGrowingNotFromRanges >( Namer::Name, ( void * )p_space );
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
			m_buffersEnd->m_data = new uint8_t[m_step * ( sizeof( Object ) + 1 )];
			m_buffersEnd->m_end = nullptr;
			uint8_t * buffer = m_buffersEnd->m_data;
			m_free = reinterpret_cast< uint8_t ** >( realloc( m_free, m_total * sizeof( uint8_t * ) ) );
			m_freeEnd = m_free + m_total;
			m_freeIndex = m_free;

			for ( size_t i = 0; i < m_step; ++i )
			{
				*m_freeIndex++ = buffer;
				// On marque le premier octet de la mémoire
				*buffer = NEVER_ALLOCATED;
				buffer += sizeof( Object ) + 1;
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
		uint8_t ** m_free = nullptr;
		//!\~english The free chunks' end.	\~french La fin des chunks libres.
		uint8_t ** m_freeEnd = nullptr;
		//!\~english The last allocated chunk.	\~french Le dernier chunk alloué.
		uint8_t ** m_freeIndex = nullptr;
		//!\~english The allocated object size	\~french La taille d'un objet alloué.
		size_t m_objectSize = 0;
		//!\~english The size increment.	\~french L'incrément de taille.
		size_t m_step = 0;
		//!\~english The total allocated size.	\~french La taille totale allouée.
		size_t m_total = 0;
	};
}

#endif
