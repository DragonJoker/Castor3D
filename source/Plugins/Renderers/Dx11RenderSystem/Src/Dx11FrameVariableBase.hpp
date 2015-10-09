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
#ifndef ___DX11_FRAME_VARIABLE_BASE_H___
#define ___DX11_FRAME_VARIABLE_BASE_H___

#include "Dx11RenderSystemPrerequisites.hpp"

namespace Dx11Render
{
	template< typename Type > struct OneVariableBinder;
	template< typename Type, uint32_t Count > struct PointVariableBinder;
	template< typename Type, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder;

	class DxFrameVariableBase
	{
	public:
		DxFrameVariableBase( DxRenderSystem * p_renderSystem );
		virtual ~DxFrameVariableBase();

	protected:
		template< typename Type > void DoBind( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Count > void DoBind( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Rows, uint32_t Columns > void DoBind( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type > void DoUnbind( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Count > void DoUnbind( DxShaderProgram & p_program, Type * p_pValue );
		template< typename Type, uint32_t Rows, uint32_t Columns > void DoUnbind( DxShaderProgram & p_program, Type * p_pValue );

	protected:
		std::string m_stdstrName;
		bool m_bPresentInProgram;
		DxRenderSystem * m_renderSystem;
	};
}

#include "Dx11FrameVariableBase.inl"

#endif
