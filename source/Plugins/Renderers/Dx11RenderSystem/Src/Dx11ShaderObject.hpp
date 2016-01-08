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
#ifndef ___DX11_SHADER_OBJECT_H___
#define ___DX11_SHADER_OBJECT_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <ShaderObject.hpp>

namespace Dx11Render
{
	class DxShaderObject
		: public Castor3D::ShaderObjectBase
	{
		friend class DxShaderProgram;
		typedef std::map< Castor::String, void * > HandleStrMap;

	public:
		/**
		 * Constructor
		 */
		DxShaderObject( DxShaderProgram * p_parent, Castor3D::eSHADER_TYPE p_type );
		/**
		 * Destructor
		 */
		virtual ~DxShaderObject();
		/**
		 *\copydoc		Castor3D::ShaderObjectBase::Create
		 */
		virtual void Create() {}
		/**
		 *\copydoc		Castor3D::ShaderObjectBase::Destroy
		 */
		virtual void Destroy();
		/**
		 *\copydoc		Castor3D::ShaderObjectBase::Compile
		 */
		virtual bool Compile();
		/**
		*\copydoc		Castor3D::ShaderObjectBase::SetEntryPoint
		*/
		virtual	void SetEntryPoint( Castor::String const & p_name )
		{
			m_strEntryPoint = p_name;
		}
		/**
		*\copydoc		Castor3D::ShaderObjectBase::GetEntryPoint
		*/
		virtual	Castor::String GetEntryPoint()const
		{
			return m_strEntryPoint;
		}

		inline ID3DBlob * GetCompiled()const
		{
			return m_compiledShader;
		}

	private:
		/**
		*\copydoc		Castor3D::ShaderObjectBase::DoRetrieveCompilerLog
		 */
		virtual Castor::String DoRetrieveCompilerLog();

	protected:
		Castor::String m_strEntryPoint;
		DxShaderProgram * m_shaderProgram;
		Castor::String m_strProfile;
		HandleStrMap m_mapParamsByName;
		ID3DBlob * m_compiledShader;
		ID3DBlob * m_errors;
		DxRenderSystem * m_renderSystem;
	};
}

#endif
