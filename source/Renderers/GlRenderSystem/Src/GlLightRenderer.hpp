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
#ifndef ___GL_LIGHT_RENDERER_H___
#define ___GL_LIGHT_RENDERER_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <Light.hpp>

namespace GlRender
{
	class GlLightRenderer
		:	public Castor3D::LightRenderer
		,	public Castor::NonCopyable
	{
	protected:
		DECLARE_MAP( void *, bool, PointerBool );
		eGL_LIGHT_INDEX m_eIndex;
		Castor::Colour m_pAmbient;
		Castor::Colour m_pDiffuse;
		Castor::Colour m_pSpecular;
		Castor::Point4f m_pPosition;
		Castor::Point3f m_pAttenuation;
		Castor::Matrix4x4r m_pOrientation;
		float m_pExponent;
		float m_pCutOff;
		bool m_bChanged;
		int m_iGlIndex;
		int m_iGlPreviousIndex;
		PointerBoolMap m_mapChangedByProgram;
		OpenGl & m_gl;

	public:
		GlLightRenderer( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlLightRenderer() {}

		virtual void Initialise();
		virtual void Enable();
		virtual void Disable();
		virtual void ApplyAmbient();
		virtual void ApplyDiffuse();
		virtual void ApplySpecular();
		virtual void ApplyPosition();
		virtual void ApplyOrientation();
		virtual void ApplyConstantAtt( float p_constant );
		virtual void ApplyLinearAtt( float p_linear );
		virtual void ApplyQuadraticAtt( float p_quadratic );
		virtual void ApplyExponent( float p_exponent );
		virtual void ApplyCutOff( float p_cutOff );

		virtual void EnableShader( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void DisableShader( Castor3D::ShaderProgramBase * p_pProgram );
		virtual void ApplyAmbientShader();
		virtual void ApplyDiffuseShader();
		virtual void ApplySpecularShader();
		virtual void ApplyPositionShader();
		virtual void ApplyOrientationShader();
		virtual void ApplyConstantAttShader( float p_constant );
		virtual void ApplyLinearAttShader( float p_linear );
		virtual void ApplyQuadraticAttShader( float p_quadratic );
		virtual void ApplyExponentShader( float p_exponent );
		virtual void ApplyCutOffShader( float p_cutOff );

		inline int GetGlIndex()const
		{
			return m_iGlIndex;
		}
		inline int GetGlPreviousIndex()const
		{
			return m_iGlPreviousIndex;
		}
		inline eGL_LIGHT_INDEX GetIndex()const
		{
			return m_eIndex;
		}
		inline Castor::Colour GetAmbient()const
		{
			return m_pAmbient;
		}
		inline Castor::Colour GetDiffuse()const
		{
			return m_pDiffuse;
		}
		inline Castor::Colour GetSpecular()const
		{
			return m_pSpecular;
		}
		inline Castor::Point4f GetPosition()const
		{
			return m_pPosition;
		}
		inline Castor::Point3f GetAttenuation()const
		{
			return m_pAttenuation;
		}
		inline Castor::Matrix4x4r GetOrientation()const
		{
			return m_pOrientation;
		}
		inline float GetExponent()const
		{
			return m_pExponent;
		}
		inline float GetCutOff()const
		{
			return m_pCutOff;
		}

		inline void SetGlIndex( int p_iIndex )
		{
			m_iGlIndex = p_iIndex;
		}
		inline void SetGlPreviousIndex( int p_iIndex )
		{
			m_iGlPreviousIndex = p_iIndex;
		}
		inline void SetIndex( eGL_LIGHT_INDEX p_eIndex )
		{
			m_eIndex = p_eIndex;
		}
	};
}

#endif