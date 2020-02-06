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
#ifndef ___DX10_SHADER_OBJECT_H___
#define ___DX10_SHADER_OBJECT_H___

#include "Dx10RenderSystemPrerequisites.hpp"

#include <ShaderObject.hpp>

namespace Dx10Render
{
	class DxShaderObject
		:	public Castor3D::ShaderObjectBase
	{
		friend class DxShaderProgram;
		typedef std::map< Castor::String, void * >	HandleStrMap;

	public:
		/**
		 * Constructor
		 */
		DxShaderObject( DxShaderProgram * p_pParent, Castor3D::eSHADER_TYPE p_eType );
		/**
		 * Destructor
		 */
		virtual ~DxShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Get compiler messages
		 */
		void RetrieveCompilerLog( Castor::String & p_strCompilerLog );
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		D�sactive le shader
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Destroys the program on GPU
		 *\~french
		 *\brief		D�truit le programme sur le GPU
		 */
		virtual void DestroyProgram();

		virtual void CreateProgram() {}

		virtual	void					SetEntryPoint( Castor::String const & p_strName )
		{
			m_strEntryPoint = p_strName;
		}
		virtual	Castor::String			GetEntryPoint()const
		{
			return m_strEntryPoint;
		}

		inline ID3DBlob * GetCompiled()const
		{
			return m_pCompiled;
		}

	protected:
		virtual void DoRetrieveShader() = 0;

	protected:
		Castor::String m_strEntryPoint;
		DxShaderProgram * m_pShaderProgram;
		Castor::String m_strProfile;
		HandleStrMap m_mapParamsByName;
		ID3DBlob * m_pCompiled;
		DxRenderSystem * m_pRenderSystem;
	};
}

#endif