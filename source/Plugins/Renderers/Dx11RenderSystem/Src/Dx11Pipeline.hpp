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
#ifndef ___DX11_PIPELINE_H___
#define ___DX11_PIPELINE_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <Pipeline.hpp>

namespace Dx11Render
{
	class DxPipelineImpl
		: public Castor3D::IPipelineImpl
	{
	public:
		DxPipelineImpl( Castor3D::Pipeline & p_pipeline );
		virtual ~DxPipelineImpl();

		virtual void ApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, Castor3D::FrameVariableBuffer & p_matrixBuffer );
		virtual void ApplyViewport( int p_windowWidth, int p_windowHeight );
		virtual void Perspective( Castor::Matrix4x4r  & p_result, Castor::Angle const & p_fovy, real p_aspect, real p_near, real p_far );
		virtual void Frustum( Castor::Matrix4x4r  & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		virtual void Ortho( Castor::Matrix4x4r  & p_result, real p_left, real p_right, real p_bottom, real p_top, real p_near, real p_far );
		virtual void LookAt( Castor::Matrix4x4r & p_result, Castor::Point3r const & p_eye, Castor::Point3r const & p_center, Castor::Point3r const & p_up );

	private:
		D3D11_VIEWPORT m_viewport;
	};
}

#endif
