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
#ifndef ___Cg_ShaderProgram___
#define ___Cg_ShaderProgram___

#include "Module_CgShader.hpp"

namespace CgShader
{
	//! Cg Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CgShaderProgram : public Castor3D::ShaderProgramBase
	{
	public:
		/**
		 * Constructor
		 */
		CgShaderProgram( Castor3D::RenderSystem * p_pRenderSystem );
		/**
		 * Destructor
		 */
		virtual ~CgShaderProgram();

	private:
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags );
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags );
		virtual Castor::String DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass );
		virtual Castor::String DoGetDeferredPixelShaderSource( uint32_t p_uiFlags );
	};
}

#endif
