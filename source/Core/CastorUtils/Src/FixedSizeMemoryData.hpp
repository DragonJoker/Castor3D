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
#ifndef ___CU_FIXED_SIZE_MEMORY_DATA_H___
#define ___CU_FIXED_SIZE_MEMORY_DATA_H___

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
	\remarks	Fixed size, basic memory leaks check.
				Holds the memory buffers, free chunks and currently allocated objects count.
	\~french
	\brief		Politique d'allocation de mémoire.
	\remarks	Taille fixe, check basique des fuites mémoire.
				Contient le tampon mémoire, les chunks libres, et le nombre d'objets actuellement alloués.
	*/
	template< typename Object, typename MemoryAllocator >
	class FixedSizeMemoryData
	{
		using Namer = MemoryDataNamer< eMEMDATA_TYPE_FIXED >;

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
			m_buffer = MemoryAllocator::Allocate( m_total * sizeof( Object ) );
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
		void Cleanup()noexcept
		{
			if ( m_freeIndex != m_freeEnd )
			{
				ReportError< ePOOL_ERROR_TYPE_COMMON_MEMORY_LEAKS_DETECTED >( Namer::Name, size_t( ( m_freeEnd - m_freeIndex ) * sizeof( Object ) ) );
			}

			delete [] m_free;
			MemoryAllocator::Deallocate( m_buffer );
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
		 *\brief		Donne un chunk mémoire disponible.
		 *\return		nullptr s'il n'y a plus de place disponible, l'adresse mémoire sinon.
		 */
		Object * Allocate()noexcept
		{
			if ( m_freeIndex == m_free )
			{
				ReportError< ePOOL_ERROR_TYPE_COMMON_OUT_OF_MEMORY >( Namer::Name );
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

				*m_freeIndex++ = reinterpret_cast< Object * >( p_space );
				return true;
			}
			else
			{
				return true;
			}
		}

	private:
		//!\~english The buffer.	\~french Le tampon.
		uint8_t * m_buffer = nullptr;
		//!\~english Pointer to the buffer's end.	\~french Pointeur sur la fin du tampon.
		uint8_t * m_bufferEnd = nullptr;
		//!\~english The free chunks.	\~french Les chunks libres.
		Object ** m_free = nullptr;
		//!\~english The free chunks' end.	\~french La fin des chunks libres.
		Object ** m_freeEnd = nullptr;
		//!\~english The las allocated chunk.	\~french Le dernier chunk alloué.
		Object ** m_freeIndex = nullptr;
		//!\~english The total pool capacity.	\~french Le nombre total possible d'éléments.
		size_t m_total = 0;
	};
}

#endif
