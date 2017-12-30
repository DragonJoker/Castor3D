/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_FOG_H___
#define ___GLSL_FOG_H___

#include "Castor3DPrerequisites.hpp"

#include <GlslIntrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class Fog
		{
		public:
			C3D_API Fog( FogType flags
				, glsl::GlslWriter & writer );
			C3D_API void applyFog( glsl::Vec4 & colour
				, glsl::Float const & dist
				, glsl::Float const & y );

		protected:
			glsl::GlslWriter & m_writer;
		};
	}
}

#endif
