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
#ifndef ___DX9_FRAME_VARIABLE_BASE_H___
#define ___DX9_FRAME_VARIABLE_BASE_H___

#include "Dx9RenderSystemPrerequisites.hpp"
#include "Dx9TextureRenderer.hpp"

namespace Dx9Render
{
	template< typename Type > struct OneVariableApplyer;
	template< typename Type, uint32_t Count > struct PointVariableApplyer;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplyer;

	class DxFrameVariableBase
	{
	public:
		DxFrameVariableBase( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBase();

		virtual void Apply() = 0;

	protected:
		void GetVariableLocation( LPCSTR p_pVarName, ID3DXEffect * p_pEffect );
		template< typename Type > void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc );
		template< typename Type, int Count > void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc );
		template< typename Type, int Rows, int Columns > void DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc );

	protected:
		D3DXHANDLE m_d3dxHandle;
		bool m_bPresentInProgram;
		DxRenderSystem * m_pRenderSystem;
	};
}

#include "Dx9FrameVariableBase.inl"

#endif
