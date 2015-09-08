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
#ifndef ___DX11_FRAGMENT_SHADER_H___
#define ___DX11_FRAGMENT_SHADER_H___

#include "Dx11ShaderObject.hpp"

namespace Dx11Render
{
	//! Class holding Fragment Shader
	class DxFragmentShader
		:	public DxShaderObject
	{
	public:
		/**
		 * Constructor
		 */
		DxFragmentShader( DxShaderProgram * p_pParent );
		/**
		 * Destructor
		 */
		virtual ~DxFragmentShader();
		/**
		 * Detaches this shader from it's program
		 */
		virtual void Detach();
		/**
		 * Attaches this shader to the given program
		 */
		virtual void AttachTo( Castor3D::ShaderProgramBase & p_program );

	private:
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void DoBind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		D�sactive le shader
		 */
		virtual void DoUnbind();

	private:
		ID3D11PixelShader * m_pPixelShader;
	};
}

#endif