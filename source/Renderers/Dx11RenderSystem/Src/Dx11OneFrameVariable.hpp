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
#ifndef ___DX11_ONE_FRAME_VARIABLE_H___
#define ___DX11_ONE_FRAME_VARIABLE_H___

#include <OneFrameVariable.hpp>

#include "Dx11FrameVariableBase.hpp"

namespace Dx11Render
{
	template< typename T >
	class DxOneFrameVariable
		:	public Castor3D::OneFrameVariable< T >
		,	public DxFrameVariableBase
	{
	public:
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc );
		DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::OneFrameVariable< T > * p_pVariable );
		virtual ~DxOneFrameVariable();

		virtual bool Initialise()
		{
			return true;
		}
		virtual void Cleanup() {}
		virtual void Bind();
		virtual void Unbind();
		virtual void Apply();
	};
}

#include "Dx11OneFrameVariable.inl"

#endif
