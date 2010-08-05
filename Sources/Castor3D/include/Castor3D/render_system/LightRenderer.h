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
#ifndef ___C3D_LightRenderer___
#define ___C3D_LightRenderer___

#include "Renderer.h"

namespace Castor3D
{
	//! The Light renderer
	/*!
	Renders a light, applies it's colours values...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API LightRenderer : public Renderer<Light>
	{
	protected:
		friend class RenderSystem;
		/**
		 * Constructor
		 */
		LightRenderer( RenderSystem * p_rs)
			:	Renderer<Light>( p_rs)
		{}
	public:
		/**
		 * Destructor
		 */
		virtual ~LightRenderer(){ Cleanup(); }
		/**
		 * Enables (shows) the light
		 */
		virtual void Enable() = 0;
		/**
		 * Disables (hides) the light
		 */
		virtual void Disable() = 0;
		/**
		 * Cleans up this renderer
		 */
		virtual void Cleanup(){}
		/**
		 * Applies ambient colour
		 */
		virtual void ApplyAmbient( float * p_ambient) = 0;
		/**
		 * Applies diffuse colour
		 */
		virtual void ApplyDiffuse( float * p_diffuse) = 0;
		/**
		 * Applies specular colour
		 */
		virtual void ApplySpecular( float * p_specular) = 0;
		/**
		 * Applies position
		 */
		virtual void ApplyPosition( float * p_position) = 0;
		/**
		 * Applies rotation
		 */
		virtual void ApplyOrientation( float * p_matrix) = 0;
		/**
		 * Applies constant attenuation
		 */
		virtual void ApplyConstantAtt( float p_constant) = 0;
		/**
		 * Applies linear attenuation
		 */
		virtual void ApplyLinearAtt( float p_linear) = 0;
		/**
		 * Applies quadratic attenuation
		 */
		virtual void ApplyQuadraticAtt( float p_quadratic) = 0;
		/**
		 * Applies exponent
		 */
		virtual void ApplyExponent( float p_exponent) = 0;
		/**
		 * Applies cut off
		 */
		virtual void ApplyCutOff( float p_cutOff) = 0;
	};
}

#endif
