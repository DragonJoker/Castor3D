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
#ifndef ___GL2_LightRenderer___
#define ___GL2_LightRenderer___

#include "Module_GLRender.h"

namespace Castor3D
{
	class C3D_GL2_API GL2LightRenderer : public GLLightRenderer
	{
	private:
		Colour		m_pAmbient;
		Colour		m_pDiffuse;
		Colour		m_pSpecular;
		Point4f		m_pPosition;
		Point3f		m_pAttenuation;
		Matrix4x4r	m_pOrientation;
		float		m_pExponent;
		float		m_pCutOff;
		bool		m_bChanged;
		int			m_iGLIndex;
		int			m_iGLPreviousIndex;

	public:
		GL2LightRenderer();
		virtual ~GL2LightRenderer(){}

		virtual void Initialise();

		inline int GetGLIndex			()const { return m_iGLIndex; }
		inline int GetGLPreviousIndex	()const { return m_iGLPreviousIndex; }
		inline void SetGLIndex			( int p_iIndex) { m_iGLIndex = p_iIndex; }
		inline void SetGLPreviousIndex	( int p_iIndex) { m_iGLPreviousIndex = p_iIndex; }

	private:
		void _enable			();
		void _disable			();
		void _applyAmbient		( const Colour & p_ambient);
		void _applyDiffuse		( const Colour & p_diffuse);
		void _applySpecular		( const Colour & p_specular);
		void _applyPosition		( const Point4f & p_position);
		void _applyOrientation	( const Matrix4x4r & p_matrix);
		void _applyConstantAtt	( float p_constant);
		void _applyLinearAtt	( float p_linear);
		void _applyQuadraticAtt	( float p_quadratic);
		void _applyExponent		( float p_exponent);
		void _applyCutOff		( float p_cutOff);
	};
}

#endif