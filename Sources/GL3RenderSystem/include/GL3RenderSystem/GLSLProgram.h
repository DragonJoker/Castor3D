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
#ifndef ___GL_GLSLProgram___
#define ___GL_GLSLProgram___

#include "Module_GLSL.h"
#include "Module_GL.h"

namespace Castor3D
{
	struct MATERIAL_SHADER
	{
		Point<float, 4>	vAmbient;
		Point<float, 4>	vDiffuse;
		Point<float, 4>	vSpecular;
		Point<float, 4>	vEmissive;
		float			rShininess;

		GLUBOVariablePtr pAmbient;
		GLUBOVariablePtr pDiffuse;
		GLUBOVariablePtr pEmissive;
		GLUBOVariablePtr pSpecular;
		GLUBOVariablePtr pShininess;

		GLUniformBufferObjectPtr pUniformBuffer;
	};

	struct LIGHT_SHADER
	{
		Point<float, 4>		vAmbients[8];
		Point<float, 4>		vDiffuses[8];
		Point<float, 4>		vSpeculars[8];
		Point<float, 4>		vPositions[8];
		Point<float, 3>		vAttenuations[8];
		Matrix<real, 4, 4>	mOrientations[8];
		float				rExponents[8];
		float				rCutOffs[8];

		GLUBOVariablePtr pAmbients;
		GLUBOVariablePtr pDiffuses;
		GLUBOVariablePtr pSpeculars;
		GLUBOVariablePtr pPositions;
		GLUBOVariablePtr pAttenuations;
		GLUBOVariablePtr pOrientations;
		GLUBOVariablePtr pExponents;
		GLUBOVariablePtr pCutOffs;

		GLUniformBufferObjectPtr pUniformBuffer;
	};

	class CS3D_GL_API GLShaderProgram : public ShaderProgram
	{
	private:   
		GLuint		m_programObject;
		String		m_linkerLog;

		MATERIAL_SHADER m_materialShader;
		LIGHT_SHADER m_lightsShader;

		GLUniformVariablePtrStrMap m_mapGLUniformVariables;
		GLUniformBufferObjectPtrStrMap m_mapUniformBuffers;

	public:
		GLShaderProgram();
		GLShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GLShaderProgram();
		virtual void	Initialise();
		/**
		 * Link all Shaders
		 */
		virtual bool	Link();
		/**
		 * Get Linker Messages
		 */
		virtual void	RetrieveLinkerLog( String & strLog);
		/**
		 * Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void	Begin();
		/**
		 * Send uniform variables to the shader
		 */
		virtual void	ApplyAllVariables();
		/**
		 * Stop using this shader. OpenGL calls will go through regular pipeline.
		 */
		virtual void	End();

		virtual void	AddUniformVariable( UniformVariablePtr p_pUniformVariable);

		GLUniformBufferObjectPtr	GetUniformBuffer( const String & p_strName);
		GLint						GetUniformLocation(const GLchar *name);  //!< Retrieve Location (index) of a Uniform Variable
		void						SetLightValues( int p_iIndex, GLLightRenderer * p_pLightRenderer);

	private:
		GLUniformVariablePtr _createGLUniformVariable( UniformVariablePtr p_pUniformVariable);

	public:
		inline GLuint								GetProgramObject		()const { return m_programObject; }
		inline const GLUniformVariablePtrStrMap &	GetGLUniformVariables	()const { return m_mapGLUniformVariables; }
	};
}

#endif