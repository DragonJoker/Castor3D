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
#ifndef ___C3D_Renderer___
#define ___C3D_Renderer___

#include "../Prerequisites.h"

namespace Castor3D
{
	class RendererBase
	{
	protected:
		SceneManager * m_pSceneManager;

	public:
		RendererBase( SceneManager * p_pSceneManager)
			:	m_pSceneManager( p_pSceneManager)
		{}
		virtual ~RendererBase()
		{}
	};
	//! The renderer representation
	/*!
	A renderer template, holds the object it renders
	_Renderable must be the rendered class (which must derivate from Renderable)
	_Renderer is the child class
	\author Sylvain DOREMUS
	\version 0.1
	\date 29/10/2010
	*/
	template <class _Renderable, class _Renderer>
	class Renderer : public RendererBase, public MemoryTraced<_Renderer>
	{
	protected:
		typedef _Renderable * target_ptr;
		target_ptr m_target;

	protected:
		/**
		 * Constructor, cannot be used 'as if'. You must derivate a class from this one
		 * to be able to create a Renderer
		 */
		Renderer( SceneManager * p_pSceneManager)
			:	RendererBase( p_pSceneManager)
			,	m_target( NULL)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~Renderer()
		{
		}

	public:
		/**@name Accessors */
		//@{
		inline target_ptr	GetTarget	()const { return m_target; }

		virtual void	SetTarget	( target_ptr p_target)	{ m_target = p_target; }
		//@}
	};
}

#endif
