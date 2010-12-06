/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_Arc___
#define ___C3D_Arc___

#include "../../material/Module_Material.h"

namespace Castor3D
{
	//! Pattern handler class
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
		typedef Templates::SharedPtr< Pattern<T> >	Pointer;
		typedef C3DVector(	Pointer)				PointerArray;		//!< Pattern pointer array

	private:
		typedef T				TObj;
		typedef T &				TObjRef;
		typedef const T &		TObjConstRef;
		typedef C3DList( TObj)	TObjList;
		TObjList m_listPoints;	//!< The points

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
			m_listPoints.clear();
		}
		/**
		 * Displays the pattern in a display mode
		 *@param p_displayMode : [in] the mode in which the display must be made
		 */
		void Display( eDRAW_TYPE p_displayMode){}
		/**
		 * Builds the mirror pattern
		 * @return the built arc
		 */
		Pointer GetReversed()
		{
			Pointer l_result( new Pattern<T>());
			TObjList::iterator l_it = m_listPoints.begin();

			for (l_it = m_listPoints.begin() ; l_it != m_listPoints.end() ; l_it++)
			{
				l_result->m_listPoints.push_front( *l_it);
			}

			return l_result;
		}

	public:
		/**@name Accessors */
		//@{
		/**
		 * Tells if the pattern is closed (first vertex is also the last)
		 *@return true if closed, false if not
		 */
		bool IsClosed()
		{
			TObj l_v1, l_v2;
			l_v1 = **m_listPoints.begin();
			l_v2 = **m_listPoints.rbegin();
			return l_v1 == l_v2;
		}
		/**
		 * Adds a vertex to the list, at a given index
		 *@param p_vertex : [in] the vertex to add
		 *@param p_index : [in] the index at which the insert must be done
		 */
		void AddPoint( TObjConstRef p_vertex, size_t p_index)
		{
			if (p_index >= m_listPoints.size())
			{
				m_listPoints.push_back( p_vertex);
				return;
			}

			TObjList::iterator l_it = m_listPoints.begin();

			for (size_t i = 0 ; i < p_index ; i++)
			{
				l_it++;
			}

			m_listPoints.insert( l_it, p_vertex);
		}
		/**
		 * @return The number of vertex constituting me
		 */
		inline size_t GetSize() { return m_listPoints.size(); }

		TObjRef operator []( size_t p_uiIndex)
		{
			CASTOR_ASSERT( p_uiIndex < m_listPoints.size());
			TObjList::iterator l_it = m_listPoints.begin();

			for (size_t i = 0 ; i < p_uiIndex ; i++)
			{
				l_it++;
			}

			return * l_it;
		}
		TObjConstRef operator []( size_t p_uiIndex)const
		{
			CASTOR_ASSERT( p_uiIndex < m_listPoints.size());
			TObjList::const_iterator l_it = m_listPoints.begin();

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

