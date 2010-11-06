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
#ifndef ___GLLightRenderer___
#define ___GLLightRenderer___

#include "Module_GLRender.h"
#include "GLRenderSystem.h"
#include "GLUniformVariable.h"

namespace Castor3D
{

	class CS3D_GL_API GLLightRenderer : public LightRenderer
	{
		friend class GLRenderSystem;

	private:
		static size_t sm_uiNumLights;

	protected:
		size_t					m_iIndex;
		bool					m_bInitialised;
		Point<float, 4>			m_varAmbient;
		Point<float, 4>			m_varDiffuse;
		Point<float, 4>			m_varSpecular;
		Point<float, 4>			m_varPosition;
		Point<float, 3>			m_varAttenuation;
		Matrix4x4r				m_varOrientation;
		float					m_varExponent;
		float					m_varCutOff;
		GLUniformBufferObject *	m_pUniformBuffer;

	public:
		GLLightRenderer();
		virtual ~GLLightRenderer(){}

		virtual void Enable();
		virtual void Disable();
		virtual void ApplyAmbient( float * p_ambient);
		virtual void ApplyDiffuse( float * p_diffuse);
		virtual void ApplySpecular( float * p_specular);
		virtual void ApplyPosition( float * p_position);
		virtual void ApplyOrientation( real * p_matrix);
		virtual void ApplyConstantAtt( float p_constant);
		virtual void ApplyLinearAtt( float p_linear);
		virtual void ApplyQuadraticAtt( float p_quadratic);
		virtual void ApplyExponent( float p_exponent);
		virtual void ApplyCutOff( float p_cutOff);

		virtual void SetTarget( target_ptr p_target);

		int GetLightIndex() { return m_iIndex; }
		
		Point<float, 4> 	GetAmbient		() const { return m_varAmbient; }
		Point<float, 4> 	GetDiffuse		() const { return m_varDiffuse; }
		Point<float, 4> 	GetSpecular		() const { return m_varSpecular; }
		Point<float, 4> 	GetPosition		() const { return m_varPosition; }
		Point<float, 3> 	GetAttenuation	() const { return m_varAttenuation; }
		Matrix4x4r		 	GetOrientation	() const { return m_varOrientation; }
		float 				GetExponent		() const { return m_varExponent; }
		float 				GetCutOff		() const { return m_varCutOff; }
	};
}

#endif