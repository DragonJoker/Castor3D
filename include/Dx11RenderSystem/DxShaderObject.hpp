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
#ifndef ___Dx11_ShaderObject___
#define ___Dx11_ShaderObject___

#include "Module_DxRender.hpp"

namespace Dx11Render
{
	class DxShaderObject : public Castor3D::ShaderObjectBase
	{
		friend class DxShaderProgram;
		typedef std::map< Castor::String, void* >	HandleStrMap;

	protected:
		Castor::String		m_strEntryPoint;
		DxShaderProgram *	m_pShaderProgram;
		Castor::String		m_strProfile;
		HandleStrMap		m_mapParamsByName;
		ID3DBlob *			m_pCompiled;
		DxRenderSystem *	m_pRenderSystem;

	public:
		/**
		 * Constructor
		 */
		DxShaderObject( DxShaderProgram * p_pParent, Castor3D::eSHADER_TYPE p_eType);
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
		void RetrieveCompilerLog( Castor::String & p_strCompilerLog);
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind()=0;
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind()=0;
		/**
		 *\~english
		 *\brief		Destroys the program on GPU
		 *\~french
		 *\brief		Détruit le programme sur le GPU
		 */
		virtual void DestroyProgram();

		virtual void CreateProgram(){}
		
		virtual	void					SetEntryPoint			( Castor::String const & p_strName)	{ m_strEntryPoint = p_strName;	}
		virtual	Castor::String			GetEntryPoint			()const								{ return m_strEntryPoint;		}

		inline ID3DBlob * GetCompiled()const { return m_pCompiled; }

	protected:
		virtual void DoRetrieveShader()=0;
	};

	//! Class holding Vertex Shader
	class DxVertexShader : public DxShaderObject
	{
	private:
		ID3D11VertexShader * m_pVertexShader;

	public:
		/**
		 * Constructor
		 */
		DxVertexShader( DxShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~DxVertexShader();
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind();

	private:
		void DoRetrieveShader();
	};

	//! Class holding Fragment Shader
	class DxFragmentShader : public DxShaderObject
	{
	private:
		ID3D11PixelShader * m_pPixelShader;

	public:
		/**
		 * Constructor
		 */
		DxFragmentShader( DxShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~DxFragmentShader();
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind();

	private:
		void DoRetrieveShader();
	};

	//! Class holding Geometry Shader
	class DxGeometryShader : public DxShaderObject
	{
	private:
		ID3D11GeometryShader * m_pGeometryShader;

	public:
		/**
		 * Constructor
		 */
		DxGeometryShader( DxShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~DxGeometryShader();
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind();

	private:
		void DoRetrieveShader();
	};

	//! Class holding Geometry Shader
	class DxHullShader : public DxShaderObject
	{
	private:
		ID3D11HullShader * m_pHullShader;

	public:
		/**
		 * Constructor
		 */
		DxHullShader( DxShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~DxHullShader();
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind();

	private:
		void DoRetrieveShader();
	};

	//! Class holding Geometry Shader
	class DxDomainShader : public DxShaderObject
	{
	private:
		ID3D11DomainShader * m_pDomainShader;

	public:
		/**
		 * Constructor
		 */
		DxDomainShader( DxShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~DxDomainShader();
		/**
		 *\~english
		 *\brief		Activates the shader
		 *\~french
		 *\brief		Active le shader
		 */
		virtual void Bind();
		/**
		 *\~english
		 *\brief		Deactivates the shader
		 *\~french
		 *\brief		Désactive le shader
		 */
		virtual void Unbind();

	private:
		void DoRetrieveShader();
	};
}

#endif