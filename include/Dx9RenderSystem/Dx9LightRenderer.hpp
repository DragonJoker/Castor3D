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
#ifndef ___Dx9_LightRenderer___
#define ___Dx9_LightRenderer___

#include "Module_Dx9Render.hpp"

namespace Castor3D
{
	class Dx9LightRenderer : public LightRenderer
	{
	protected:
		typedef void (Dx9LightRenderer:: * ColourFunction)( Colour const & p_colour);
		typedef void (Dx9LightRenderer:: * OrientationFunction)( Matrix4x4r const & p_orientation);
		typedef void (Dx9LightRenderer:: * PositionFunction)( const Point4f & p_position);
		typedef void (Dx9LightRenderer:: * AttenuationComponentFunction)( float p_comp);
		typedef void (Dx9LightRenderer:: * SpotComponentFunction)( float p_comp);
		typedef void (Dx9LightRenderer:: * StateFunction)();
		typedef ColourFunction PColourFunction;
		typedef OrientationFunction POrientationFunction;
		typedef PositionFunction PPositionFunction;
		typedef AttenuationComponentFunction PAttenuationComponentFunction;
		typedef SpotComponentFunction PSpotComponentFunction;
		typedef StateFunction PStateFunction;

	protected:
		int			m_iIndex;
		bool		m_bChanged;
		Colour		m_pAmbient;
		Colour		m_pDiffuse;
		Colour		m_pSpecular;
		Point4f		m_pPosition;
		Point3f		m_pAttenuation;
		Matrix4x4r	m_pOrientation;
		float		m_pExponent;
		float		m_pCutOff;
		int			m_iD3dIndex;
		int			m_iD3dPreviousIndex;

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

		D3DLIGHT9 m_dx9Light;

	public:
		Dx9LightRenderer();
		virtual ~Dx9LightRenderer(){}

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

		inline void SetIndex( int p_iIndex) { m_iIndex = p_iIndex; }
		inline int GetIndex()const { return m_iIndex; }

	protected:
		void _enable			();
		void _disable			();
		void _applyAmbient		( Colour const & p_ambient);
		void _applyDiffuse		( Colour const & p_diffuse);
		void _applySpecular		( Colour const & p_specular);
		void _applyPosition		( const Point4f & p_position);
		void _applyOrientation	( Matrix4x4r const & p_matrix);
		void _applyConstantAtt	( float p_constant);
		void _applyLinearAtt	( float p_linear);
		void _applyQuadraticAtt	( float p_quadratic);
		void _applyExponent		( float p_exponent);
		void _applyCutOff		( float p_cutOff);
	};
}

#endif