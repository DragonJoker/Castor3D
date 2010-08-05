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
#ifndef ___C3D_SceneRenderer___
#define ___C3D_SceneRenderer___

#include "Renderer.h"

namespace Castor3D
{
	//! The class which renders a SceneNode
	/*!
	This can apply the scene node transformations and can also remove it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API SceneNodeRenderer : public Renderer<SceneNode>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor, needs the owner, and only the owner can create it
		 */
		SceneNodeRenderer( RenderSystem * p_rs)
			:	Renderer<SceneNode>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~SceneNodeRenderer(){}
		/**
		 * Applies the scene node transformations
		 */
		virtual void ApplyTransformations() = 0;
		/**
		 * Removes the scene node transformations
		 */
		virtual void RemoveTransformations() = 0;
	};
}

#endif
