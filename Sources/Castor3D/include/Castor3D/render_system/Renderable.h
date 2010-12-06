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
#ifndef ___C3D_Renderable___
#define ___C3D_Renderable___

#include "Module_Render.h"
#include "Renderer.h"
#include "RenderSystem.h"
#include "../main/Serialisable.h"

namespace Castor3D
{
	//! The renderable base class.
	/*!
	A renderable is an object which can be rendered through a renderer
	This is a helper class to delegate the creation of the renderer
	_Renderable must be the child class
	_Renderer is the class that will render the child class
	\author Sylvain DOREMUS
	\version 0.1
	\date 29/10/2010
	*/
	template <class _Renderable, class _Renderer>
	class Renderable : public Serialisable, public MemoryTraced<_Renderable>
	{
	private:
		typedef Templates::SharedPtr<_Renderer> renderer_type;

	protected:
		renderer_type m_pRenderer;

	protected:
		/**
		 * Constructor, cannot be used 'as if'. You must derivate a class from this one
		 * to be able to create a Renderable
		 */
		Renderable()
		{
			m_pRenderer = RenderSystem::CreateRenderer<_Renderer>();

			if ( ! m_pRenderer.null())
			{
				m_pRenderer->SetTarget( (_Renderable *)this);
			}
		}
		/**
		 * Destructor
		 */
		virtual ~Renderable()
		{
		}

	public:
		/**
		 * Main render function, to be implemented by every derivated class
		 *@param p_displayMode : [in] information about the draw type (triangles, lines, ...)
		 */
		virtual void Render( eDRAW_TYPE p_displayMode)=0;
		/**
		 * Remove from render function, base implementation is dummy, so derivated classes
		 * may or not implement a new behaviour for it
		 */
		virtual void EndRender(){}
		/**
		 * 2D render function, base implementation is dummy, so derivated classes
		 * may or not implement a new behaviour for it
		 *@param p_displayMode : [in] information about the draw type (triangles, lines, ...)
		 */
		virtual void Render2D( eDRAW_TYPE p_displayMode){}
		/**
		 * Accessor to the renderer
		 */
		renderer_type GetRenderer() { return m_pRenderer; }
	};
}

#endif
