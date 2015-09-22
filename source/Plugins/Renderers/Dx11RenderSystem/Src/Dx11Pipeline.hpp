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

		virtual void ApplyViewport( int p_iWindowWidth, int p_iWindowHeight );
		virtual void ApplyMatrix( Castor::Matrix4x4r const & p_matrix, Castor::String const & p_name, Castor3D::FrameVariableBuffer & p_matrixBuffer );

	private:
		D3D11_VIEWPORT m_viewport;
	};
}

#endif
