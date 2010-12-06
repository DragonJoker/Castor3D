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
#ifndef ___GL3_ShaderProgram___
#define ___GL3_ShaderProgram___

#include "Module_GLRender.h"

namespace Castor3D
{
	struct MATERIAL_SHADER
	{
	};

	struct LIGHT_SHADER
	{
	};

	class C3D_GL3_API GL4ShaderProgram : public GLShaderProgram
	{
	private:
		Point4f						m_vAmbient;
		Point4f						m_vDiffuse;
		Point4f						m_vSpecular;
		Point4f						m_vEmissive;
		float						m_rShininess;
		GLUBOPoint4fVariablePtr 	m_pAmbient;
		GLUBOPoint4fVariablePtr 	m_pDiffuse;
		GLUBOPoint4fVariablePtr 	m_pEmissive;
		GLUBOPoint4fVariablePtr 	m_pSpecular;
		GLUBOFloatVariablePtr 		m_pShininess;
		GLUniformBufferObjectPtr	m_pMatsUniformBuffer;

		Point4f							m_vAmbients[8];
		Point4f							m_vDiffuses[8];
		Point4f							m_vSpeculars[8];
		Point4f							m_vPositions[8];
		Point3f							m_vAttenuations[8];
		Matrix4x4f						m_mOrientations[8];
		float							m_rExponents[8];
		float							m_rCutOffs[8];
		GLUBOPoint4fVariablePtr 		m_pAmbients;
		GLUBOPoint4fVariablePtr 		m_pDiffuses;
		GLUBOPoint4fVariablePtr 		m_pSpeculars;
		GLUBOPoint4fVariablePtr 		m_pPositions;
		GLUBOPoint3fVariablePtr 		m_pAttenuations;
		GLUBOMatrix4x4fVariablePtr 		m_pOrientations;
		GLUBOFloatVariablePtr 			m_pExponents;
		GLUBOFloatVariablePtr 			m_pCutOffs;
		GLUniformBufferObjectPtr		m_pLightsUniformBuffer;

		std::set <int> m_setFreeLights;

		GLUniformBufferObjectPtrStrMap m_mapUniformBuffers;

	public:
		GL4ShaderProgram();
		GL4ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GL4ShaderProgram();
		/**
		 * Link all Shaders
		 */
		virtual bool	Link();
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

		int AssignLight();
		void FreeLight( int p_iIndex);

		GLUniformBufferObjectPtr	GetUniformBuffer( const String & p_strName);
		void						SetLightValues( int p_iIndex, GL4LightRenderer * p_pLightRenderer);
	};
}

#endif