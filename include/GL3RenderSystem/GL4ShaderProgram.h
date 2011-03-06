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
#ifndef ___Gl4_ShaderProgram___
#define ___Gl4_ShaderProgram___

#include "Module_Gl3Render.h"

namespace Castor3D
{
	struct MATERIAL_SHADER
	{
	};

	struct LIGHT_SHADER
	{
	};

	class C3D_Gl3_API Gl4ShaderProgram : public GlShaderProgram
	{
	private:
		Point4f						m_vAmbient;
		Point4f						m_vDiffuse;
		Point4f						m_vSpecular;
		Point4f						m_vEmissive;
		float						m_rShininess;
		GlUboPoint4fVariablePtr 	m_pAmbient;
		GlUboPoint4fVariablePtr 	m_pDiffuse;
		GlUboPoint4fVariablePtr 	m_pEmissive;
		GlUboPoint4fVariablePtr 	m_pSpecular;
		GlUboFloatVariablePtr 		m_pShininess;
		GlUniformBufferObjectPtr	m_pMatsUniformBuffer;

		Point4f							m_vAmbients[8];
		Point4f							m_vDiffuses[8];
		Point4f							m_vSpeculars[8];
		Point4f							m_vPositions[8];
		Point3f							m_vAttenuations[8];
		Matrix4x4f						m_mOrientations[8];
		float							m_rExponents[8];
		float							m_rCutOffs[8];
		GlUboPoint4fVariablePtr 		m_pAmbients;
		GlUboPoint4fVariablePtr 		m_pDiffuses;
		GlUboPoint4fVariablePtr 		m_pSpeculars;
		GlUboPoint4fVariablePtr 		m_pPositions;
		GlUboPoint3fVariablePtr 		m_pAttenuations;
		GlUboMatrix4x4fVariablePtr 		m_pOrientations;
		GlUboFloatVariablePtr 			m_pExponents;
		GlUboFloatVariablePtr 			m_pCutOffs;
		GlUniformBufferObjectPtr		m_pLightsUniformBuffer;

		std::set <int> m_setFreeLights;

		GlUniformBufferObjectPtrStrMap m_mapUniformBuffers;

	public:
		Gl4ShaderProgram();
		Gl4ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~Gl4ShaderProgram();
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

		GlUniformBufferObjectPtr	GetUniformBuffer( const String & p_strName);
		void						SetLightValues( int p_iIndex, Gl4LightRenderer * p_pLightRenderer);
	};
}

#endif