/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include "Module_Dx9Render.h"

namespace Castor3D
{
	class C3D_Dx9_API Dx9ShaderObject : public HlslShaderObject
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
		String m_strMainFunction;
		Dx9FrameVariablePtrStrMap m_mapDx9Variables;

	public:
		/**
		 * Constructor
		 */
		Dx9ShaderObject( eSHADER_PROGRAM_TYPE p_eType, const String & p_strMainFunction);
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
		/**
		 * Detaches this shader from it's program
		 */
		void Detach();
		/**
		 * Attaches this shader to the given program
		 */
		void AttachTo( Dx9ShaderProgram * p_pProgram);
		/**
		 * Destroys the GL Shader Program
		 */
		virtual void DestroyProgram();

		virtual void Begin()=0;
		virtual void End()=0;
		Dx9FrameVariablePtr GetD3dFrameVariable( const String & p_strName);
		inline ID3DXConstantTable *	GetInputConstants	()const { return m_pInputConstants; }

		template <typename T>
		void AddFrameVariable( shared_ptr< OneFrameVariable<T> > p_pVariable)
		{
			if (m_mapDx9Variables.find( p_pVariable->GetName()) == m_mapDx9Variables.end())
			{
				shared_ptr< Dx9OneFrameVariable<T> > l_pVariable( new Dx9OneFrameVariable<T>( p_pVariable.get(), m_pInputConstants));
				HlslShaderObject::AddFrameVariable( p_pVariable);
				m_mapDx9Variables[p_pVariable->GetName()] = l_pVariable;
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( shared_ptr< PointFrameVariable<T, Count> > p_pVariable)
		{
			if (m_mapDx9Variables.find( p_pVariable->GetName()) == m_mapDx9Variables.end())
			{
				shared_ptr< Dx9PointFrameVariable<T, Count> > l_pVariable( new Dx9PointFrameVariable<T, Count>( p_pVariable.get(), m_pInputConstants));
				HlslShaderObject::AddFrameVariable( p_pVariable);
				m_mapDx9Variables[p_pVariable->GetName()] = l_pVariable;
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( shared_ptr< MatrixFrameVariable<T, Rows, Columns> > p_pVariable)
		{
			if (m_mapDx9Variables.find( p_pVariable->GetName()) == m_mapDx9Variables.end())
			{
				shared_ptr< Dx9MatrixFrameVariable<T, Rows, Columns> > l_pVariable( new Dx9MatrixFrameVariable<T, Rows, Columns>( p_pVariable.get(), m_pInputConstants));
				HlslShaderObject::AddFrameVariable( p_pVariable);
				m_mapDx9Variables[p_pVariable->GetName()] = l_pVariable;
			}
		}
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
		Dx9VertexShader();
		/**
		 * Destructor
		 */
		virtual ~Dx9VertexShader();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();
		virtual bool Compile();
		virtual void Begin();
		virtual void End();
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
		Dx9FragmentShader();
		/**
		 * Destructor
		 */
		virtual ~Dx9FragmentShader();
		/**
		 * Creates the GL Shader program
		 */
		virtual void CreateProgram();
		virtual bool Compile();
		virtual void Begin();
		virtual void End();
	};
}

#endif