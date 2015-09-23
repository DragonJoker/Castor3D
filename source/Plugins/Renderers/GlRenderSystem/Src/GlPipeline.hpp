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
#ifndef ___GL_PIPELINE_H___
#define ___GL_PIPELINE_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Pipeline.hpp>

namespace GlRender
{
	class GlPipelineImpl
		: public Castor3D::IPipelineImpl
	{
	public:
		GlPipelineImpl( OpenGl & p_gl, Castor3D::Pipeline & p_pipeline );
		virtual ~GlPipelineImpl();

		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );
		virtual void Perspective( Castor::Matrix4x4r  & p_result, Castor::Angle const & p_aFOVY, real p_aspect, real p_near, real p_far );
		virtual void Frustum( Castor::Matrix4x4r  & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		virtual void Ortho( Castor::Matrix4x4r  & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		virtual void LookAt( Castor::Matrix4x4r & p_result, Castor::Point3r const & p_eye, Castor::Point3r const & p_center, Castor::Point3r const & p_up );

	protected:
		OpenGl & m_gl;
	};
}

#endif
