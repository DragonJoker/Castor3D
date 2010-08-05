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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Arc___
#define ___C3D_Arc___


#include "../../material/Module_Material.h"

namespace Castor3D
{
	//! Arc handler class
	/*!
	An arc is a collection of consecutive points
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Arc
	{
	private:
		Vector3fPtrList m_vertex;	//!< The vertexes

	public:
		/**
		 * Constructor
		 */
		Arc();
		/**
		 * Destructor
		 */
		~Arc();
		/**
		 * Displays the arc in a display mode
		 *@param p_displayMode : [in] the mode in which the display must be made
		 */
		void Display( DrawType p_displayMode);
		/**
		 * Builds the mirror arc of this one
		 * @return the built arc
		 */
		Arc * GetReversed();
		/**
		 * Tells if the arc is closed (first vertex is also the last)
		 *@return true if closed, false if not
		 */
		bool IsClosed();
		/**
		 * Adds a vertex to the list, at a given index
		 *@param p_vertex : [in] the vertex to add
		 *@param p_index : [in] the index at which the insert must be done
		 */
		void AddVertex( Vector3f * p_vertex, int p_index);
		/**
		 * Retrieves the vertex at a given index, NULL if none
		 *@param p_index : The index we must look at
		 *@return The retrieved vertex
		 */
		Vector3f * GetVertex( unsigned int p_index);

	public:
		/**
		 * @return The number of vertex constituting this arc
		 */
		inline size_t GetNumVertex() { return m_vertex.size(); }
	};
}

#endif

