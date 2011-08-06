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
#ifndef ___Gl_LightRenderer___
#define ___Gl_LightRenderer___

#include "Module_GlRender.hpp"

namespace Castor3D
{
	class GlLightRenderer : public LightRenderer
	{
	protected:
		typedef void (GlLightRenderer:: * ColourFunction)( Colour const & p_colour);
		typedef void (GlLightRenderer:: * OrientationFunction)( Matrix4x4r const & p_orientation);
		typedef void (GlLightRenderer:: * PositionFunction)( const Point4f & p_position);
		typedef void (GlLightRenderer:: * AttenuationComponentFunction)( float p_comp);
		typedef void (GlLightRenderer:: * SpotComponentFunction)( float p_comp);
		typedef void (GlLightRenderer:: * StateFunction)();
		typedef ColourFunction PColourFunction;
		typedef OrientationFunction POrientationFunction;
		typedef PositionFunction PPositionFunction;
		typedef AttenuationComponentFunction PAttenuationComponentFunction;
		typedef SpotComponentFunction PSpotComponentFunction;
		typedef StateFunction PStateFunction;

	protected:
		int								m_iIndex;
		Colour							m_pAmbient;
		Colour							m_pDiffuse;
		Colour							m_pSpecular;
		Point4f							m_pPosition;
		Point3f							m_pAttenuation;
		Matrix4x4r						m_pOrientation;
		float							m_pExponent;
		float							m_pCutOff;
		bool							m_bChanged;
		int								m_iGlIndex;
		int								m_iGlPreviousIndex;

		PStateFunction					m_pfnEnableFunction;
		PStateFunction					m_pfnDisableFunction;
		PColourFunction					m_pfnAmbientFunction;
		PColourFunction					m_pfnDiffuseFunction;
		PColourFunction					m_pfnSpecularFunction;
		POrientationFunction			m_pfnOrientationFunction;
		PPositionFunction				m_pfnPositionFunction;
		PAttenuationComponentFunction	m_pfnConstantAttFunction;
		PAttenuationComponentFunction	m_pfnLinearAttFunction;
		PAttenuationComponentFunction	m_pfnQuadraticAttFunction;
		PSpotComponentFunction			m_pfnExponentFunction;
		PSpotComponentFunction			m_pfnCutOffFunction;
#if CASTOR_USE_OPENGL4
		GlUboPoint4fVariablePtr 		m_pAmbients;
		GlUboPoint4fVariablePtr 		m_pDiffuses;
		GlUboPoint4fVariablePtr 		m_pSpeculars;
		GlUboPoint4fVariablePtr 		m_pPositions;
		GlUboPoint3fVariablePtr 		m_pAttenuations;
		GlUboMatrix4x4fVariablePtr 		m_pOrientations;
		GlUboFloatVariablePtr 			m_pExponents;
		GlUboFloatVariablePtr 			m_pCutOffs;
		GlUniformBufferObjectPtr		m_pUniformBuffer;
#endif
	public:
		GlLightRenderer();
		virtual ~GlLightRenderer(){}

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

		inline int			GetGlIndex			()const	{ return m_iGlIndex; }
		inline int			GetGlPreviousIndex	()const	{ return m_iGlPreviousIndex; }
		inline int			GetIndex			()const	{ return m_iIndex; }
		inline Point4f 		GetAmbient			()const	{ return m_pAmbient; }
		inline Point4f 		GetDiffuse			()const	{ return m_pDiffuse; }
		inline Point4f 		GetSpecular			()const	{ return m_pSpecular; }
		inline Point4f 		GetPosition			()const	{ return m_pPosition; }
		inline Point3f 		GetAttenuation		()const	{ return m_pAttenuation; }
		inline Matrix4x4f	GetOrientation		()const	{ return m_pOrientation; }
		inline float 		GetExponent			()const	{ return m_pExponent; }
		inline float 		GetCutOff			()const	{ return m_pCutOff; }

		inline void SetGlIndex			( int p_iIndex) { m_iGlIndex = p_iIndex; }
		inline void SetGlPreviousIndex	( int p_iIndex) { m_iGlPreviousIndex = p_iIndex; }
		inline void SetIndex			( int p_iIndex) { m_iIndex = p_iIndex; }


	protected:
		void _enable				();
		void _disable				();
		void _applyAmbient			( Colour const & p_ambient);
		void _applyDiffuse			( Colour const & p_diffuse);
		void _applySpecular			( Colour const & p_specular);
		void _applyPosition			( const Point4f & p_position);
		void _applyOrientation		( Matrix4x4r const & p_matrix);
		void _applyConstantAtt		( float p_constant);
		void _applyLinearAtt		( float p_linear);
		void _applyQuadraticAtt		( float p_quadratic);
		void _applyExponent			( float p_exponent);
		void _applyCutOff			( float p_cutOff);

		void _enableGlsl			();
		void _disableGlsl			();
		void _applyAmbientGlsl		( Colour const & p_ambient);
		void _applyDiffuseGlsl		( Colour const & p_diffuse);
		void _applySpecularGlsl		( Colour const & p_specular);
		void _applyPositionGlsl		( const Point4f & p_position);
		void _applyOrientationGlsl	( Matrix4x4r const & p_matrix);
		void _applyConstantAttGlsl	( float p_constant);
		void _applyLinearAttGlsl	( float p_linear);
		void _applyQuadraticAttGlsl	( float p_quadratic);
		void _applyExponentGlsl		( float p_exponent);
		void _applyCutOffGlsl		( float p_cutOff);
#if CASTOR_USE_OPENGL4
		void _enableGl4				();
#endif
	};
}

#endif