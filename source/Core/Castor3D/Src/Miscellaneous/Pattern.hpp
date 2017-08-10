/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PATTERN_H___
#define ___C3D_PATTERN_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
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
	class Pattern
	{
	private:
		typedef T TObj;
		typedef T & TObjRef;
		typedef T const & TObjConstRef;
		typedef typename std::list< TObj > TObjList;

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
		Pointer getReversed()const
		{
			Pointer result = std::make_shared< Pattern< T > >();

			for ( typename TObjList::iterator it = m_listElements.begin(); it != m_listElements.end(); it++ )
			{
				result->m_listElements.push_front( *it );
			}

			return result;
		}
		/**
		 *\~english
		 *\brief		Tells if the pattern is closed (first element is also the last)
		 *\return		\p true if closed, \p false if not
		 *\~french
		 *\brief		Dit si le chemin est fermé (le premier élément est aussi le dernier)
		 *\return		\p true si fermé, \p false sinon
		 */
		bool isClosed()const
		{
			TObj const & t1 = **m_listElements.begin();
			TObj const & t2 = **m_listElements.rbegin();
			return t1 == t2;
		}
		/**
		 *\~english
		 *\brief		adds an element to the list, at a given index
		 *\param[in]	p_tElement	The element to add
		 *\param[in]	p_index	The index at which the insertion must be done
		 *\~french
		 *\brief		Ajoute un sommet, à l'indice donné
		 *\param[in]	p_tElement	L'élément à ajouter
		 *\param[in]	p_index	L'indice souhaité
		 */
		void addElement( TObjConstRef p_tElement, uint32_t p_index = 0xFFFFFFFF )
		{
			if ( p_index >= m_listElements.size() )
			{
				m_listElements.push_back( p_tElement );
			}
			else
			{
				typename TObjList::iterator it = m_listElements.begin();

				for ( uint32_t i = 0; i < p_index; i++ )
				{
					it++;
				}

				m_listElements.insert( it, p_tElement );
			}
		}
		/**
		 *\~english
		 *\return		The number of elements constituting the pattern
		 *\~french
		 *\return		Le nombre d'éléments dans le chemin
		 */
		inline uint32_t getSize()const
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
		TObjRef operator[]( uint32_t p_index )
		{
			REQUIRE( p_index < m_listElements.size() );
			typename TObjList::iterator it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				it++;
			}

			return * it;
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
		TObjConstRef operator[]( uint32_t p_index )const
		{
			REQUIRE( p_index < m_listElements.size() );
			typename TObjList::const_iterator it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				it++;
			}

			return * it;
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
		TObjRef getElement( uint32_t p_index )
		{
			REQUIRE( p_index < m_listElements.size() );
			typename TObjList::iterator it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				it++;
			}

			return * it;
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
		TObjConstRef getElement( uint32_t p_index )const
		{
			REQUIRE( p_index < m_listElements.size() );
			typename TObjList::const_iterator it = m_listElements.begin();

			for ( uint32_t i = 0; i < p_index; i++ )
			{
				it++;
			}

			return * it;
		}

	private:
		//!\~english	The elements.
		//!\~french		Les éléments.
		TObjList m_listElements;
	};
}

#endif
