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
#ifndef ___GLCameraRenderer___
#define ___GLCameraRenderer___

#include "Module_GLRender.h"

namespace Castor3D
{
	class GLCameraRenderer : public CameraRenderer
	{
	public:
		GLCameraRenderer( RenderSystem * p_rs)
			:	CameraRenderer( p_rs)
		{}
		virtual ~GLCameraRenderer(){}

		virtual void ApplyTransformations( const Vector3f & p_position, float * p_matrix);
		virtual void RemoveTransformations();
		virtual bool Select( Scene * p_scene, SelectionMode p_mode, void ** p_found, int x, int y);
	};
	
	class GLViewportRenderer : public ViewportRenderer
	{
	public:
		GLViewportRenderer( RenderSystem * p_rs)
			:	ViewportRenderer( p_rs)
		{}
		virtual ~GLViewportRenderer(){}

		virtual void Apply( ProjectionType p_type);

		virtual Vector3f GetDirection( const Point2D<int> & p_mouse);
	};
}

#endif
