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
Place - Suite 330, Boston, MA 02101-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___DX10_FRAME_VARIABLE_BASE_H___
#define ___DX10_FRAME_VARIABLE_BASE_H___

#include "Dx10RenderSystemPrerequisites.hpp"
#include "Dx10RenderSystem.hpp"
#include "Dx10TextureRenderer.hpp"

namespace Dx10Render
{
	template< typename Type > struct OneVariableApplier;
	template< typename Type, uint32_t Count > struct PointVariableApplier;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplier;

	class DxFrameVariableBase
	{
	public:
		DxFrameVariableBase( DxRenderSystem * p_pRenderSystem );
		virtual ~DxFrameVariableBase();

		virtual void Apply() = 0;

	protected:
#if C3DDX10_USE_DXEFFECTS
		void GetVariableLocation( LPCSTR p_pVarName, ID3D10Effect * p_pEffect );
#else
#endif
		template< typename Type > void DoApply( DxShaderProgram & p_program, Type * p_pVariable, uint32_t p_uiOcc );
		template< typename Type, uint32_t Count > void DoApply( DxShaderProgram & p_program, Type * p_pVariable, uint32_t p_uiOcc );
		template< typename Type, uint32_t Rows, uint32_t Columns > void DoApply( DxShaderProgram & p_program, Type * p_pVariable, uint32_t p_uiOcc );

	protected:
		bool m_bPresentInProgram;
		DxRenderSystem * m_pRenderSystem;
#if C3DDX10_USE_DXEFFECTS
		ID3D10EffectVariable * m_pVariable;
#else
#endif
	};
}

#include "Dx10FrameVariableBase.inl"

#endif
