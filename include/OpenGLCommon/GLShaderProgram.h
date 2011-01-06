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
#ifndef ___GL_ShaderProgram___
#define ___GL_ShaderProgram___

#include "Module_GLRender.h"

namespace Castor3D
{
	class GLShaderProgram : public ShaderProgram
	{
	protected:
		GLuint		m_programObject;
		String		m_linkerLog;
		GLFrameVariablePtrStrMap m_mapGLFrameVariables;

	public:
		GLShaderProgram();
		GLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GLShaderProgram();
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

		GLFrameVariablePtr GetGLFrameVariable( const String & p_strName);

		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, ShaderObjectPtr p_pPobject);

		template <typename T>
		void AddFrameVariable( typename SmartPtr< OneFrameVariable<T> >::Shared p_pVariable, ShaderObjectPtr p_pPobject)
		{
			ShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapGLFrameVariables.find( p_pVariable->GetName()) == m_mapGLFrameVariables.end())
				{
					typename SmartPtr< GLOneFrameVariable<T> >::Shared l_pGLVariable( new GLOneFrameVariable<T>( p_pVariable.get(), & m_programObject));
					m_mapGLFrameVariables.insert( GLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Count>
		void AddFrameVariable( typename SmartPtr< PointFrameVariable<T, Count> >::Shared p_pVariable, ShaderObjectPtr p_pPobject)
		{
			ShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapGLFrameVariables.find( p_pVariable->GetName()) == m_mapGLFrameVariables.end())
				{
					typename SmartPtr< GLPointFrameVariable<T, Count> >::Shared l_pGLVariable( new GLPointFrameVariable<T, Count>( p_pVariable.get(), & m_programObject));
					m_mapGLFrameVariables.insert( GLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

		template <typename T, size_t Rows, size_t Columns>
		void AddFrameVariable( typename SmartPtr< MatrixFrameVariable<T, Rows, Columns> >::Shared p_pVariable, ShaderObjectPtr p_pPobject)
		{
			ShaderProgram::AddFrameVariable( p_pVariable, p_pPobject);

			if ( ! p_pVariable == NULL)
			{
				if (m_mapGLFrameVariables.find( p_pVariable->GetName()) == m_mapGLFrameVariables.end())
				{
					typename SmartPtr< GLMatrixFrameVariable<T, Rows, Columns> >::Shared l_pGLVariable( new GLMatrixFrameVariable<T, Rows, Columns>( p_pVariable.get(), & m_programObject));
					m_mapGLFrameVariables.insert( GLFrameVariablePtrStrMap::value_type( p_pVariable->GetName(), static_pointer_cast<GLFrameVariableBase>( l_pGLVariable)));
				}
			}
		}

	public:
		inline GLuint								GetProgramObject		()const { return m_programObject; }
		inline const GLFrameVariablePtrStrMap &		GetGLFrameVariables		()const { return m_mapGLFrameVariables; }
	};
}

#endif