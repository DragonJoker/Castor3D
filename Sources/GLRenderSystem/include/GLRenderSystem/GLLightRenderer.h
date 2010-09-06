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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GLLightRenderer___
#define ___GLLightRenderer___

#include "Module_GLRender.h"
#include "GLRenderSystem.h"

namespace Castor3D
{
	class CS3D_GL_API GLLightRenderer : public LightRenderer
	{
		friend class GLRenderSystem;

	protected:
		static String GLLightRenderer::sm_vertexShader;
		static String GLLightRenderer::sm_fragmentShader;
		static std::vector <GLShaderProgram *> sm_pLightShaders;
		GLRenderSystem * m_glRenderSystem;
		int m_iIndex;

	public:
		GLLightRenderer( GLRenderSystem * p_rs)
			:	LightRenderer( p_rs),
				m_glRenderSystem( p_rs)
		{}
		virtual ~GLLightRenderer(){}

		virtual void Enable();
		virtual void Disable();
		virtual void ApplyAmbient( float * p_ambient);
		virtual void ApplyDiffuse( float * p_diffuse);
		virtual void ApplySpecular( float * p_specular);
		virtual void ApplyPosition( float * p_position);
		virtual void ApplyOrientation( float * p_matrix);
		virtual void ApplyConstantAtt( float p_constant);
		virtual void ApplyLinearAtt( float p_linear);
		virtual void ApplyQuadraticAtt( float p_quadratic);
		virtual void ApplyExponent( float p_exponent);
		virtual void ApplyCutOff( float p_cutOff);
	};
}

#endif