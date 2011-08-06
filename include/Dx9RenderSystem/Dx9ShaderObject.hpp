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
#ifndef ___Dx9_ShaderObject___
#define ___Dx9_ShaderObject___

#include "Module_Dx9Render.hpp"

namespace Castor3D
{
	class Dx9ShaderObject : public HlslShaderObject
	{
		friend class Dx9ShaderProgram;

	protected:
		String m_compilerLog;
		Dx9ShaderProgram * m_pShaderProgram;
		ID3DXBuffer * m_pCompiledShader;
		ID3DXBuffer * m_pErrors;
		ID3DXConstantTable * m_pInputConstants;
		IDirect3DVertexDeclaration9 * m_pInputVariables;
		String m_strProfile;

	public:
		/**
		 * Constructor
		 */
		Dx9ShaderObject( Dx9ShaderProgram * p_pParent, eSHADER_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~Dx9ShaderObject();
		/**
		 * Compiles program
		 */
		virtual bool Compile();
		/**
		 * Get compiler messages
		 */
		void RetrieveCompilerLog( String & p_strCompilerLog);

		virtual void Bind()=0;

		virtual void Unbind()=0;
		/**
		 * Detaches this shader from it's program
		 */
		void Detach();
		/**
		 * Attaches this shader to the given program
		 */
		void AttachTo( Dx9ShaderProgram * p_pProgram);
		/**
		 * Destroys the HLSL Shader Program
		 */
		virtual void DestroyProgram();

		inline ID3DXConstantTable *		GetInputConstants		()const { return m_pInputConstants; }
		inline ID3DXConstantTable **	GetInputConstantsPtr	()		{ return & m_pInputConstants; }
	};

	//! Class holding Vertex Shader
	class Dx9VertexShader : public Dx9ShaderObject
	{
	private:
		IDirect3DVertexShader9 * m_pVertexShader;

	public:
		/**
		 * Constructor
		 */
		Dx9VertexShader( Dx9ShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~Dx9VertexShader();
		/**
		 * Creates the HLSL Shader program
		 */
		virtual void CreateProgram();
		virtual bool Compile();
		virtual void Bind();
		virtual void Unbind();
	};

	//! Class holding Fragment Shader
	class Dx9FragmentShader : public Dx9ShaderObject
	{
	private:
		IDirect3DPixelShader9 * m_pPixelShader;

	public:
		/**
		 * Constructor
		 */
		Dx9FragmentShader( Dx9ShaderProgram * p_pParent);
		/**
		 * Destructor
		 */
		virtual ~Dx9FragmentShader();
		/**
		 * Creates the HLSL Shader program
		 */
		virtual void CreateProgram();
		virtual bool Compile();
		virtual void Bind();
		virtual void Unbind();
	};
}

#endif