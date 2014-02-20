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
#ifndef ___Gl_CameraRenderer___
#define ___Gl_CameraRenderer___

#include "Module_GlRender.hpp"

#include <Castor3D/Camera.hpp>

namespace GlRender
{
	class GlCameraRenderer : public Castor3D::CameraRenderer, CuNonCopyable
	{
	private:
		OpenGl &	m_gl;

	public:
		GlCameraRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlCameraRenderer();

		virtual bool Select( Castor3D::SceneSPtr p_pScene, Castor3D::Camera::eSELECTION_MODE p_eMode, int p_iX, int p_iY, Castor3D::stSELECT_RESULT & p_stFound );
		virtual void Render();
		virtual void EndRender();
	};
}

#endif