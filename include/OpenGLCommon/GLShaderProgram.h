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
#ifndef ___Gl_ShaderProgram___
#define ___Gl_ShaderProgram___

#include "Module_GlRender.h"

namespace Castor3D
{
	class GlShaderProgram : public GlslShaderProgram
	{
	protected:
		GLuint		m_programObject;
		String		m_linkerLog;
		GlFrameVariablePtrStrMap m_mapGlFrameVariables;

	public:
		GlShaderProgram();
		GlShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GlShaderProgram();
		virtual void Cleanup();
		virtual void Initialise();
		/**
		 * Link all Shaders
		 */
		virtual bool Link();
		/**
		 * Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( String & strLog);
		/**
		 * Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void Begin();
		/**
		 * Send uniform variables to the shader
		 */
		virtual void ApplyAllVariables();
		/**
		 * Stop using this shader. OpenGL calls will go through regular pipeline.
		 */
		virtual void End();

		GlFrameVariablePtr GetGlFrameVariable( const String & p_strName);

		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, GlslShaderObjectPtr p_pPobject);

		template <typename T>
		void AddFrameVariable( shared_ptr< OneFrameVariable<T> > p_pVariable, GlslShaderObjectPtr p_pPobject)
		{
			GlslShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if (p_pVariable != NULL)
			{
				if (m_mapGlFrameVariables.find( p_pVariable->GetName()) == m_mapGlFrameVariables.end())
				{
					shared_ptr< GlOneFrameVariable<T> > l_pGLVariable( new GlOneFrameVariable<T>( p_pVariable.get(), & m_programObject));
					m_mapGlFrameVariables.insert( GlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( shared_ptr< PointFrameVariable<T, Count> > p_pVariable, GlslShaderObjectPtr p_pPobject)
		{
			GlslShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if (p_pVariable != NULL)
			{
				if (m_mapGlFrameVariables.find( p_pVariable->GetName()) == m_mapGlFrameVariables.end())
				{
					shared_ptr< GlPointFrameVariable<T, Count> > l_pGLVariable( new GlPointFrameVariable<T, Count>( p_pVariable.get(), & m_programObject));
					m_mapGlFrameVariables.insert( GlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( shared_ptr< MatrixFrameVariable<T, Rows, Columns> > p_pVariable, GlslShaderObjectPtr p_pPobject)
		{
			GlslShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if (p_pVariable != NULL)
			{
				if (m_mapGlFrameVariables.find( p_pVariable->GetName()) == m_mapGlFrameVariables.end())
				{
					shared_ptr< GlMatrixFrameVariable<T, Rows, Columns> > l_pGLVariable( new GlMatrixFrameVariable<T, Rows, Columns>( p_pVariable.get(), & m_programObject));
					m_mapGlFrameVariables.insert( GlFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GlFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

	public:
		inline GLuint								GetProgramObject		()const { return m_programObject; }
		inline const GlFrameVariablePtrStrMap &		GetGlFrameVariables		()const { return m_mapGlFrameVariables; }
	};
}

#endif
