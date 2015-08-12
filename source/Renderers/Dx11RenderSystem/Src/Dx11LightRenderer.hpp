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
#ifndef ___DX11_LIGHT_RENDERER_H___
#define ___DX11_LIGHT_RENDERER_H___

#include "Dx11RenderSystemPrerequisites.hpp"

#include <LightRenderer.hpp>
#include <Colour.hpp>
#include <Point.hpp>
#include <SquareMatrix.hpp>

namespace Dx11Render
{
	class DxLightRenderer
		:	public Castor3D::LightRenderer
	{
	public:
		DxLightRenderer( DxRenderSystem * p_pRenderSystem );
		virtual ~DxLightRenderer() {}

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

		inline void SetIndex( int p_iIndex )
		{
			m_iIndex = p_iIndex;
		}
		inline int GetIndex()const
		{
			return m_iIndex;
		}

	protected:
		void _enable();
		void _disable();

	protected:
		int m_iIndex;
		bool m_bChanged;
		Castor::Colour m_pAmbient;
		Castor::Colour m_pDiffuse;
		Castor::Colour m_pSpecular;
		Castor::Point4f m_pPosition;
		Castor::Point3f m_pAttenuation;
		Castor::Matrix4x4r m_pOrientation;
		float m_pExponent;
		float m_pCutOff;
		int m_iD3dIndex;
		int m_iD3dPreviousIndex;
	};
}

#endif