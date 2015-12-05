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
#ifndef ___GLSL_LIGHTING_H___
#define ___GLSL_LIGHTING_H___

#include "GlslIntrinsics.hpp"

namespace GlRender
{
	namespace GLSL
	{
		template< typename LightingModel >
		class Lighting
		{
		public:
			// Common ones
			inline void Declare_Light( GlslWriter & p_writer );
			inline void Declare_GetLight( GlslWriter & p_writer );
			// LightingModel specifics
			inline void Declare_ComputeLightDirection( GlslWriter & p_writer );
			inline void Declare_Bump( GlslWriter & p_writer );
			inline void Declare_ComputeFresnel( GlslWriter & p_writer );
			// Calls
			inline Light GetLight( Type const & p_value );
			template< typename ... Values > inline Vec4 ComputeLightDirection( Type const & p_value, Values const & ... p_values );
			template< typename ... Values > inline void Bump( Type const & p_value, Values const & ... p_values );
			template< typename ... Values > inline Float ComputeFresnel( Type const & p_value, Values const & ... p_values );
		};

		struct BlinnPhongLightingModel
		{
			C3D_Gl_API static void Declare_ComputeLightDirection( GlslWriter & p_writer );
			C3D_Gl_API static void Declare_Bump( GlslWriter & p_writer );
			C3D_Gl_API static void Declare_ComputeFresnel( GlslWriter & p_writer );
		};
	}
}

#include "GlslLighting.inl"

#endif
