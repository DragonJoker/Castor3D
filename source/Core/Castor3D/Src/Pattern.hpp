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
#ifndef ___C3D_PATTERN_H___
#define ___C3D_PATTERN_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Pattern handler class
	\remark		A pattern is a collection of consecutive points
	\~french
	\brief		Classe de gestion de chemin
	\remark		Un chemin est une collection de points consécutifs
	*/
	template< typename T >
	class C3D_API Pattern
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Pattern()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Pattern()
		{
			m_listElements.clear();
		}
		/**
		 *\~english
		 *\brief		Builds the mirror pattern
		 *\return		The built pattern
		 *\~french
		 *\brief		Constuir le chemin miroir
		 *\return		Le chemin construit
		 */
		Pointer GetReversed()const
		{
			Pointer l_pReturn = std::make_shared< Pattern< T > >();

			for ( typename TObjList::iterator l_it = m_listElements.begin(); l_it != m_listElements.end(); l_it++ )
			{
				l_pReturn->m_listElements.push_front( *l_it );
			}

			return l_pReturn;
		}
		/**
		 *\~english
		 *\brief		Tells if the pattern is closed (first element is also the last)
		 *\return		\pt true if closed, \p false if not
		 *\~french
		 *\brief		Dit si le chemin est fermé (le premier élément est aussi le dernier)
		 *\return		\p true si fermé, \p false sinon
		 */
		bool IsClosed()const
		{
			TObj const & l_t1 = **m_listElements.begin();
			TObj const & l_t2 = **m_listElements.rbegin();
			return l_t1 == l_t2;
		}
		/**
		 *\~english
		 *\brief		Adds an element to the list, at a given index
		 *\param[in]	p_tElement	The element to add
		 *\param[in]	p_index	The index at which the insertion must be done
		 *\~french
		 *\brief		Ajoute un sommet, à l'indice donné
		 *\param[in]	p_tElement	L'élément à ajouter
		 *\param[in]	p_index	L'indice souhaité
		 */
		void AddElement( TObjConstRef p_tElement, uint32_t p_index = 0xFFFFFFFF )
		{
			if ( p_index >= m_listElements.size() )
			{
				m_listElements.push_back( p_tElement );
			}
			else
			{
				typename TObjList::iterator l_it = m_listElements.begin();

				for ( uint32_t i = 0; i < p_index; i++ )
				{
					l_it++;
				}

				m_listElements.insert( l_it, p_tElement );
			}
		}
		/**
		 *\~english
		 *\return		The number of elements constituting the pattern
		 *\~french
		 *\return		Le nombre d'éléments dans le chemin
		 */
		inline uint32_t GetSize()const
		{
			return uint32_t( m_listElements.size() );
		}
		/**
		 *\~english
		 *\brief		Accessor to an element
		 *\param[in]	p_index	the index of the wanted element
		 *\return		Modifiable element reference
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	p_index	Index de l'élément voulu
		 *\return		Une référence modifiable sur l'élément
		 */
		TObjRef operator []( uint32_t p_index )
		{
			CASTOR_ASSERT( p_index < m_listElements.size() );
			typename TObjList::iterator l_it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 *\~english
		 *\brief		Constant accessor to an element
		 *\param[in]	p_index	the index of the wanted element
		 *\return		Constant element reference
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	p_index	Index de l'élément voulu
		 *\return		Une référence constante sur l'élément
		 */
		TObjConstRef operator []( uint32_t p_index )const
		{
			CASTOR_ASSERT( p_index < m_listElements.size() );
			typename TObjList::const_iterator l_it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 *\~english
		 *\brief		Accessor to an element
		 *\param[in]	p_index	the index of the wanted element
		 *\return		Modifiable element
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	p_index	Index de l'élément voulu
		 *\return		Une référence modifiable sur l'élément
		 */
		TObjRef GetElement( uint32_t p_index )
		{
			CASTOR_ASSERT( p_index < m_listElements.size() );
			typename TObjList::iterator l_it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 *\~english
		 *\brief		Constant accessor to an element
		 *\param[in]	p_index	the index of the wanted element
		 *\return		Constant element
		 *\~french
		 *\brief		Accesseur sur les éléments
		 *\param[in]	p_index	Index de l'élément voulu
		 *\return		Une référence constante sur l'élément
		 */
		TObjConstRef GetElement( uint32_t p_index )const
		{
			CASTOR_ASSERT( p_index < m_listElements.size() );
			typename TObjList::const_iterator l_it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				l_it++;
			}

			return * l_it;
		}

	private:
		typedef T TObj;
		typedef T & TObjRef;
		typedef T const & TObjConstRef;
		typedef typename std::list< TObj > TObjList;
		//!\~english The elements	\~french Les éléments
		TObjList m_listElements;
	};
}

#endif
