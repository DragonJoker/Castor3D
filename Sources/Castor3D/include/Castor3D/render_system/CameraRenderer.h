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
#ifndef ___C3D_CameraRenderer___
#define ___C3D_CameraRenderer___

#include "Renderer.h"

namespace Castor3D
{
	//! The Camera renderer
	/*!
	Applies the camera transformations, also removes it
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API CameraRenderer : public Renderer<Camera>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor
		 */
		CameraRenderer( RenderSystem * p_rs)
			:	Renderer<Camera>( p_rs)
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~CameraRenderer(){}
		/**
		 * Applies the transformations of the camera
		 */
		virtual void ApplyTransformations( const Vector3f & p_position, float * p_matrix) = 0;
		/**
		 * Removes the tranformations
		 */
		virtual void RemoveTransformations() = 0;
		/**
		* Returns the first object at mouse coords x and y
		*@param p_mode : [in] The selection mode (vertex, face, submesh, geometry)
		*@param p_found : [out] The found object (vertex, face, submesh, geometry)
		*@param x : [in] The x mouse coordinate
		*@param y : [in] The y mouse coordinate
		*@return true if something was found, false if not
		*/
		virtual bool Select( Scene * p_scene, SelectionMode p_mode, void ** p_found, int x, int y) = 0;
	};
	//! The Viewport renderer
	/*!
	Renders a viewport : applies it's fov, sets the display width and height, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API ViewportRenderer : public Renderer<Viewport>
	{
	public:
		/**
		 * Constructor
		 */
		ViewportRenderer( RenderSystem * p_rs)
			:	Renderer<Viewport>( p_rs)
		{}
		/**
		 * Destructor
		 */
		virtual ~ViewportRenderer(){}
		/**
		 * Renders the viewport's transformations
		 */
		virtual void Apply( ProjectionType p_type) = 0;
		/**
		 * Retrieves the viewport direction and returns it
		 */
		virtual Vector3f GetDirection( const Point2D<int> & p_mouse)=0;
	};
}

#endif
