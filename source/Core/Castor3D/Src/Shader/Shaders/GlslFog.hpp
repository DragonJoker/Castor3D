/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_FOG_H___
#define ___GLSL_FOG_H___

#include "Castor3DPrerequisites.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class Fog
		{
		public:
			C3D_API Fog( FogType flags
				, sdw::ShaderWriter & writer );
			C3D_API void applyFog( sdw::Vec4 const & bgColour
				, sdw::Float const & dist
				, sdw::Float const & y
				, sdw::Vec4 & colour );

		protected:
			sdw::Function< sdw::Vec4
				, sdw::InVec4
				, sdw::InVec4
				, sdw::InFloat
				, sdw::InFloat > m_fog;
		};
	}
}

#endif
