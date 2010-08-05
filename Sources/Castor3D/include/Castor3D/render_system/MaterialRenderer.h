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
#ifndef ___C3D_MaterialRenderer___
#define ___C3D_MaterialRenderer___

#include "Renderer.h"

#include "../geometry/Module_Geometry.h"

namespace Castor3D
{
	//! The TextureEnvironment renderer
	/*!
	Applies all the combination functions of a texture environment
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API TextureEnvironmentRenderer : public Renderer<TextureEnvironment>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor, only RenderSystem can use it
		 */
		TextureEnvironmentRenderer( RenderSystem * p_rs)
			:	Renderer<TextureEnvironment>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~TextureEnvironmentRenderer(){}
		/**
		 * Applies the combination functions
		 */
		virtual void Apply() = 0;
	};
	//! The TextureUnit renderer
	/*!
	Initialises a texture unit, draws it, removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API TextureRenderer : public Renderer<TextureUnit>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor, only RenderSystem can use it
		 */
		TextureRenderer( RenderSystem * p_rs)
			:	Renderer<TextureUnit>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~TextureRenderer(){ Cleanup(); }
		/**
		 * Cleans up this renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the texture, generates mipmapping...
		 */
		virtual bool Initialise() = 0;
		/**
		 * Draws the texture
		 */
		virtual void Apply() = 0;
		/**
		 * Removes the texture
		 */
		virtual void Remove() = 0;
	};

	//! The Pass renderer
	/*!
	Initialises a pass, draws it, removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API PassRenderer : public Renderer<Pass>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor, only RenderSystem can use it
		 */
		PassRenderer( RenderSystem * p_rs)
			:	Renderer<Pass>( p_rs)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~PassRenderer(){ Cleanup(); }
		/**
		 * Cleans up this renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the material
		 */
		virtual void Initialise() = 0;
		/**
		 * Applies the material
		 */
		virtual void Apply( Submesh * p_submesh, DrawType p_displayMode) = 0;
		/**
		 * Applies the material
		 */
		virtual void Apply( DrawType p_displayMode) = 0;
		/**
		 * Removes the material
		 */
		virtual void Remove() = 0;
	};
}

#endif
