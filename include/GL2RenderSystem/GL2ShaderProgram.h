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
#ifndef ___GL2_ShaderProgram___
#define ___GL2_ShaderProgram___

#include "Module_GLRender.h"

namespace Castor3D
{
	class C3D_GL2_API GL2ShaderProgram : public GLShaderProgram
	{
	private:
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared		m_pAmbients;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared		m_pDiffuses;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared		m_pSpeculars;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared		m_pPositions;
		SmartPtr < GLPointFrameVariable<float, 3> >::Shared		m_pAttenuations;
		SmartPtr < GLMatrixFrameVariable<float, 4, 4> >::Shared	m_pOrientations;
		SmartPtr < GLOneFrameVariable<float> >::Shared			m_pExponents;
		SmartPtr < GLOneFrameVariable<float> >::Shared			m_pCutOffs;

		std::set <int> m_setFreeLights;

		SmartPtr < GLPointFrameVariable<float, 4> >::Shared 	m_pAmbient;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared 	m_pDiffuse;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared 	m_pEmissive;
		SmartPtr < GLPointFrameVariable<float, 4> >::Shared 	m_pSpecular;
		SmartPtr < GLOneFrameVariable<float> >::Shared 		m_pShininess;
		bool m_bMatChanged;

		SmartPtr < GLPointFrameVariable<float, 4> >::Shared 	m_pAmbientLight;

	public:
		GL2ShaderProgram();
		GL2ShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		virtual ~GL2ShaderProgram();
		/**
		 * Link all Shaders
		 */
		virtual bool	Link();
		/**
		 * Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void	Begin();

		int AssignLight();
		void FreeLight( int p_iIndex);

		void SetAmbientLight( const Point4f & p_crColour);
		void SetLightAmbient( int p_iIndex, const Point4f & p_crColour);
		void SetLightDiffuse( int p_iIndex, const Point4f & p_crColour);
		void SetLightSpecular( int p_iIndex, const Point4f & p_crColour);
		void SetLightPosition( int p_iIndex, const Point4f & p_ptPosition);
		void SetLightOrientation( int p_iIndex, const Matrix4x4r & p_mtxOrientation);
		void SetLightAttenuation( int p_iIndex, const Point3f & p_fAtt);
		void SetLightExponent( int p_iIndex, float p_fExp);
		void SetLightCutOff( int p_iIndex, float p_fCut);
		void SetMaterialAmbient( const Point4f & p_crColour);
		void SetMaterialDiffuse( const Point4f & p_crColour);
		void SetMaterialSpecular( const Point4f & p_crColour);
		void SetMaterialEmissive( const Point4f & p_crColour);
		void SetMaterialShininess( float p_fShine);
	};
}

#endif