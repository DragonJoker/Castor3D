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
#ifndef ___Cg_GlShaderProgram___
#define ___Cg_GlShaderProgram___

#include "Module_CgShader.hpp"

#if defined( Cg_OpenGL )
#include "CgShaderProgram.hpp"

namespace CgShader
{
	class CgGlShaderProgram : public CgShaderProgram
	{
	protected:
		Castor::String		m_linkerLog;

	public:
		CgGlShaderProgram( Castor3D::RenderSystem * p_pRenderSystem );
		virtual ~CgGlShaderProgram();

		void Begin( uint8_t p_byIndex, uint8_t p_byCount );
		virtual int GetAttributeLocation( Castor::String const & p_strName)const;

	private:
		Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_eType);
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable	>	DoCreateTextureVariable(	int p_iNbOcc );
	};
}
#endif
#endif
