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
#ifndef ___CU_FIXED_SIZE_MARKED_MEMORY_DATA_H___
#define ___CU_FIXED_SIZE_MARKED_MEMORY_DATA_H___

#include "MemoryDataTyper.hpp"

#include <cstddef>

namespace Castor
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
		using Namer = MemoryDataNamer< eMEMDATA_TYPE_FIXED_MARKED >;

		static const uint8_t NEVER_ALLOCATED = 0x00;
		static const uint8_t ALLOCATED = 0xAD;
		static const uint8_t FREED = 0xFE;

	protected:
		/**
		 *\~english
		 *\brief		Initialises the pool with given objects count.
		 *\param[in]	p_count	The object's count.
		 *\~french
		 *\brief		Initialise le pool avec le nombre d'objets donné.
		 *\param[in]	p_count	Le compte des objets.
		 */
		void Initialise( size_t p_count )noexcept
		{
			m_total = p_count;
			m_buffer = new uint8_t[m_total + m_total * sizeof( Object )];
			m_free = new uint8_t *[m_total];
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;
			size_t l_size = sizeof( Object ) + 1;

			for ( size_t i = 0; i < m_total; ++i )
			{
				*m_freeIndex++ = m_bufferEnd;
				// On marque le premier octet de la mémoire
				*m_bufferEnd = NEVER_ALLOCATED;
				m_bufferEnd += l_size;
			}

			m_freeEnd = m_freeIndex;
		}
		/**
		 *\~english
		 *\brief		Cleans the pool up, reports memory leaks.
		 *\~french
		 *\brief		Nettoie le pool, rapporte les fuites de mémoire.
		 */
		void Cleanup()noexcept
		{
			if ( m_freeIndex != m_freeEnd )
			{
				ReportError< ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED >( Namer::Name, size_t( ( m_freeEnd - m_freeIndex ) * sizeof( Object ) ) );
				uint8_t * l_buffer = m_buffer;
				size_t l_size = sizeof( Object ) + 1;

				for ( size_t i = 0; i < m_total; ++i )
				{
					if ( *l_buffer == ALLOCATED )
					{
						ReportError< ePOOL_ERROR_TYPE_MARKED_LEAK_ADDRESS >( Namer::Name, ( void * )( l_buffer + 1 ) );
					}

					l_buffer += l_size;
				}
			}

			delete [] m_free;
			delete [] m_buffer;
			m_free = NULL;
			m_buffer = NULL;
			m_freeIndex = m_free;
			m_bufferEnd = m_buffer;
		}		
		/**
		 *\~english
		 *\brief		Gives the address an available chunk.
		 *\remarks		Set the marked byte to "Allocated" state.
		 *\return		NULL if no memory available, the memory address if not.
		 *\~french
		 *\brief		Donne un chunk mémoire disponible.
		 *\remarks		Met l'octet de marquage dans l'état "Alloué".
		 *\return		NULL s'il n'y a plus de place disponible, l'adresse mémoire sinon.
		 */
		Object * Allocate()noexcept
		{
			if ( m_freeIndex == m_free )
			{
				ReportError< ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY >( Namer::Name );
				return NULL;
			}
		
			uint8_t * l_space = *--m_freeIndex;
			*l_space = ALLOCATED;
			return reinterpret_cast< Object * >( ++l_space );
		}		
		/**
		 *\~english
		 *\brief		Frees the given memory.
		 *\remarks		Checks if the given address comes from the pool, and if it has been allocated by the pool, via the marked byte.
		 *\remarks		Set the marked byte to "Free" state.
		 *\param[in]	p_space	The memory to free.
		 *\return		NULL if no memory available, the memory address if not.
		 *\~french
		 *\brief		Libère la mémoire donnée.
		 *\remarks		Vérifie si la mémoire fait bien partie du pool, et si elle a bien été allouée par le pool, via l'octet de marquage.
						Met l'octet de marquage dans l'état "Libre".
		 *\param[in]	p_space	La mémoire à libérer.
		 *\return		true si la mémoire faisait partie du pool.
		 */
		bool Deallocate( void * p_space )noexcept
		{
			if ( p_space )
			{
				if ( m_freeIndex == m_freeEnd )
				{
					ReportError< ePOOL_ERROR_TYPE_COMMON_POOL_IS_FULL >( Namer::Name, ( void * )p_space );
					return false;
				}

				if ( ptrdiff_t( p_space ) < ptrdiff_t( m_buffer ) || ptrdiff_t( p_space ) >= ptrdiff_t( m_bufferEnd ) )
				{
					ReportError< ePOOL_ERROR_TYPE_COMMON_NOT_FROM_RANGE >( Namer::Name, ( void * )p_space );
					return false;
				}

				uint8_t * l_marked = reinterpret_cast< uint8_t * >( p_space );
				--l_marked;

				if ( *l_marked != ALLOCATED )
				{
					if ( *l_marked == FREED )
					{
						ReportError< ePOOL_ERROR_TYPE_MARKED_DOUBLE_DELETE >( Namer::Name, ( void * )p_space );
						return false;
					}

					ReportError< ePOOL_ERROR_TYPE_MARKED_NOT_FROM_POOL >( Namer::Name, ( void * )p_space );
					return false;
				}

				*l_marked = FREED;
				*m_freeIndex++ = l_marked;
				return true;
			}
			else
			{
				return true;
			}
		}

	private:
		//!\~english The buffer.	\~french Le tampon.
		uint8_t * m_buffer = NULL;
		//!\~english Pointer to the buffer's end.	\~french Pointeur sur la fin du tampon.
		uint8_t * m_bufferEnd = NULL;
		//!\~english The free chunks.	\~french Les chunks libres.
		uint8_t ** m_free = NULL;
		//!\~english The free chunks' end.	\~french La fin des chunks libres.
		uint8_t ** m_freeEnd = NULL;
		//!\~english The last allocated chunk.	\~french Le dernier chunk alloué.
		uint8_t ** m_freeIndex = NULL;
		//!\~english The total pool capacity.	\~french Le nombre total possible d'éléments.
		size_t m_total = 0;
	};
}

#endif // ___FIXED_SIZE_MARKED_MEMORY_DATA_H___
