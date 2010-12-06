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
		SharedPtr < GLPointFrameVariable<float, 4> >		m_pAmbients;
		SharedPtr < GLPointFrameVariable<float, 4> >		m_pDiffuses;
		SharedPtr < GLPointFrameVariable<float, 4> >		m_pSpeculars;
		SharedPtr < GLPointFrameVariable<float, 4> >		m_pPositions;
		SharedPtr < GLPointFrameVariable<float, 3> >		m_pAttenuations;
		SharedPtr < GLMatrixFrameVariable<float, 4, 4> >	m_pOrientations;
		SharedPtr < GLOneFrameVariable<float> >				m_pExponents;
		SharedPtr < GLOneFrameVariable<float> >				m_pCutOffs;

		std::set <int> m_setFreeLights;

		SharedPtr < GLPointFrameVariable<float, 4> > 	m_pAmbient;
		SharedPtr < GLPointFrameVariable<float, 4> > 	m_pDiffuse;
		SharedPtr < GLPointFrameVariable<float, 4> > 	m_pEmissive;
		SharedPtr < GLPointFrameVariable<float, 4> > 	m_pSpecular;
		SharedPtr < GLOneFrameVariable<float> > 		m_pShininess;
		bool m_bMatChanged;

		SharedPtr < GLPointFrameVariable<float, 4> > 	m_pAmbientLight;

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