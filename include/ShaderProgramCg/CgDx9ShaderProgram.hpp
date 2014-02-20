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
#ifndef ___Cg_DxShaderProgram___
#define ___Cg_DxShaderProgram___

#include "Module_CgShader.hpp"

#if defined( Cg_Direct3D9 )
#include "CgShaderProgram.hpp"

namespace CgShader
{
	class CgD3D9ShaderProgram : public CgShaderProgram
	{
	protected:
		Castor::String					m_linkerLog;
		CgD3D9FrameVariablePtrStrMap	m_mapCgDx9FrameVariables;

	public:
		CgD3D9ShaderProgram( Castor3D::RenderSystem * p_pRenderSystem );
		virtual ~CgD3D9ShaderProgram();

		void Begin( uint8_t p_byIndex, uint8_t p_byCount );
		int GetAttributeLocation( Castor::String const & CU_PARAM_UNUSED( p_strName ) )const{ return 0; }

	private:
		Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_eType );
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable	>	DoCreateTextureVariable(	int p_iNbOcc );
	};
}
#endif
#endif
