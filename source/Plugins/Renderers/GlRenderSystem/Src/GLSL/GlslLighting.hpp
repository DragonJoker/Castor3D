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
		class LightingModel
		{
		public:
			virtual void WriteCompute( uint64_t p_flags, GlslWriter & p_writer, Int const & i,
									   Vec3 & p_v3MapSpecular, Mat4 & c3d_mtxModelView, Vec3 & p_v3MapMormal,
									   Vec4 & c3d_v4MatAmbient, Vec4 & c3d_v4MatDiffuse, Vec4 & c3d_v4MatSpecular,
									   Vec3 & p_v3Position, Vec3 & p_v3Normal, Vec3 & p_v3Eye, Float & p_fShininess,
									   Vec3 & p_vtxVertex, Vec3 & p_vtxTangent, Vec3 & p_vtxBitangent, Vec3 & p_vtxNormal,
									   Vec3 & p_v3Ambient, Vec3 & p_v3Diffuse, Vec3 & p_v3Specular ) = 0;
			// Common ones
			C3D_Gl_API void Declare_Light( GlslWriter & p_writer );
			C3D_Gl_API void Declare_GetLight( GlslWriter & p_writer );
			// Calls
			C3D_Gl_API Light GetLight( Type const & p_value );
			// Main Computation
		};

		class BlinnPhongLightingModel
			: public LightingModel
		{
		public:
			C3D_Gl_API virtual void WriteCompute( uint64_t p_flags, GlslWriter & p_writer, Int const & i,
												  Vec3 & p_v3MapSpecular, Mat4 & c3d_mtxModelView, Vec3 & p_v3MapMormal,
												  Vec4 & c3d_v4MatAmbient, Vec4 & c3d_v4MatDiffuse, Vec4 & c3d_v4MatSpecular,
												  Vec3 & p_v3Position, Vec3 & p_v3Normal, Vec3 & p_v3Eye, Float & p_fShininess,
												  Vec3 & p_vtxVertex, Vec3 & p_vtxTangent, Vec3 & p_vtxBitangent, Vec3 & p_vtxNormal,
												  Vec3 & p_v3Ambient, Vec3 & p_v3Diffuse, Vec3 & p_v3Specular );
		};
	}
}

#include "GlslLighting.inl"

#endif
