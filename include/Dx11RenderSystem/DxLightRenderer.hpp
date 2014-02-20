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
#ifndef ___Dx11_LightRenderer___
#define ___Dx11_LightRenderer___

#include "Module_DxRender.hpp"

namespace Dx11Render
{
	class DxLightRenderer : public Castor3D::LightRenderer
	{
	protected:
		int					m_iIndex;
		bool				m_bChanged;
		Castor::Colour		m_pAmbient;
		Castor::Colour		m_pDiffuse;
		Castor::Colour		m_pSpecular;
		Castor::Point4f		m_pPosition;
		Castor::Point3f		m_pAttenuation;
		Castor::Matrix4x4r	m_pOrientation;
		float				m_pExponent;
		float				m_pCutOff;
		int					m_iD3dIndex;
		int					m_iD3dPreviousIndex;

	public:
		DxLightRenderer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxLightRenderer(){}

		virtual void Initialise();

		virtual void Enable				();
		virtual void Disable			();
		virtual void ApplyAmbient		();
		virtual void ApplyDiffuse		();
		virtual void ApplySpecular		();
		virtual void ApplyPosition		();
		virtual void ApplyOrientation	();
		virtual void ApplyConstantAtt	( float p_constant);
		virtual void ApplyLinearAtt		( float p_linear);
		virtual void ApplyQuadraticAtt	( float p_quadratic);
		virtual void ApplyExponent		( float p_exponent);
		virtual void ApplyCutOff		( float p_cutOff);

		virtual void EnableShader				( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DisableShader				( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void ApplyAmbientShader			();
		virtual void ApplyDiffuseShader			();
		virtual void ApplySpecularShader		();
		virtual void ApplyPositionShader		();
		virtual void ApplyOrientationShader		();
		virtual void ApplyConstantAttShader		( float p_constant);
		virtual void ApplyLinearAttShader		( float p_linear);
		virtual void ApplyQuadraticAttShader	( float p_quadratic);
		virtual void ApplyExponentShader		( float p_exponent);
		virtual void ApplyCutOffShader			( float p_cutOff);

		inline void SetIndex( int p_iIndex) { m_iIndex = p_iIndex; }
		inline int GetIndex()const { return m_iIndex; }

	protected:
		void _enable				();
		void _disable				();
		void DoApplyAmbient			( Castor::Colour const & p_ambient);
		void DoApplyDiffuse			( Castor::Colour const & p_diffuse);
		void DoApplySpecular		( Castor::Colour const & p_specular);
		void DoApplyPosition		( Castor::Point4f const & p_position);
		void DoApplyOrientation		( Castor::Matrix4x4r const & p_matrix);
		void DoApplyConstantAtt		( float p_constant);
		void DoApplyLinearAtt		( float p_linear);
		void DoApplyQuadraticAtt	( float p_quadratic);
		void DoApplyExponent		( float p_exponent);
		void DoApplyCutOff			( float p_cutOff);
	};
}

#endif