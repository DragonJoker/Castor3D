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
#ifndef ___C3D_Pattern___
#define ___C3D_Pattern___

#include "Prerequisites.hpp"

namespace Castor3D
{
	//! Pattern handler class C3D_API
	/*!
	A pattern is a collection of consecutive points
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	template <typename T>
	class C3D_API Pattern : public MemoryTraced< Pattern<T> >
	{
	public:
		typedef typename Templates::shared_ptr< Pattern<T> >	Pointer;			//!< Pattern pointer
		typedef typename Container<Pointer>::Vector		PointerArray;		//!< Pattern pointer array

	private:
		typedef T										TObj;
		typedef T &										TObjRef;
		typedef T const &								TObjConstRef;
		typedef typename Container<TObj>::List			TObjList;
		TObjList m_listElements;	//!< The elements

	public:
		/**
		 * Constructor
		 */
		Pattern()
		{}
		/**
		 * Destructor
		 */
		~Pattern()
		{
			m_listElements.clear();
		}
		/**
		 * Builds the mirror pattern
		 * @return the built pattern
		 */
		Pointer GetReversed()
		{
			Pointer l_pReturn( new Pattern<T>());

			for (typename TObjList::iterator l_it = m_listElements.begin() ; l_it != m_listElements.end() ; l_it++)
			{
				l_pReturn->m_listElements.push_front( *l_it);
			}

			return l_pReturn;
		}
		/**@name Accessors */
		//@{
		/**
		 * Tells if the pattern is closed (first vertex is also the last)
		 *@return true if closed, false if not
		 */
		bool IsClosed()
		{
			TObj l_t1, l_t2;
			l_t1 = **m_listElements.begin();
			l_t2 = **m_listElements.rbegin();
			return l_t1 == l_t2;
		}
		/**
		 * Adds a vertex to the list, at a given index
		 *@param p_tElement : [in] the vertex to add
		 *@param p_uiIndex : [in] the index at which the insertion must be done
		 */
		void AddElement( TObjConstRef p_tElement, size_t p_uiIndex)
		{
			if (p_uiIndex >= m_listElements.size())
			{
				m_listElements.push_back( p_tElement);
				return;
			}

			typename TObjList::iterator l_it = m_listElements.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			m_listElements.insert( l_it, p_tElement);
		}
		/**
		 * @return The number of vertex constituting me
		 */
		inline size_t GetSize()
		{
			return m_listElements.size();
		}
		/**
		 * Accessor to an element
		 *@param p_uiIndex : [in] the index of the wanted element
		 *@return modifiable element
		 */
		TObjRef operator []( size_t p_uiIndex)
		{
			CASTOR_ASSERT( p_uiIndex < m_listElements.size());
			typename TObjList::iterator l_it = m_listElements.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 * Constant accessor to an element
		 *@param p_uiIndex : [in] the index of the wanted element
		 *@return constant element
		 */
		TObjConstRef operator []( size_t p_uiIndex)const
		{
			CASTOR_ASSERT( p_uiIndex < m_listElements.size());
			typename TObjList::const_iterator l_it = m_listElements.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 * Accessor to an element
		 *@param p_uiIndex : [in] the index of the wanted element
		 *@return modifiable element
		 */
		TObjRef GetElement( size_t p_uiIndex)
		{
			CASTOR_ASSERT( p_uiIndex < m_listElements.size());
			typename TObjList::iterator l_it = m_listElements.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			return * l_it;
		}
		/**
		 * Constant accessor to an element
		 *@param p_uiIndex : [in] the index of the wanted element
		 *@return constant element
		 */
		TObjConstRef GetElement( size_t p_uiIndex)const
		{
			CASTOR_ASSERT( p_uiIndex < m_listElements.size());
			typename TObjList::const_iterator l_it = m_listElements.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			return * l_it;
		}
		//@}
	};
}

#endif

