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
#ifndef ___C3D_Renderer___
#define ___C3D_Renderer___

namespace Castor3D
{
	//! The renderer representation
	/*!
	 * A renderer template, holds the object it renders and it's owner (the render system)
	 */
	template <class T>
	class Renderer
	{
	protected:
		T * m_target;					//!< The rendered targer
		RenderSystem * m_renderSystem;	//!< The render system

	public:
		/**
		 * Constructor
		 */
		Renderer( RenderSystem * p_rs)
			:	m_renderSystem( p_rs)
		{}

	public:
		inline T * GetTarget()const { return m_target; }
		inline void SetTarget( T * p_target) { m_target = p_target; }
	};
}

#endif
